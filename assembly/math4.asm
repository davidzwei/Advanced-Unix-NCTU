; ~  nc up.zoolab.org 2513                                                   INT ✘  2m 19s  tang@tang-MS-7C02  15:35:37
;math4: 32-bit signed arithmetic
;        var4 = (var1 * -5) / (-var2 % var3)
;        note: overflowed part should be truncated
;======
;      var1 @ 0x600000-600004
;      var2 @ 0x600004-600008
;      var3 @ 0x600008-60000c
;      var4 @ 0x60000c-600010
;======
;Enter your codes: (type 'done:' when done)

mov eax, [0x600000]
mov ebx, -5
imul ebx
mov ecx, eax

mov eax, 0x0
sub eax, [0x600004]
mov ebx, [0x600008]
mov edx, 0
idiv ebx

mov ebx, edx
mov eax, ecx
mov edx, 0
idiv ebx 
mov [0x60000c], eax
done: