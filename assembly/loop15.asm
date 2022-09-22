;  ~  nc up.zoolab.org 2509                                                              ✔ │ tang@tang-MS-7C02  20:22:32
; loop15:
;         str1 is a string contains 15 lowercase and uppercase alphbets.
;         implement a loop to convert all alplabets to lowercase,
;         and store the result in str2.
; ======
;       str1 @ 0x600000-600010
;       str2 @ 0x600010-600020
; ======
; Enter your codes: (type 'done:' when done)
L1:
    cmp eax, 15
    jge L2
    mov ebx, [0x600000 + eax]
    or ebx, 0x20
    mov [0x600010 + eax], ebx
    inc eax
    jmp L1
L2:
done: