        lw      0       1       val1    # R1 = 10
        sw      1       0       store   # mem[store] = R1
        lw      0       2       store   # R2 = mem[store]
        halt
val1    .fill   10
store   .fill   0
