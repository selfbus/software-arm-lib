.section .rodata
.global incbin_bl_start
.balign 16
incbin_bl_start:
.incbin "bootloader.bin" // path to bootloader.bin is set under "Project Properties->MCU Assembler->General->Include paths (-I)"

.global incbin_bl_end
.balign 1
incbin_bl_end:
.byte 0
