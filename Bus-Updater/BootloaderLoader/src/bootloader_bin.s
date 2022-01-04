.section .rodata
.global incbin_bl_start
.balign 16
incbin_bl_start:
.incbin "Bus-Updater.bin"

.global incbin_bl_end
.balign 1
incbin_bl_end:
.byte 0
