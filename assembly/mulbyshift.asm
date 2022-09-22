;  ~  nc up.zoolab.org 2516                                                              ✔ │ tang@tang-MS-7C02  16:50:33
; mulbyshift: multiply val1 by 26 and store the result in val2
; ======
;       val1 @ 0x600000-600004
;       val2 @ 0x600004-600008
; ======
; Enter your codes: (type 'done:' when done)

mov eax, [0x600000]
shl eax, 1
add ebx, eax
shl eax, 2 
add ebx, eax
shl eax, 1    
add ebx, eax
mov [0x600004], ebx
done:
