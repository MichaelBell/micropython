# Based on Gadgetoid's PSRAM block device for Presto

import machine
import vfs


PSRAM_BASE = 0x1800000
PSRAM_SIZE = 8 * 1024 * 1024

class PSRAMBlockDevice:
    def __init__(self, size, offset=None, blocksize=512, debug=False):
        self.debug = debug
        self.blocks, remainder = divmod(size, blocksize)

        if remainder:
            raise ValueError(f"Size should be a multiple of {blocksize:0,d}")

        self.blocksize = blocksize

        if offset is None:
            offset = PSRAM_SIZE - size

        self.offset = PSRAM_BASE | offset

    def readblocks(self, block_num, buf, offset=0):
        if self.debug:
            print(f"PSRAM: readblocks: {block_num} {len(buf)}, {offset}")
        src = self.offset + (block_num * self.blocksize) + offset
        for i in range(len(buf)):
            buf[i] = machine.mem8[src+i]

    def writeblocks(self, block_num, buf, offset=0):
        if self.debug:
            print(f"PSRAM: writeblocks: {block_num} {len(buf)}, {offset}")
        dest = self.offset + (block_num * self.blocksize) + offset
        for i in range(len(buf)):
            machine.mem8[dest+i] = buf[i]

    def eraseblock(self, block_num):
        if self.debug:
            print(f"PSRAM: eraseblock: {block_num}")
        dest = self.offset + (block_num * self.blocksize)
        for i in range(0, self.blocksize, 4):
            machine.mem32[dest+i] = 0

    def ioctl(self, op, arg):
        if self.debug:
            print(f"PSRAML: ioctl: {op} {arg}")
        if op == 1:  # Initialize
            return None
        if op == 2:  # Shutdown
            return None
        if op == 3:  # Sync
            return 0
        if op == 4:  # Block Count
            return self.blocks
        if op == 5:  # Block Size
            return self.blocksize
        if op == 6:  # Erase
            self.eraseblock(arg)
            return 0


def mkramfs(size=1024 * 64, mount_point="/ramfs", offset=None, debug=False):
    psram = PSRAMBlockDevice(size, offset=offset, debug=debug)

    try:
        fs = vfs.VfsLfs2(psram, progsize=256)
    except OSError:
        vfs.VfsLfs2.mkfs(psram, progsize=256)
        fs = vfs.VfsLfs2(psram, progsize=256)

    vfs.mount(fs, mount_point)
