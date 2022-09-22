;  ~  nc up.zoolab.org 2507                                                      INT ✘  27s  tang@tang-MS-7C02  17:03:42
; isolatebit:
;         get the value bit-11 ~ bit-5 in AX and store the result in val1
;         (zero-based bit index)
; ======
;       val1 @ 0x600000-600001
;       val2 @ 0x600001-600002
; ======
; Enter your codes: (type 'done:' when done)

; 0000 1111 1110 0000
; AND AX, 0xFE0
shl AX, 4
shr AX, 9
mov [0x600000], AL ; only take 8bit(1 bytes) to 0x600000, cannot write to 0x600001
done: