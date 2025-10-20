 lw 0 1 mcand
 lw 0 2 mplier
 lw 0 7 one
 add 0 0 3
 lw 0 6 fiftin
 noop
loop nor 7 7 4
 nor 1 1 5
 nor 4 5 5
 noop
 beq 5 0 noadd
 add 3 2 3
noadd add 2 2 2
 add 7 7 7
 beq 7 6 skip
 beq 0 0 loop
skip noop
 halt
fiftin .fill 32768
one .fill 1
mcand .fill 6203
mplier .fill 1429