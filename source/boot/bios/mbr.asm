bits 16
org 0x7c00

mbr:
jmp boot:
nop



boot:

times 510-($-$$) db 0
db 0x55, 0xaa~