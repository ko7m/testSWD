// Generated by vscode-hexdump
//
// R0 - Address of data to perform CRC computation
// R1 - Length in bytes
//
//   0:   2400            movs    r4, #0			; Set r4 to 0xffffffff by doing a bitwise not of the value 0
//   2:   43e4            mvns    r4, r4
//   4:   4b06            ldr     r3, [pc, #24]		; set r3 to 0xedb88320 (Polynomial for CRC32 (CRC32_POLY))
//   6:   7802            ldrb    r2, [r0, #0]		; Get the next byte of data
//   8:   3001            adds    r0, #1			; Increment the data pointer along
//   a:   4054            eors    r4, r2			;
//   c:   2208            movs    r2, #8			;
//   e:   0864            lsrs    r4, r4, #1		;
//  10:   d300            bcc.n   0x14
//  12:   405c            eors    r4, r3			; crc = (crc >>1) ^ CRC32_POLY
//  14:   3a01            subs    r2, #1
//  16:   d1fa            bne.n   0xe
//  18:   3901            subs    r1, #1
//  1a:   d1f4            bne.n   0x6				; iterate over 8 bits
//  1c:   43e0            mvns    r0, r4			; crc = crc ^ ~0
//  1e:   be00            bkpt    0x0000
//  20:   8320            .byte   0x20, 0x83
//  22    edb8            .byte   0xb8, 0xed

0x00, 0x24, 0xe4, 0x43, 0x06, 0x4b, 0x02, 0x78,
0x01, 0x30, 0x54, 0x40, 0x08, 0x22, 0x64, 0x08,
0x00, 0xd3, 0x5c, 0x40, 0x01, 0x3a, 0xfa, 0xd1,
0x01, 0x39, 0xf4, 0xd1, 0xe0, 0x43, 0x00, 0xbe,
0x20, 0x83, 0xb8, 0xed,
