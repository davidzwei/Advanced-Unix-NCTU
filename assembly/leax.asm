;  ~  nc up.zoolab.org 2508                                                   INT ✘  1m 47s  tang@tang-MS-7C02  02:07:36
; leax:
;         eax = edi * 2
;         ebx = edi * 3
;         ecx = edi * 5
;         edx = edi * 9
; ======
; ======
; Enter your codes: (type 'done:' when done)
mov esi, edi
shl edi, 1
mov eax, edi
add ebx, eax
add ebx, esi

add ecx, eax
add ecx, ebx

add edx, eax
add edx, eax
add edx, ecx
