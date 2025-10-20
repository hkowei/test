Main    lw   0   1   five
        lw   0   2   Limit
start   add  1   1   1
        beq  1   2   done
        lw   0   3   F
inner   nor  3   1   3
        sw   0   3   result
        beq  3   0   start
        lw   0   4   FUNCAd
        jalr 4   7
        noop
sub     add  2   1   2
        jalr 7   6
done    halt
five    .fill M
loVal .fill 3
result  .fill 0
FUNCAd .fill sub
Limit   .fill 16
