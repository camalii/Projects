#include "cpu.h"
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ADDRESS_TEST(size)                                                         \
    {                                                                              \
        if (cpu->memory + cpu->instructionPointer + (size) -1 > cpu->stackLimit) { \
            cpu->instructionPointer--;                                             \
            cpu->status = cpuInvalidAddress;                                       \
            return 0;                                                              \
        }                                                                          \
    }

#define CPU_ERROR(errorStatus, addressIndex)                                \
    {                                                                       \
        cpu->status = errorStatus;                                          \
        cpu->instructionPointer = cpu->instructionPointer - (addressIndex); \
        return 0;                                                           \
    }

enum instructions
{
    Nop, Halt, Add, Sub, Mul, MyDiv, Inc, Dec, Loop, Movr, Load, Store, In, Get, Out, Put, Swap, Push, Pop
#ifdef BONUS_JMP
    , Cmp = 19, Jmp = 20, Jz = 21, Jnz = 22, Jgt = 23
#endif
#ifdef BONUS_CALL
    , Call = 24, Ret = 25
#endif
};

static int32_t *getRegister(struct cpu *cpu, uint32_t instructionLenght)
{
    int32_t regNumber = *(cpu->memory + cpu->instructionPointer++);
    switch (regNumber) {
    case 0:
        return &(cpu->A);
        break;
    case 1:
        return &(cpu->B);
        break;
    case 2:
        return &(cpu->C);
        break;
    case 3:
        return &(cpu->D);
        break;
    default:
        cpu->instructionPointer = cpu->instructionPointer - (instructionLenght);
        cpu->status = cpuIllegalOperand;
        return NULL;
        break;
    }
}

#ifdef BONUS_JMP
static int32_t *getRegister5(struct cpu *cpu, uint32_t instructionLenght)
{
    int32_t regNumber = *(cpu->memory + cpu->instructionPointer++);
    switch (regNumber) {
    case 0:
        return &(cpu->A);
        break;
    case 1:
        return &(cpu->B);
        break;
    case 2:
        return &(cpu->C);
        break;
    case 3:
        return &(cpu->D);
        break;
    case 4:
        return &(cpu->result);
        break;
    default:
        cpu->instructionPointer = cpu->instructionPointer - (instructionLenght);
        cpu->status = cpuIllegalOperand;
        return NULL;
        break;
    }
}
#endif

int32_t *cpuCreateMemory(FILE *program, size_t stackCapacity, int32_t **stackBottom)
{
    assert(program != NULL);
    assert(stackBottom != NULL);
    unsigned char *ptr = NULL;
    unsigned char *ptrNew = NULL;
    int32_t memorySize = 0;
    int32_t freeMemory = 0;
    int32_t position = 0;
    int ch = 0;

    do {
        for (char stp = 0; stp < 4; stp++) {
            ch = fgetc(program);

            if (ch == EOF) {
                if (stp == 0)
                    break;
                if (ptr != NULL)
                    free(ptr);
                return NULL;
            }

            if (freeMemory == 0) {
                memorySize += 4096;
                ptrNew = (unsigned char *) realloc(ptr, memorySize);
                if (ptrNew == NULL) {
                    free(ptr);
                    return NULL;
                } // not enough memory = ending function
                ptr = ptrNew;
                freeMemory = 4096;
            }
            *(ptr + position) = (char) ch;
            position++;
            freeMemory--;
        }
    } while (ch != EOF);

    if (position == 0 && stackCapacity == 0)
        return NULL; // if program and stack are empty

    while (freeMemory < (int32_t)(stackCapacity * (sizeof(int32_t)))) {
        memorySize += 4096;
        ptrNew = (unsigned char *) realloc(ptr, memorySize);
        if (ptrNew == NULL) {
            free(ptr);
            return NULL;
        }
        ptr = ptrNew;
        freeMemory += 4096;
    }

    if (ptr != NULL)
        memset(ptr + position, 0, memorySize - position);

    *stackBottom = (int32_t *) (ptr + memorySize - sizeof(int32_t));

    return (int32_t *) ptr;
}

void cpuCreate(struct cpu *cpu, int32_t *memory, int32_t *stackBottom, size_t stackCapacity)
{
    assert(cpu != NULL);
    assert(memory != NULL);
    assert(stackBottom != NULL);
    cpu->A = 0;
    cpu->B = 0;
    cpu->C = 0;
    cpu->D = 0;
    cpu->status = cpuOK;
    cpu->stackBottom = stackBottom;
    cpu->stackSize = 0;
    cpu->stackLimit = stackBottom - stackCapacity;
    cpu->memory = memory;
    cpu->instructionPointer = 0;
#ifdef BONUS_JMP
    cpu->result = 0;
#endif
}

void cpuDestroy(struct cpu *cpu)
{
    assert(cpu != NULL);
    free(cpu->memory);
}

void cpuReset(struct cpu *cpu)
{
    assert(cpu != NULL);
    cpu->A = 0;
    cpu->B = 0;
    cpu->C = 0;
    cpu->D = 0;
#ifdef BONUS_JMP
    cpu->result = 0;
#endif
    cpu->status = cpuOK;
    cpu->stackSize = 0;
    cpu->instructionPointer = 0;

    for (int32_t i = 1; i <= (cpu->stackBottom - cpu->stackLimit); i++)
        *(cpu->stackLimit + i) = 0;
}

int cpuStatus(struct cpu *cpu)
{
    assert(cpu != NULL);
    return cpu->status;
}

static int add(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t *registr = getRegister(cpu, 2);

    if (registr == NULL)
        return 0;

    cpu->A = cpu->A + (*registr);
#ifdef BONUS_JMP
    cpu->result = cpu->A;
#endif
    return 1;
}

static int sub(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t *registr = getRegister(cpu, 2);

    if (registr == NULL)
        return 0;

    cpu->A = cpu->A - (*registr);
#ifdef BONUS_JMP
    cpu->result = cpu->A;
#endif
    return 1;
}

static int mul(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t *registr = getRegister(cpu, 2);

    if (registr == NULL)
        return 0;

    cpu->A = cpu->A * (*registr);
#ifdef BONUS_JMP
    cpu->result = cpu->A;
#endif
    return 1;
}

static int myDiv(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t *registr = getRegister(cpu, 2);

    if (registr == NULL)
        return 0;
    if (*registr == 0)
        CPU_ERROR(cpuDivByZero, 2);

    cpu->A = cpu->A / (*registr);
#ifdef BONUS_JMP
    cpu->result = cpu->A;
#endif
    return 1;
}

static int inc(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t *registr = getRegister(cpu, 2);

    if (registr == NULL)
        return 0;

    *registr = *registr + 1;
#ifdef BONUS_JMP
    cpu->result = *registr;
#endif
    return 1;
}

static int dec(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t *registr = getRegister(cpu, 2);

    if (registr == NULL)
        return 0;

    *registr = *registr - 1;
#ifdef BONUS_JMP
    cpu->result = *registr;
#endif
    return 1;
}

static int loop(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t value = *(cpu->memory + cpu->instructionPointer++);

    if (cpu->C != 0)
        cpu->instructionPointer = value;

    return 1;
}

static int movr(struct cpu *cpu)
{
    ADDRESS_TEST(2);
    int32_t *registr = getRegister(cpu, 2);

    if (registr == NULL)
        return 0;

    int32_t value = *(cpu->memory + cpu->instructionPointer++);
    *registr = value;
    return 1;
}

static int load(struct cpu *cpu)
{
    ADDRESS_TEST(2);
    int32_t *registr = getRegister(cpu, 2);

    if (registr == NULL)
        return 0;

    int32_t value = *(cpu->memory + cpu->instructionPointer++);
    int32_t *addr = 1 + cpu->D + value + (cpu->stackBottom - cpu->stackSize);

    if (addr > cpu->stackBottom || addr <= (cpu->stackBottom - cpu->stackSize))
        CPU_ERROR(cpuInvalidStackOperation, 3);

    *registr = *addr;
    return 1;
}

static int store(struct cpu *cpu)
{
    ADDRESS_TEST(2);
    int32_t *registr = NULL;
#ifdef BONUS_JMP
    registr = getRegister5(cpu, 2);
#else
    registr = getRegister(cpu, 2);
#endif

    if (registr == NULL)
        return 0;

    int32_t value = *(cpu->memory + cpu->instructionPointer++);
    int32_t *addr = 1 + cpu->D + value + (cpu->stackBottom - cpu->stackSize);

    if (addr > (cpu->stackBottom) || addr <= (cpu->stackBottom - cpu->stackSize))
        CPU_ERROR(cpuInvalidStackOperation, 3);

    *addr = *registr;
    return 1;
}

static int in(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t *registr = getRegister(cpu, 2);

    if (registr == NULL)
        return 0;

    int32_t number;
    int rcode = scanf("%i", &number);

    if (rcode == 0)
        CPU_ERROR(cpuIOError, 2);
    if (rcode == EOF) {
        cpu->C = 0;
        number = -1;
    }

    *registr = number;
    return 1;
}

static int get(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t *registr = getRegister(cpu, 2);

    if (registr == NULL)
        return 0;

    int number = getchar();
    if (number == EOF) {
        cpu->C = 0;
        number = -1;
    }

    *registr = number;
    return 1;
}

static int out(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t *registr = NULL;
#ifdef BONUS_JMP
    registr = getRegister5(cpu, 2);
#else
    registr = getRegister(cpu, 2);
#endif

    if (registr == NULL)
        return 0;

    printf("%i", *registr);
    return 1;
}

static int put(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t *registr = NULL;
#ifdef BONUS_JMP
    registr = getRegister5(cpu, 2);
#else
    registr = getRegister(cpu, 2);
#endif

    if (registr == NULL)
        return 0;
    if ((uint32_t) *registr < 256) {
        printf("%c", *registr);
        return 1;
    }

    CPU_ERROR(cpuIllegalOperand, 2);
    return 1;
}

static int swap(struct cpu *cpu)
{
    ADDRESS_TEST(2);
    int32_t *registr = getRegister(cpu, 2);

    if (registr == NULL)
        return 0;

    int32_t *reg1 = registr;
    registr = getRegister(cpu, 3);

    if (registr == NULL)
        return 0;

    int32_t value = *reg1;
    *reg1 = *registr;
    *registr = value;
    return 1;
}

static int push(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t *registr = NULL;
#ifdef BONUS_JMP
    registr = getRegister5(cpu, 2);
#else
    registr = getRegister(cpu, 2);
#endif

    if (registr == NULL)
        return 0;
    if ((cpu->stackBottom - cpu->stackSize) > cpu->stackLimit) {
        *(cpu->stackBottom - cpu->stackSize) = *registr;
        cpu->stackSize++;
    } else
        CPU_ERROR(cpuInvalidStackOperation, 2);
    return 1;
}

static int pop(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t *registr = getRegister(cpu, 2);

    if (registr == NULL)
        return 0;
    if (cpu->stackSize > 0) {
        cpu->stackSize--;
        *registr = *(cpu->stackBottom - cpu->stackSize);
        *(cpu->stackBottom - cpu->stackSize) = 0;
        return 1;
    }

    CPU_ERROR(cpuInvalidStackOperation, 2);
    return 1;
}

#ifdef BONUS_JMP
static int cmp(struct cpu *cpu)
{
    ADDRESS_TEST(2);
    int32_t *registr = getRegister5(cpu, 2);
    if (registr == NULL)
        return 0;
    int32_t x = *registr;
    registr = getRegister5(cpu, 3);
    if (registr == NULL)
        return 0;
    cpu->result = x - (*registr);
    return 1;
}

static int jmp(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t value = *(cpu->memory + cpu->instructionPointer++);
    cpu->instructionPointer = value;
    return 1;
}

static int jz(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t value = *(cpu->memory + cpu->instructionPointer++);
    if (cpu->result == 0)
        cpu->instructionPointer = value;
    return 1;
}

static int jnz(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t value = *(cpu->memory + cpu->instructionPointer++);
    if (cpu->result != 0)
        cpu->instructionPointer = value;
    return 1;
}

static int jgt(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t value = *(cpu->memory + cpu->instructionPointer++);
    if (cpu->result > 0)
        cpu->instructionPointer = value;
    return 1;
}
#endif

#ifdef BONUS_CALL
static int call(struct cpu *cpu)
{
    ADDRESS_TEST(1);
    int32_t value = *(cpu->memory + cpu->instructionPointer++);
    if ((cpu->stackBottom - cpu->stackSize) > cpu->stackLimit) {
        *(cpu->stackBottom - cpu->stackSize) = cpu->instructionPointer;
        cpu->stackSize++;
        cpu->instructionPointer = value;
    } else
        CPU_ERROR(cpuInvalidStackOperation, 2);
    return 1;
}

static int ret(struct cpu *cpu)
{
    if (cpu->stackSize > 0) {
        cpu->stackSize--;
        cpu->instructionPointer = *(cpu->stackBottom - cpu->stackSize);
        *(cpu->stackBottom - cpu->stackSize) = 0;
        return 1;
    }
    CPU_ERROR(cpuInvalidStackOperation, 1);
    return 1;
}
#endif

int cpuStep(struct cpu *cpu)
{
    assert(cpu != NULL);

    if (cpu->status != cpuOK)
        return 0;
    if (cpu->instructionPointer < 0 || cpu->instructionPointer > (cpu->stackLimit - cpu->memory))
        CPU_ERROR(cpuInvalidAddress, 0);

    int32_t instruction = *(cpu->memory + cpu->instructionPointer++);

    switch (instruction) {
    case Nop:
        break;
    case Halt:
        cpu->status = cpuHalted;
        return 0;
        break;
    case Add: // add REG
        return add(cpu);
        break;
    case Sub: //sub REG
        return sub(cpu);
        break;
    case Mul: // mul REG
        return mul(cpu);
        break;
    case MyDiv: // div REG
        return myDiv(cpu);
        break;
    case Inc: // inc REG
        return inc(cpu);
        break;
    case Dec: // dec REG
        return dec(cpu);
        break;
    case Loop: // loop index
        return loop(cpu);
        break;
    case Movr: // movr REG NUM
        return movr(cpu);
        break;
    case Load: // load REG NUM
        return load(cpu);
        break;
    case Store: // store REG NUM
        return store(cpu);
        break;
    case In: // in REG
        return in(cpu);
        break;
    case Get: // get REG
        return get(cpu);
        break;
    case Out: // out REG
        return out(cpu);
        break;
    case Put: // put REG
        return put(cpu);
        break;
    case Swap: // swap REG REG
        return swap(cpu);
        break;
    case Push: // push REG
        return push(cpu);
        break;
    case Pop: // pop REG
        return pop(cpu);
        break;

#ifdef BONUS_JMP
    case Cmp: // CMP reg_x reg_y result = x - y
        return cmp(cpu);
        break;
    case Jmp: // jmp index
        return jmp(cpu);
        break;
    case Jz: //jz INDEX, jump to INDEX, if result == 0
        return jz(cpu);
        break;
    case Jnz: //jnz INDEX, jump to INDEX, if result != 0
        return jnz(cpu);
        break;
    case Jgt: //jgt INDEX, jump to INDEX, if result > 0
        return jgt(cpu);
        break;
#endif
#ifdef BONUS_CALL
    case Call: // call index
        return call(cpu);
        break;
    case Ret: // ret
        return ret(cpu);
        break;
#endif
    default:
        CPU_ERROR(cpuIllegalInstruction, 1);
        break;
    }
    return 1;
}

int cpuRun(struct cpu *cpu, size_t steps)
{
    assert(cpu != NULL);
    if (cpu->status != cpuOK)
        return 0;
    size_t count = 0;

    for (size_t i = 0; i < steps; i++) {
        count++;
        int32_t a = cpuStep(cpu);
        if (a == 0) {
            if (cpu->status == cpuHalted)
                return count;
            return -count;
        }
    }
    return count;
}

int32_t cpuPeek(struct cpu *cpu, char reg)
{
    assert(cpu != NULL);
    switch (reg) {
    case 'A':
        return cpu->A;
        break;
    case 'B':
        return cpu->B;
        break;
    case 'C':
        return cpu->C;
        break;
    case 'D':
        return cpu->D;
        break;
#ifdef BONUS_JMP
    case 'R':
        return cpu->result;
        break;
#endif
    case 'S':
        return cpu->stackSize;
        break;
    case 'I':
        return cpu->instructionPointer;
        break;
    default:
        return 0;
    }
    return 0;
}
