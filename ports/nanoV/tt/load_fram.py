import time
import sys
import rp2
import machine
from machine import Pin

from ttboard.mode import RPMode
from ttboard.demoboard import DemoBoard

from pio_spi import PIOSPI

CMD_WRITE = 0x02
CMD_READ = 0x03
CMD_READ_SR1 = 0x05
CMD_WEN = 0x06
CMD_ID  = 0x9F

@rp2.asm_pio(autopush=True, push_thresh=32, in_shiftdir=rp2.PIO.SHIFT_RIGHT)
def pio_capture():
    wait(0, gpio, 23)
    wait(1, gpio, 23)
    wrap_target()
    in_(pins, 8)


def load_program(prog):
    tt = DemoBoard()
    tt.shuttle.tt_um_MichaelBell_nanoV.enable()

    # Put project into reset, this sets the SPI pins to bidir.
    tt.reset_project(True)

    cs = Pin(22, Pin.OUT)
    cs.on()
    spi = PIOSPI(1, Pin(21), Pin(24), Pin(23), freq=1000000)

    capture = False
    if capture:
        sm = rp2.StateMachine(0, pio_capture, 2_000_000, in_base=Pin(21))

        capture_len=1024
        buf = bytearray(capture_len)

        rx_dma = rp2.DMA()
        c = rx_dma.pack_ctrl(inc_read=False, treq_sel=4) # Read using the SM0 RX DREQ
        sm.restart()
        rx_dma.config(
            read=0x5020_0020,        # Read from the SM0 RX FIFO
            write=buf,
            ctrl=c,
            count=capture_len//4,
            trigger=True
        )
        sm.active(1)

    def flash_cmd(data, dummy_len=0, read_len=0):
        dummy_buf = bytearray(dummy_len)
        read_buf = bytearray(read_len)
        
        cs.off()
        spi.write(bytearray(data))
        if dummy_len > 0:
            spi.readinto(dummy_buf)
        if read_len > 0:
            spi.readinto(read_buf)
        cs.on()
        
        return read_buf

    def flash_cmd2(data, data2):
        cs.off()
        spi.write(bytearray(data))
        spi.write(data2)
        cs.on()

    def print_bytes(data):
        for b in data: print("%02x " % (b,), end="")
        print()
    
    cs.off()
    id = flash_cmd([CMD_ID], 0, 4)
    cs.on()
    
    if capture:
        for j in range(4):
            print("%02d: " % (j+21,), end="")
            for d in buf:
                print("-" if (d & (1 << j)) != 0 else "_", end = "")
            print()
        del sm
        del rx_dma
    
    
    print_bytes(id)

    flash_cmd([CMD_WEN])

    with open(prog, "rb") as f:
    #if False:
        buf = bytearray(4096)
        sector = 0
        while True:
            num_bytes = f.readinto(buf)
            #print_bytes(buf[:512])
            if num_bytes == 0:
                break
            
            flash_cmd2([CMD_WRITE, sector >> 4, ((sector & 0xF) << 4), 0], buf[:num_bytes])
            print(".", end="")
            sector += 1
        print()
        print("Program done")

    with open(prog, "rb") as f:
        data = bytearray(256)
        i = 0
        while True:
            num_bytes = f.readinto(data)
            if num_bytes == 0:
                break
            
            data_from_flash = flash_cmd([CMD_READ, i >> 8, i & 0xFF, 0], 0, num_bytes)
            for j in range(num_bytes):
                if data[j] != data_from_flash[j]:
                    raise Exception(f"Error at {i:02x}:{j:02x}: {data[j]} != {data_from_flash[j]}")
            i += 1

    print("Verify done")
    data_from_flash = flash_cmd([CMD_READ, 0, 0, 0], 0, 16)
    print_bytes(data_from_flash)
    
    del spi
    for i in range(21,25):
        Pin(i, Pin.IN)
