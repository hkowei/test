        lw      0       1       val1    # R1 = 4
        lw      0       4       subAdd # R4 = 地址 sub
        jalr    4       5               # 调用 sub, R5=返回地址
        halt
sub     add     1       1       3       # R3 = R1 + R1
        jalr    5       0               # 返回
        halt
val1    .fill   4
subAdd .fill   sub
