;  ~  nc up.zoolab.org 2522                                                   INT ✘  5m 55s  tang@tang-MS-7C02  17:33:51
; ul+lu: convert the alphabet in CH from upper to lower or from lower to upper
; ======
; ======
; Enter your codes: (type 'done:' when done)

; 65->97(32)
; (A) 0100 0001 = 0x41, 
; (a) 0110 0001 = 0x61
xor CH, 0x20
done:
