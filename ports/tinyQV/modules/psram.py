import vfs

from _psram import PSRAMBlockDevice

def mkramfs(size=1024 * 64, mount_point="/ramfs", offset=256 * 1024, debug=False):
    psram = PSRAMBlockDevice(offset, size)

    try:
        fs = vfs.VfsLfs2(psram, progsize=64)
    except OSError:
        vfs.VfsLfs2.mkfs(psram, progsize=64)
        fs = vfs.VfsLfs2(psram, progsize=64)

    vfs.mount(fs, mount_point)
