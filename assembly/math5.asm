; ~  nc up.zoolab.org 2514                                                      INT ✘  60s  tang@tang-MS-7C02  15:52:57
;math5: 32-bit signed arithmetic
;        var3 = (var1 * -var2) / (var3 - ebx)
;        note: overflowed part should be truncated
;======
;      var1 @ 0x600000-600004
;      var2 @ 0x600004-600008
;      var3 @ 0x600008-60000c
;======
;Enter your codes: (type 'done:' when done)

mov eax, [0x600000]
sub ecx, [0x600004]
imul ecx

neg ebx
add ebx, [0x600008]
mov edx, 0
idiv ebx
mov [0x600008], eax
done: 