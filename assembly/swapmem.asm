;  ~  nc up.zoolab.org 2519                                                           INT ✘  tang@tang-MS-7C02  17:22:02
; swapmem: swap the values in val1 and val2
; ======
;       val1 @ 0x600000-600008
;       val2 @ 0x600008-600010
; ======
; Enter your codes: (type 'done:' when done)

mov RAX, [0x600000]
mov RBX, [0x600008]
mov [0x600008], RAX
mov [0x600000], RBX
done: