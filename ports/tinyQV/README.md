# The tinyQV port

This port is based on the minimal MicroPython port, targeting tinyQV as on Tiny Tapeout sky25a.

## Building for ttsky25a tinyQV

The Makefile expects the [tinyQV-sdk](https://github.com/MichaelBell/tinyQV-sdk) to be a sibling
directory of the micropython directory.  Make sure to checkout the `ttsky25a` branch.

Additionally, it requires an RV32E compatible [Risc-V toolchain](https://github.com/MichaelBell/riscv-gnu-toolchain) in /opt/tinyQV

To build:

    $ make clean
    $ make submodules
    $ make

Building will produce the build/firmware.bin file suitable for loading onto the [QSPI Pmod](https://github.com/mole99/qspi-pmod).

## Running tinyQV

### Once ttsky25a is delivered

Use the [TinyQV Programmer](https://tinyqv.rebel-lion.uk/) to program and launch the firmware.

This will give you a MicroPython REPL on the tinyQV UART at 115200 baud.  However, note that TinyQV UART has a bug which is worked around by sending all characters twice.  The tinyQV SDK has a workaround for this which requires every character to be sent twice.  The console in the TinyQV Programmer implements this workaround, but if you are interacting with the UART in another way you must be aware of it.

### Testing on FPGA

I'm using an ECP5 board that sits in the TT demoboard.  There's a programmer in my [TT-ECP5](https://github.com/MichaelBell/tt-ecp5) repo.

## Using tinyQV Micropython

The Micropython build is currently basic, without more advanced modules.  I expect to expand that as the port progresses.

The machine module provides a Pin object for accessing the inputs and outputs.  Outputs are pins 0-7 and inputs are pins 8-15.

For example

    for i in range(256):
      for j in range(2,8):
        Pin(j).value((i >> j) & 1)
    
will cycle the top 6 outputs (visible on the 7 segment display on the TT demo board).  Note out0 is used for UART, so if you configure it as an output pin then the UART will stop working.

Below test about SPI and SD is not currently valid - we need a SPI peripheral for the SoC!

An SPI module is also provided, allowing use of the hard SPI block, which uses the following pins:

    ui[2]: "SPI MISO"
    uo[3]: "SPI MOSI"
    uo[4]: "SPI CS"
    uo[5]: "SPI SCK"

The basic instantiation is `spi = machine.SPI()`, which provides an SPI at 16MHz clock using the above pins.

Additional arguments are:

- `divisor`: 2, 4, 6 or 8.  Default 4.  Divides the 64MHz clock to provide the SPI clock.
- `read_latency`: 0 or 1, default 0.  A value of 1 delays the sampling of read data by half an SPI clock cycle, which may be required when using a small divisor.
- `use_cs`: boolean, default `True`.  Automatically sets the CS line low when an SPI transfer is requested.  Set to `False` to manually control the CS.

Once initialised the [standard machine.SPI](https://docs.micropython.org/en/latest/library/machine.SPI.html#machine-spi) functions are available.

## SD Card

An SD card library is included, this works with the SPI pins.

Example usage:

    from sd import SDCard
    import os
    from machine import Pin
    
    sd = SDCard(Pin(4))
    os.mount(sd, "/sd")
    os.listdir("/sd")

## TODO

* Implement interrupt (Ctrl-C)
* Full interface support not just REPL - get Thonny working
* Infrastructure for user peripherals
