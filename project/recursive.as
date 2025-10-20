        lw          0       1       n
        lw	    0       2	    r
        lw          0       5       zero
        lw          0       6       Faddr        
        jalr        6       7                  
        halt
comb    lw          0       6       pos1
        sw          5       7       Stack
        add         5       6       5
        sw          5       1       Stack
        add         5       6       5
        sw          5       2       Stack
        add         5       6       5
        sw          5       4       Stack
        add         5       6       5
        lw          0       6       zero
        beq         2       6       ret1
        beq         1       2       ret1
else    lw          0       6       neg1
        add         1       6       1
        lw          0       6       Faddr
        jalr        6       7
        add         0       3       4
        lw          0       6       neg1
        add         2       6       2
        lw          0       6       Faddr
        jalr        6       7
        add         4       3       3
        beq         0       0       endif
ret1    lw          0       3       pos1
endif   lw          0       6       neg1
        add         5       6       5
        lw          5       4       Stack
        add         5       6       5
        lw          5       2       Stack
        add         5       6       5
        lw          5       1       Stack
        add         5       6       5
        lw          5       7       Stack
        jalr        7       6
n       .fill       7
r       .fill       3
Faddr   .fill       comb        
pos1    .fill       1       
neg1    .fill       -1
zero    .fill       0
 