        lw      0       1       val1    # R1 = 5
        lw      0       2       val2    # R2 = 5
        beq     1       2       skip    # 如果相等跳过下一条
        add     0       0       3       # 不执行
skip    add     1       2       3       # R3 = R1 + R2
        halt
val1    .fill   5
val2    .fill   5
