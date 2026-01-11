#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Anni8OP.h"

#define MAX_LABELS 64
#define MAX_LINES 128

#define Byte uint8_t

typedef struct {
    char Name[32];

    Byte Address;
} Label;

Label Labels[MAX_LABELS];

int LabelCount = 0;

void Trim(char *String) {
    char *End;

    while (*String == ' ' || *String == '\r')
        String++;
        
    End = String + strlen(String) - 1;

    while (End > String && (*End == ' ' || *End == '\t' || *End == '\n' || *End == '\r'))
        *End-- = 0;
}

Byte GetOpcode(const char *Instruction) {
    if (strcmp(Instruction, "NOP") == 0) return ANI8_NOP;
    if (strcmp(Instruction, "HLT") == 0) return ANI8_HLT;
    if (strcmp(Instruction, "LDA") == 0) return ANI8_LD_A_IMM;
    if (strcmp(Instruction, "LDB") == 0) return ANI8_LD_B_IMM;
    if (strcmp(Instruction, "LDA_ADDR") == 0) return ANI8_LDA_ADDR;
    if (strcmp(Instruction, "STA") == 0) return ANI8_STA_ADDR;
    if (strcmp(Instruction, "ADD") == 0) return ANI8_ADD_A;
    if (strcmp(Instruction, "SUB") == 0) return ANI8_SUB_A;
    if (strcmp(Instruction, "INC") == 0) return ANI8_INC_A;
    if (strcmp(Instruction, "INC_A") == 0) return ANI8_INC_A;
    if (strcmp(Instruction, "INC_B") == 0) return ANI8_INC_B;
    if (strcmp(Instruction, "DEC") == 0) return ANI8_DEC_A;
    if (strcmp(Instruction, "DEC_A") == 0) return ANI8_DEC_A;
    if (strcmp(Instruction, "DEC_B") == 0) return ANI8_DEC_B;
    if (strcmp(Instruction, "JMP") == 0) return ANI8_JMP;
    if (strcmp(Instruction, "JZ") == 0) return ANI8_JZ;
    if (strcmp(Instruction, "JNZ") == 0) return ANI8_JNZ;
    if (strcmp(Instruction, "JC") == 0) return ANI8_JC;
    if (strcmp(Instruction, "PUSH") == 0) return ANI8_PUSH_A;
    if (strcmp(Instruction, "PUSH_A") == 0) return ANI8_PUSH_A;
    if (strcmp(Instruction, "PUSH_B") == 0) return ANI8_PUSH_B;
    if (strcmp(Instruction, "POP") == 0) return ANI8_POP_A;
    if (strcmp(Instruction, "POP_A") == 0) return ANI8_POP_A;
    if (strcmp(Instruction, "POP_B") == 0) return ANI8_POP_B;
    if (strcmp(Instruction, "CALL") == 0) return ANI8_CALL;
    if (strcmp(Instruction, "REU") == 0) return ANI8_RET;
    if (strcmp(Instruction, "BRK") == 0) return ANI8_BRK;

    return 0x00; // NOP
}

int FindLabel(const char* Name) {
    for (int i=0; i < LabelCount; i++)
        if (strcmp(Labels[i].Name, Name) == 0) return Labels[i].Address;

    return -1;
}

int Assemble(const char* _FILE, const char* OutFile, Byte* Memory) {
    FILE *File = fopen(_FILE, "r");
    if (!File) {
        fprintf(stderr, "Input file");

        return 0;
    }

    char Line[MAX_LINES];
    int Address = 0;

    while (fgets(Line, sizeof(Line), File)) {
        Trim(Line);

        if (Line[0] == ';' || Line[0] == 0)
            continue;

        char *Colon = strchr(Line, ':');

        if (Colon) {
            *Colon = 0;

            Trim(Line);
            strcpy(Labels[LabelCount].Name, Line);

            Labels[LabelCount].Address = Address;
            LabelCount++;

            continue;
        }

        char Instruction[32];

        sscanf(Line, "%s", Instruction);
        Byte OP = GetOpcode(Instruction);

        if (OP == ANI8_LD_A_IMM || OP == ANI8_LD_B_IMM)
            Address += 2;
        else if (OP == ANI8_LDA_ADDR || OP == ANI8_STA_ADDR || OP == ANI8_JMP || OP == ANI8_JZ || OP == ANI8_JNZ || OP == ANI8_JC || OP == ANI8_CALL)
            Address += 3;
        else
            Address += 1;
    }

    rewind(File);

    Address = 0;

    while(fgets(Line, sizeof(Line), File)) {
        Trim(Line);

        if (Line[0] == ';' || Line[0] == 0)
            continue;

        if (strchr(Line, ':'))
            continue;

        char Instruction[32], Operand[32];
        int Count = sscanf(Line, "%s %s", Instruction, Operand);

        Byte OP = GetOpcode(Instruction);

        Memory[Address++] = OP;

        if (Count == 2) {
            if (Operand[0] == '#') {
                int Value = atoi(Operand + 1);

                Memory[Address++] = Value & 0xFF;
            } else if (Operand[0]=='0' && Operand[1]=='x') {
                int Value = (int)strtol(Operand, NULL, 16);

                Memory[Address++] = Value & 0xFF;
                Memory[Address++] = (Value >> 8) & 0xFF;
            } else {
                int LabelAddress = FindLabel(Operand);

                if (LabelAddress < 0) {
                    fprintf(stderr,"Unknown label: %s\n", Operand);

                    return 0;
                }

                Memory[Address++] = LabelAddress & 0xFF;
                Memory[Address++] = (LabelAddress >> 8) & 0xFF;
            }
        }
    }

    fclose(File);

    FILE *Output = fopen(OutFile, "wb");
    if (!Output) {
        fprintf(stderr, "Output file");
        
        return 0;
    }

    fwrite(Memory, 1, Address, Output);
    fclose(Output);

    return Address;
}
