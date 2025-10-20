        lw      0       1       five        ; $1 = 5
        lw      0       4       SubAdr      ; $4 = address of SubAdr
start   jalr    4       7                  ; jump to subOne
        beq     0       1       done        ; loop until $1 == 0
        beq     0       0       start
done    halt
five    .fill   5