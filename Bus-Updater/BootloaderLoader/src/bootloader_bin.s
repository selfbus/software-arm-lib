.section .rodata
.global incbin_bl_start
.balign 16
incbin_bl_start:
.incbin "Bus-Updater.bin" // path to Bus-Updater.bin is set under "Project Properties->MCU Assembler->General->Include paths (-I)"

.global incbin_bl_end
.balign 1
incbin_bl_end:
.byte 0
