# Anni8 - v0.8

> This is a ripped part of the Yari fantasy console; updates between the Yari's Anni8 CPU & this Anni8 CPU will be noticable.

**Anni8** (ANI8 for short/internals) is a 8-Bit fantasy CPU, written in the C programming language. Originally designed as the CPU core of the Yari fantasy console, this is a standalone repository for the CPU and assembler for anyone who wants to explore retro CPU design, experiment with assembly programming, or use it as a base for other fantasy console projects. Anni8 also has its own built in assembler, ANI8 Assembly, which is the Assembly version the Anni8 CPU uses.

## ANI8 Assembly

ANI8 Assembly derives off the Assembly programming language, and serves as a simple, human-readable way to program the CPU. The following data below is the instruction sets, alongside an example:

**Memory Instructions**

| Instruction | Description |
| ----------- | ----------------------------- |
| `LDA addr` | Load value from memory into A |
| `STA addr` | Store value of A into memory  |
| `LDB addr` | Load value from memory into B |
| `STB addr` | Store value of B into memory  |

**Arithmetic Instructions**

| Instruction | Mnemonic |
| ----------- | --------- |
| `ADD_A` | A = A + B |
| `ADD_B` | B = B + A |
| `SUB_A` | A = A - B |
| `SUB_B` | B = B - A |
| `INC_A` | A = A + 1 |
| `INC_B` | B = B + 1 |
| `DEC_A` | A = A - 1 |
| `DEC_B` | B = B - 1 |

**Stack Instructions**

| Instruction | Description |
| ----------- | --------------------------- |
| `PUSH_A` | Push A onto stack |
| `PUSH_B` | Push B onto stack |
| `POP_A` | Pop stack into A |
| `POP_B` | Pop stack into B |
| `CALL addr` | Push PC and jump to address |
| `RET` | Return from subroutine |

**Debugging Instructions**

| Instruction | Description                          |
| ----------- | ------------------------------------ |
| `BRK` | Pause CPU and optionally print trace |
| `DUMP` | Dump CPU state and a memory segment  |

Example ANI8 Assembly program, which sums the numbers 1-5 and stored in memory.

```asm
START:
    LDA #0
    LDB #0

    ; Loop from 1 to 5
    LDA #1

LOOP:
    ADD_B
    INC_A

    LDA #5
    SUB_A

    JNZ LOOP

    DUMP
    BRK

    HLT
```
