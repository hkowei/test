        lw 0 1 num
        beq 0 0 skip
        halt
skip    add 1 1 1
        halt
num     .fill 2
