        lw 0 1 x
        lw 0 2 y
        add 1 2 3
        nor 3 1 4
        sw 0 3 res
        beq 1 2 equal
        noop
        jalr 0 7
equal   halt
x       .fill 7
y       .fill 7
res     .fill 0
