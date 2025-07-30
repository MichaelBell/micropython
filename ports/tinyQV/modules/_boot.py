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
