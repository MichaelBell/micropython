import machine

def get_base_address(peripheral_num):
    if peripheral_num < 1 or peripheral_num >= 40:
        raise ValueError(f"Peripheral number {peripheral_num} is out of range")
    
    if peripheral_num < 16:
        return 0x800_0000 + 0x40 * peripheral_num
    elif peripheral_num < 32:
        return 0x800_0400 + 0x10 * (peripheral_num - 16)
    else:
        return 0x800_0600 + 0x40 * (peripheral_num - 32)

def get_peripheral_address(peripheral_num, address):
    base_address = get_base_address(peripheral_num)

    if 16 <= peripheral_num < 32:
        if address < 0 or address >= 16:
            raise ValueError(f"Address {address} out of range for peripheral {peripheral_num}")
    else:
        if address < 0 or address >= 64:
            raise ValueError(f"Address {address} out of range for peripheral {peripheral_num}")

    return base_address + address

def read_byte_reg(peripheral_num, address):
    return machine.mem8[get_peripheral_address(peripheral_num, address)]

def write_byte_reg(peripheral_num, address, value):
    machine.mem8[get_peripheral_address(peripheral_num, address)] = (value & 0xff)

def read_hword_reg(peripheral_num, address):
    return machine.mem16[get_peripheral_address(peripheral_num, address)]

def write_hword_reg(peripheral_num, address, value):
    machine.mem16[get_peripheral_address(peripheral_num, address)] = (value & 0xffff)

def read_word_reg(peripheral_num, address):
    return machine.mem32[get_peripheral_address(peripheral_num, address)]

def write_word_reg(peripheral_num, address, value):
    machine.mem32[get_peripheral_address(peripheral_num, address)] = (value & 0xffffffff)
