START:
    LDA #0
    LDB #0

    LDA #1
    LDA #0

    STA 0x2000
    STA 0x2001
    STA 0x2002
    STA 0x2003
    STA 0x2004

SUM_LOOP:
    ADD_A
    ADD_A

    LDA #0
    STA 0x2000
    LDA #1
    STA 0x2001
    LDA #2
    STA 0x2002
    LDA #3
    STA 0x2003
    LDA #4
    STA 0x2004

    ADD_B

    PUSH_A

    CALL INC_COUNTER

    POP_A

    LDA #5
    SUB_A

    JZ END_SUM
    JMP SUM_LOOP

INC_COUNTER:
    INC_A

    RET

END_SUM:
    BRK

    HLT
