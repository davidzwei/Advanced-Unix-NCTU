;~  nc up.zoolab.org 2512                                                           INT ✘  tang@tang-MS-7C02  15:33:13
;math3: 32-bit unsigned arithmetic
;        var4 = (var1 * 5) / (var2 - 3)
;        note: overflowed part should be truncated
;======
;      var1 @ 0x600000-600004
;      var2 @ 0x600004-600008
;      var4 @ 0x600008-60000c
;======
;Enter your codes: (type 'done:' when done)
add eax, [0x600000]
mov ebx, 5
mul ebx
mov ecx, [0x600004]
sub ecx, 3
lea edx, [0]
div ecx
mov [0x600008], eax
done:
