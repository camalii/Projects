/* Represents a 32-bit processor */
#ifndef CPU_H

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum cpuStatus
{
    cpuOK,
    cpuHalted,
    cpuIllegalInstruction,
    cpuIllegalOperand,
    cpuInvalidAddress,
    cpuInvalidStackOperation,
    cpuDivByZero,
    cpuIOError
};

struct cpu
{
    int32_t A;
    int32_t B;
    int32_t C;
    int32_t D;
    int32_t status;
    int32_t stackSize;
    int32_t instructionPointer;
    int32_t *memory;
    int32_t *stackBottom;
    int32_t *stackLimit;
#ifdef BONUS_JMP
    int32_t result;
#endif
};

int32_t *cpuCreateMemory(FILE *program, size_t stackCapacity, int32_t **stackBottom);
void cpuCreate(struct cpu *cpu, int32_t *memory, int32_t *stackBottom, size_t stackCapacity);
void cpuDestroy(struct cpu *cpu);
void cpuReset(struct cpu *cpu);
int cpuStatus(struct cpu *cpu);
int cpuStep(struct cpu *cpu);
int cpuRun(struct cpu *cpu, size_t steps);
int32_t cpuPeek(struct cpu *cpu, char reg);

// TODO structs and headers here

#endif
