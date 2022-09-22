;  ~  nc up.zoolab.org 2511                                                      INT ✘  31s  tang@tang-MS-7C02  02:45:00
; math2: signed arithmetic
;         eax = (-var1 * var2) + var3
; ======
;       var1 @ 0x600000-600004
;       var2 @ 0x600004-600008
;       var3 @ 0x600008-60000c
; ======
; Enter your codes: (type 'done:' when done)
sub eax, [0x600000]
mul dword PTR [0x600004]
add eax, [0x600008]