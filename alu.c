#include "alu.h"


/*
Speed Notes:

Make these static inlines into macros... at the very least do it for the 
math ones... or the one lines ones

get rid of need for instruction struct, only use the pointers to the 
registers. 

*/

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


int performOperation(uint32_t instruction, uint32_t *registers, int *pc) {
        //instruction_struct instructionVals = interpretInstruction(instruction);

        uint32_t opcode = (uint32_t) Bitpack_getu(instruction, 4, 28);
        
        if(opcode == 13) {
                // uint32_t regA;
                // uint32_t value;

                // regA = ;
                // value = (uint32_t) Bitpack_getu(instruction, 25, 0);

                return(loadv(&(registers[(uint32_t)Bitpack_getu(instruction, 3, 25)]), (uint32_t) Bitpack_getu(instruction, 25, 0))); 
        } else {
                three_regs registersUsed;

                registersUsed.a = &(registers[(uint32_t) Bitpack_getu(instruction, 3, 6)]);
                registersUsed.b = &(registers[(uint32_t) Bitpack_getu(instruction, 3, 3)]);
                registersUsed.c = &(registers[(uint32_t) Bitpack_getu(instruction, 3, 0)]);
                
                switch(opcode)
                {
                        case 0:
                                return(cmov(registersUsed));
                        case 1:
                                return(sload(registersUsed));
                        case 2:
                                return(sstore(registersUsed));
                        case 3:
                                return(add(registersUsed));
                        case 4:
                                return(mult(registersUsed));
                        case 5:
                                return(divide(registersUsed));
                        case 6:
                                return(nand(registersUsed));
                        case 7:
                                return(halt(registersUsed));
                        case 8:
                                return(map(registersUsed));
                        case 9:
                                return(unmap(registersUsed));
                        case 10:
                                return(ALUoutput(registersUsed));
                        case 11:
                                return(ALUinput(registersUsed));
                        case 12:
                                return(loadp(registersUsed, pc));
                        default:
                                return 1;
                }
        }
}

/*
 *      interpretInstruction will unpack the given uint32_t 
 *      instruction and split it into an opcode, and three
 *      registers (a, b, and c.)
 *      interpretInstruction then puts these into a struct and
 *      returns it.
 */
// instruction_struct interpretInstruction(uint32_t instruction)
// {
//         instruction_struct instructions;

//         instructions.opcode = (uint32_t) Bitpack_getu(instruction, 4, 28);
//         instructions.a = (uint32_t) Bitpack_getu(instruction, 3, 6);
//         instructions.b = (uint32_t) Bitpack_getu(instruction, 3, 3);
//         instructions.c = (uint32_t) Bitpack_getu(instruction, 3, 0);

//         return instructions;
// }




