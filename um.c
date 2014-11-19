#include <stdint.h>
#include <stdio.h>
#include "memoryManager.h"
#include "ioModule.h"



void initialize(FILE *fp, int* programCounter, uint32_t* registers);
int execute(int* programCounter, uint32_t* registers);

typedef struct three_regs {
        uint32_t a;
        uint32_t b;
        uint32_t c;
} three_regs;


/* Conditional Move */
static inline int cmov(three_regs regs)
{

        if (*(regs.c) != 0) {
                *(regs.a) = *(regs.b);
        }

        return 0;
}

/* Segmented Load */
static inline int sload(three_regs regs)
{
        *(regs.a) = getWord(*(regs.b), *(regs.c));
        return 0;
}

/* Segmented Store */
static inline int sstore(three_regs regs)
{
        return putWord(*(regs.c), *(regs.a), *(regs.b));
}

/* Addition */
static inline int add(three_regs regs)
{
        *(regs.a) = (*(regs.b) + *(regs.c));
        return 0;
}

/* Multiplication */
static inline int mult(three_regs regs)
{
        *(regs.a) = (*(regs.b) * *(regs.c));
        return 0;
}

/* divsion */
static inline int divide(three_regs regs)
{
        *(regs.a) = *(regs.b) / *(regs.c);
        return 0;
}

/* Bitwise NAND */
static inline int nand(three_regs regs)
{
        *(regs.a) = ~(*(regs.b) & *(regs.c));
        return 0;
}

/* Halt */
static inline int halt(three_regs regs)
{
        (void) regs;
        return 1;
}

/* Map Segment */
static inline int map(three_regs regs)
{
        uint32_t tempid = newSegment(*(regs.c));

        if (tempid != 0) {
                *(regs.b) = tempid;
                return 0;
        }
        return 1;
}

/* Unmap Segment */
static inline int unmap(three_regs regs)
{
        return(removeSegment(*(regs.c)));
}

/* UMOutput */
static inline int ALUoutput(three_regs regs)
{
        IOoutput(*(regs.c));
        return 0;

}

/* Input */
static inline int ALUinput(three_regs regs)
{
        uint32_t input = IOinput();
        if (input == ~(uint32_t)0) {
                *(regs.c) = ~0;
                return 0;
        } else if (input <= 255) {
                *(regs.c) = input;
                return 0;
        }
        return 1;
}

/* Load Program */
static inline int loadp(three_regs regs, int *pc)
{
        if (*(regs.b) == 0) {
                *pc = *(regs.c);
                return 0;
        } else{
                replaceSegment(*(regs.b), 0);
                *pc = *(regs.c);
                return 0;
        }
        return 1;
}

/* Load Value */
static inline int loadv(uint32_t* regA, uint32_t value)
{
        *regA = value;
        return 0;
}


int main(int argc, char* argv[]) {

        FILE *fp;

        if (argc == 2) {
                fp = fopen(argv[1], "rb");
        } else {

                fprintf(stderr, "Invalid File\n");
                exit(EXIT_FAILURE);
        }

        int* pc = malloc(sizeof(int));
        
        initialize(fp, pc);
        execute(pc);

        fclose(fp);

        free(pc);

        deleteMemory();

}




/*
 *      This will initialize and allocate memory for an array  
 *      of registers and call newMemory in the
 *      memoryManager
 *      Only segment zero of the segmented memory will be
 *      mapped and all registers are zero. Additionally the 
 *      program counter will point to $m[0][0]
 */
void initialize(FILE *fp, int *pc) 
{

        *pc = 0;


        fseek(fp, 0, SEEK_END);
        int programLength = ftell(fp);
        rewind(fp);

        newMemory(programLength);
        loadProgram(fp);
}

/*
 *      Execute runs a loop that increments the program counter
 *      and performs each operation as requested by the 
 *      instruction word pointed too by the program counter.
 *      This function will return an integer to indicate if the
 *      program succeeded or failed.
 */
int execute(int* pc)
{

        int program_status = 0;

        uint32_t* registers = malloc(sizeof(uint32_t) * 8);

        for (int i = 0; i < 8; i++) {
                registers[i] = (uint32_t) 0;
        }


        while(program_status != 1) {

                *pc += 1;


                uint32_t instruction = getWord(0, *pc - 1);
                
                uint32_t opcode =  instruction;
                opcode = opcode >> 28;
                
                if(opcode == 13) {
                        uint32_t lreg = instruction;
                        uint32_t lval = instruction;
                        program_status = loadv(&(registers[((lreg << 4) >> 29)]),  (lval << 7) >> 7); 
                } else {
                        uint32_t a = instruction;
                        uint32_t b = instruction;
                        uint32_t c = instruction;
                        
                        three_regs registersUsed;

                        registersUsed.a = &(registers[(a << 23) >> 29]);
                        registersUsed.b = &(registers[(b << 26) >> 29]);
                        registersUsed.c = &(registers[(c << 29) >> 29]);

                        registersUsed.a = (a << 23) >> 29;
                        registersUsed.b = (b << 26) >> 29;
                        registersUsed.c = (c << 29) >> 29;
                        
                        if(opcode == 1) {
                                program_status = (sload(registersUsed));
                        } else if (opcode == 2){
                                program_status = (sstore(registersUsed));
                        } else if (opcode == 6) {
                                program_status = (nand(registersUsed));
                        } else if (opcode == 12) {
                                program_status = (loadp(registersUsed, pc));
                        } else if(opcode == 3) {
                                program_status = (add(registersUsed));
                        } else if(opcode == 0) {
                                program_status = (cmov(registersUsed));
                        } else if (opcode == 8) {
                                program_status = (map(registersUsed));
                        } else if (opcode == 9) {
                                program_status = (unmap(registersUsed));
                        } else if (opcode == 5) {
                                program_status = (divide(registersUsed));
                        } else if (opcode == 4) {
                                program_status = (mult(registersUsed));
                        } else if (opcode == 10) {
                                program_status = (ALUoutput(registersUsed));
                        } else if (opcode == 7) {
                                program_status = (halt(registersUsed));
                        } else if (opcode == 11) {
                                program_status = (ALUinput(registersUsed));
                        } else {
                                program_status =  1;
                        }
                }

        }

        free(registers);
        return 0;

}

