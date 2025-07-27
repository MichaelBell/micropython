import gc
gc.threshold(128 * 1024)

import psram
psram.mkramfs(256 * 1024, "/", 0)

# Hack for ECP5 testing at 56MHz - set the UART divider for 115200
import machine
machine.mem32[0x800_0088] = 486 # 555 // 2
