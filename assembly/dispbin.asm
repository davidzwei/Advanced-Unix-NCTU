; dispbin:
;         given a number in AX, store the corresponding bit string in str1.
;         for example, if AX = 0x1234, the result should be:
;         str1 = 0001001000111000
; ======
;       str1 @ 0x600000-600014
; ======
; Enter your codes: (type 'done:' when done)
L1:
    cmp ebx, 15
    jge L2
    mov ecx, 0
    sub ecx, ebx
    
    inc ebx
    jmp L1
L2:
done: