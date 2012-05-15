    org 0
    ld c, 0          ; 000E
    ld b, 10h        ; 1006
loop:   INC C
        DJNZ loop
    
