#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Anni8OP.h"

#define Byte uint8_t

int ANI8_StepMode = 0;
int ANI8_Trace = 1;

typedef enum {
    ANI8_FLAG_Z = 1 << 0,
    ANI8_FLAG_C = 1 << 1,
    ANI8_FLAG_N = 1 << 2
} Anni8_Flags;

typedef struct {
    Byte A; // GP Register
    Byte B; // GP Register

    Byte SP; // Stack Pointer
    Byte IR; // Instruction Register

    Byte Flags;

    Byte Memory[0x10000]; // 64KB

    uint16_t PC;

    int Halted;
} Anni8_CPU;

void Ani8_Reset(Anni8_CPU *CPU) {
    memset(CPU -> Memory, 0, sizeof(CPU -> Memory));

    CPU -> A = 0;
    CPU -> B = 0;

    CPU -> SP = 0xFF;
    CPU -> Flags = 0;

    CPU -> PC = 0x0000;
    CPU -> Halted = 0;
}

Byte Ani8_Fetch(Anni8_CPU *CPU) {
    CPU -> IR = CPU -> Memory[CPU -> PC];
    CPU -> PC += 1;

    return CPU -> IR;
}

void Ani8_Step(Anni8_CPU *CPU) {
    Byte Opcode = Ani8_Fetch(CPU);

    switch (Opcode) {
        case ANI8_NOP:
            break;
        
        case ANI8_HLT:
            CPU -> Halted = 1;

            break;
        
        case ANI8_LD_A_IMM:
            CPU -> A = CPU -> Memory[CPU -> PC++]; // Reads the next byte
            CPU -> Flags = 0;

            if (CPU -> A == 0) CPU -> Flags |= ANI8_FLAG_Z;
            if (CPU -> A & 0x80) CPU -> Flags |= ANI8_FLAG_N;

            break;
        
        case ANI8_LD_B_IMM:
            CPU -> B = CPU -> Memory[CPU -> PC++];
            CPU -> Flags = 0;

            if (CPU -> B == 0) CPU -> Flags |= ANI8_FLAG_Z;
            if (CPU -> B & 0x80) CPU -> Flags |= ANI8_FLAG_N;

            break;

        case ANI8_ADD_A: {
            uint16_t Result = CPU -> A + CPU -> B;
            
            CPU -> Flags &= ~(ANI8_FLAG_Z | ANI8_FLAG_N | ANI8_FLAG_C);

            if ((Result & 0xFF) == 0) CPU -> Flags |= ANI8_FLAG_Z;
            if (Result & 0x80) CPU -> Flags |= ANI8_FLAG_N;
            if (Result > 0xFF) CPU -> Flags |= ANI8_FLAG_C;

            CPU -> A = Result & 0xFF;

            break;
        }

        case ANI8_ADD_B: {
            uint16_t Result = CPU -> B + CPU -> A;
            
            CPU -> Flags &= ~(ANI8_FLAG_Z | ANI8_FLAG_N | ANI8_FLAG_C);

            if ((Result & 0xFF) == 0) CPU -> Flags |= ANI8_FLAG_Z;
            if (Result & 0x80) CPU -> Flags |= ANI8_FLAG_N;
            if (Result > 0xFF) CPU -> Flags |= ANI8_FLAG_C;

            CPU -> B = Result & 0xFF;

            break;
        }

        case ANI8_SUB_A: {
            uint16_t Result = CPU -> A - CPU -> B;
            
            CPU -> Flags &= ~(ANI8_FLAG_Z | ANI8_FLAG_N | ANI8_FLAG_C);

            if (CPU -> A < CPU -> B) CPU -> Flags |= ANI8_FLAG_C;
            if ((Result & 0xFF) == 0) CPU -> Flags |= ANI8_FLAG_Z;
            if (Result & 0x80) CPU -> Flags |= ANI8_FLAG_N;

            CPU -> A = Result & 0xFF;

            break;
        }

        case ANI8_SUB_B: {
            uint16_t Result = CPU -> B - CPU -> A;
            
            CPU -> Flags &= ~(ANI8_FLAG_Z | ANI8_FLAG_N | ANI8_FLAG_C);

            if (CPU -> B < CPU -> A) CPU -> Flags |= ANI8_FLAG_C;
            if ((Result & 0xFF) == 0) CPU -> Flags |= ANI8_FLAG_Z;
            if (Result & 0x80) CPU -> Flags |= ANI8_FLAG_N;

            CPU -> B = Result & 0xFF;

            break;
        }

        case ANI8_INC_A: {
            CPU -> A++;
            
            CPU -> Flags &= ~(ANI8_FLAG_Z | ANI8_FLAG_N);

            if (CPU -> A == 0) CPU -> Flags |= ANI8_FLAG_Z;
            if (CPU -> A & 0x80) CPU -> Flags |= ANI8_FLAG_N;

            break;
        }

        case ANI8_INC_B: {
            CPU -> B++;
            
            CPU -> Flags &= ~(ANI8_FLAG_Z | ANI8_FLAG_N);

            if (CPU -> B == 0) CPU -> Flags |= ANI8_FLAG_Z;
            if (CPU -> B & 0x80) CPU -> Flags |= ANI8_FLAG_N;

            break;
        }

        case ANI8_DEC_A: {
            CPU -> A--;
            
            CPU -> Flags &= ~(ANI8_FLAG_Z | ANI8_FLAG_N);

            if (CPU -> A == 0) CPU -> Flags |= ANI8_FLAG_Z;
            if (CPU -> A & 0x80) CPU -> Flags |= ANI8_FLAG_N;

            break;
        }

        case ANI8_DEC_B: {
            CPU -> B--;
            
            CPU -> Flags &= ~(ANI8_FLAG_Z | ANI8_FLAG_N);

            if (CPU -> B == 0) CPU -> Flags |= ANI8_FLAG_Z;
            if (CPU -> B & 0x80) CPU -> Flags |= ANI8_FLAG_N;

            break;
        }

        case ANI8_JMP: {
            Byte Low  = CPU -> Memory[CPU -> PC++];
            Byte High = CPU -> Memory[CPU -> PC++];

            uint16_t Address = (High << 8) | Low;

            CPU -> PC = Address;
            
            break;
        }

        case ANI8_JZ: {
            Byte Low  = CPU -> Memory[CPU -> PC++];
            Byte High = CPU -> Memory[CPU -> PC++];

            uint16_t Address = (High << 8) | Low;

            if (CPU -> Flags & ANI8_FLAG_Z)
                CPU -> PC = Address;
            
            break;
        }

        case ANI8_JNZ: {
            Byte Low  = CPU -> Memory[CPU -> PC++];
            Byte High = CPU -> Memory[CPU -> PC++];

            uint16_t Address = (High << 8) | Low;

            if (!(CPU -> Flags & ANI8_FLAG_Z))
                CPU -> PC = Address;
            
            break;
        }

        case ANI8_JC: {
            Byte Low  = CPU -> Memory[CPU -> PC++];
            Byte High = CPU -> Memory[CPU -> PC++];

            uint16_t Address = (High << 8) | Low;

            if (CPU -> Flags & ANI8_FLAG_C)
                CPU -> PC = Address;
            
            break;
        }

        case ANI8_PUSH_A: {
            CPU -> Memory[0x0100 + CPU -> SP] = CPU -> A;
            CPU -> SP--;

            break;
        }

        case ANI8_PUSH_B: {
            CPU -> Memory[0x0100 + CPU -> SP] = CPU -> B;
            CPU -> SP--;

            break;
        }

        case ANI8_POP_A: {
            CPU -> SP++;
            CPU -> A = CPU -> Memory[0x0100 + CPU -> SP];

            break;
        }

        case ANI8_POP_B: {
            CPU -> SP++;
            CPU -> B = CPU -> Memory[0x0100 + CPU -> SP];

            break;
        }

        case ANI8_CALL: {
            Byte Low  = CPU -> Memory[CPU -> PC++];
            Byte High = CPU -> Memory[CPU -> PC++];

            uint16_t Address = (High << 8) | Low;

            CPU -> Memory[0x0100 + CPU -> SP] = (CPU -> PC & 0xFF);
            CPU -> SP--;

            CPU -> Memory[0x0100 + CPU -> SP] = (CPU -> PC >> 8);
            CPU -> SP--;

            CPU -> PC = Address;

            break;
        }

        case ANI8_RET: {
            Byte Low;
            Byte High;

            CPU -> SP++;
            High = CPU -> Memory[0x0100 + CPU -> SP];

            CPU -> SP++;
            Low = CPU -> Memory[0x0100 + CPU -> SP];

            CPU -> PC = (High << 8) | Low;

            break;
        }

        case ANI8_LDA_ADDR: {
            Byte Low = CPU -> Memory[CPU -> PC++];
            Byte High = CPU -> Memory[CPU -> PC++];

            uint16_t Address = (High << 8) | Low;

            CPU -> A = CPU -> Memory[Address];

            CPU -> Flags &= ~(ANI8_FLAG_Z | ANI8_FLAG_N);

            if (CPU -> A == 0) CPU -> Flags |= ANI8_FLAG_Z;
            if (CPU -> A & 0x80) CPU -> Flags |= ANI8_FLAG_N;

            break;
        }

        case ANI8_STA_ADDR: {
            Byte Low = CPU -> Memory[CPU -> PC++];
            Byte High = CPU -> Memory[CPU -> PC++];

            uint16_t Address = (High << 8) | Low;

            CPU -> Memory[Address] = CPU -> A;

            break;
        }

        case ANI8_BRK: {
            CPU -> Halted = 1;

            if (ANI8_Trace)
                printf("[BRK] PC = %04X A = %03X B = %03X F = %02X\n", CPU -> PC, CPU -> A, CPU -> B, CPU -> Flags);
        }

        case ANI8_DUMP: {
            printf("=== CPU DUMP ===\n");
            printf("PC = %04X SP = %02X A = %03X B = %03X F = %02X\n", CPU -> PC, CPU -> SP, CPU -> A, CPU -> B, CPU -> Flags);
            printf("Memory[0x2000-0x200F]: ");

            for (int i=0; i < 16; i++)
                printf("%02X ", CPU -> Memory[0x2000 + i]);
            
            printf("\n================\n");

            break;
        }

        default:
            fprintf(stderr, "Unknown opcode: %02X\n", Opcode);
        
            break;
    }

    if (ANI8_Trace)
        printf("PC = %04X OP = %02X A = %03X B = %03X F = %02X | Z:%d N:%d C:%d\n", CPU -> PC, CPU -> IR, CPU -> A, CPU -> B, CPU -> Flags, (CPU -> Flags & ANI8_FLAG_Z) ? 1 : 0, (CPU -> Flags & ANI8_FLAG_N) ? 1 : 0, (CPU -> Flags & ANI8_FLAG_C) ? 1 : 0);
}

/*int main() {
    Anni8_CPU CPU = {0};

    Ani8_Reset(&CPU);

    while (!CPU.Halted && CPU.PC <= 0x0FF) {
        Ani8_Step(&CPU);

        if (ANI8_StepMode) {
            printf("Press ENTER to step...");
            
            getchar();
        }
    }

    return 0;
}*/
