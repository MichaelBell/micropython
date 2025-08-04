import gc
gc.threshold(128 * 1024)

import vfs

from _psram import PSRAMBlockDevice

psram = PSRAMBlockDevice()

try:
    fs = vfs.VfsLfs2(psram, progsize=64)
except OSError:
    vfs.VfsLfs2.mkfs(psram, progsize=64)
    fs = vfs.VfsLfs2(psram, progsize=64)

vfs.mount(fs, '/')

# Hack for ECP5 testing at 56MHz - set the UART divider for 115200
import machine
machine.mem32[0x800_0088] = 486 # 555 // 2
