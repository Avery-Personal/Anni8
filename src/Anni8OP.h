#ifndef ANI_OPCODES_H
#define ANI_OPCODES_H

typedef enum {
    ANI8_NOP = 0x00,
    ANI8_HLT = 0x01,

    ANI8_LD_A_IMM = 0x10,
    ANI8_LD_B_IMM = 0x11,

    ANI8_LDA_ADDR = 0x12,
    ANI8_STA_ADDR = 0x13,

    ANI8_ADD_A = 0x20,
    ANI8_ADD_B = 0x21,

    ANI8_SUB_A = 0x22,
    ANI8_SUB_B = 0x23,

    ANI8_INC_A = 0x24,
    ANI8_INC_B = 0x25,

    ANI8_DEC_A = 0x26,
    ANI8_DEC_B = 0x27,

    ANI8_JMP = 0x40,
    ANI8_JZ = 0x41,
    ANI8_JNZ = 0x42,
    ANI8_JC = 0x43,

    ANI8_PUSH_A = 0x50,
    ANI8_PUSH_B = 0x52,

    ANI8_POP_A = 0x51,
    ANI8_POP_B = 0x53,
    
    ANI8_CALL = 0x54,
    ANI8_RET = 0x55,

    ANI8_BRK = 0xF0,
    ANI8_DUMP = 0xF1,
} Anni8_Opcodes;

#endif
