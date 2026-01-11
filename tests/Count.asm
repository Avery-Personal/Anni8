START:
    LDA #0
    LDB #1

    LDA #0
    
    STA 0x2000
    STA 0x2001
    STA 0x2002
    STA 0x2003
    STA 0x2004
    STA 0x2005
    STA 0x2006
    STA 0x2007
    STA 0x2008
    STA 0x2009

LOOP:
    INC_A

    LDA #0
    ADD_A

    LDB #0
    STA 0x2000
    
    ADD_B

    PUSH_A

    CALL INC_B_SUB

    POP_A

    LDA #10
    SUB_A

    JZ END_LOOP
    JMP LOOP

INC_B_SUB:
    INC_B

    RET

END_LOOP:
    DUMP
    BRK
    
    HLT
