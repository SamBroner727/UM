#include <ioModule.h>
#include <stdlib.h>
#include <stdint.h>
#include "memoryManager.h"
#include "bitpack.h"


typedef struct three_regs {
        uint32_t* a;
        uint32_t* b;
        uint32_t* c;
} three_regs;

// typedef struct instruction_struct {
//         uint32_t opcode;
//         uint32_t a;
//         uint32_t b;
//         uint32_t c;
// } instruction_struct;

/*
 *      performOperation will call interpretInstruction to
 *      obtain an instruction that has been unpacked into
 *      a struct containing an opcode and 3 registers.
 *      performOperation then calls the function corresponding
 *      to the given opcode.
 */
int performOperation(uint32_t instruction, uint32_t *registers, int *pc);

/*
 *      interpretInstruction will unpack the given uint32_t 
 *      instruction and split it into an opcode, and three
 *      registers (a, b, and c.)
 *      interpretInstruction then puts these into a struct and
 *      returns it.
 */
// instruction_struct interpretInstruction(uint32_t instruction);




// /* Conditional Move */
// static inline int cmov(three_regs regs);

// /* Segmented Load */
// static inline int sload(three_regs regs);

// /* Segmented Store */
// static inline int sstore(three_regs regs);

// /* Addition */
// static inline int add(three_regs regs);

// /* Multiplication */
// static inline int mult(three_regs regs);

// /* Division */
// static inline int divide(three_regs regs);

// /* Bitwise NAND */
// static inline int nand(three_regs regs);

// /* Halt */
// static inline int halt(three_regs regs);

// /* Map Segment */
// static inline int map(three_regs regs);

// /* Unmap Segment */
// static inline int unmap(three_regs regs);

// /* Output */
// static inline int ALUoutput(three_regs regs);

// /* Input */
// static inline int ALUinput(three_regs regs);

// /* Load Program */
// static inline int loadp(three_regs regs); 

// /* Load Value */
// static inline int loadv(uint32_t* regA, uint32_t value);


