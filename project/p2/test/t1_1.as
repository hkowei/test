subOne  lw      0       2       neg1        ; $2 = -1
        add     1       2       1           ; $1 = $1 - 1
        jalr    7       6                  ; return
neg1    .fill   -1
SubAdr  .fill   subOne
