#include <stdint.h>
#include <stdio.h>
#include "bitpack.h"
#include <stdlib.h>



// void initialize(FILE *fp, int* programCounter, uint32_t* registers);
// int execute(int* programCounter, uint32_t* registers);


typedef struct segment {
        uint32_t *words;
        int length;
} segment;

typedef struct stack_struct {
        uint32_t stackSize;
        uint32_t stackCapacity;
        uint32_t *stack;
} stack_struct;


typedef struct mem_struct {
        uint32_t memorySize;
        uint32_t memoryCapacity;
        segment **memory;

} mem_struct;

mem_struct myMem;
stack_struct myStack;

typedef struct three_regs {
        uint32_t* a;
        uint32_t* b;
        uint32_t* c;
} three_regs;

/*
 *      initializeSegmentZero is a function that specifically
 *      initializes array $m[0] and the sequence that
 *      represents the segmented memory.
 */
static inline void initializeSegmentZero(int segZeroLength)
{
        
        segment* segmentZero = malloc(sizeof(*segmentZero));
        segmentZero->words = malloc(segZeroLength * sizeof(uint32_t));
        segmentZero->length = segZeroLength;

        myMem.memory[0] = segmentZero;
}

/*
 *      newMemory initializes sequence memory and the stack stack
 *      Afterwords, it is asserted that enough memory was created
 *      and SegmentZero is initialized with the proper length.
 */
static inline void newMemory(int segZeroLength)
{

        myMem.memory = malloc(sizeof(segment *) * 2);
        myMem.memoryCapacity = 2;

        initializeSegmentZero(segZeroLength);

        myMem.memorySize = 1;

        myStack.stack = malloc(sizeof(uint32_t) * 2);
        myStack.stackCapacity = 2;
        myStack.stackSize = 0;
}





/*
 *      This function will remove the sequence and free all of
 *      the memory allocated to any remaining segments
 */
static inline void deleteMemory()
{
        // TODO : move Seq_get into a local variable... we are assuming
        // that it's not null more often
        for (uint32_t i = 0; i < myMem.memorySize; i++) {
                if (myMem.memory[i] != NULL) {
                    free((myMem.memory[i])->words);
                    free(myMem.memory[i]);
                }
        }
        free(myMem.memory);

        free(myStack.stack);
}



/*
 *      memoryExpand function
 *
 */
static inline void memoryExpand()
{
        myMem.memoryCapacity = myMem.memoryCapacity * 2;
        segment* *temp = malloc(myMem.memoryCapacity * (sizeof(segment *)));
        
        for (uint32_t i = 0; i < myMem.memorySize; i++) {
            temp[i] = myMem.memory[i];
        }

        free(myMem.memory);

        myMem.memory = temp;
}

/*
 *      stackExpand function
 *
 */
static inline void stackExpand()
{
        myStack.stackCapacity = myStack.stackCapacity * 2;
        uint32_t *temp = malloc(myMem.memoryCapacity * (sizeof(uint32_t)));
        
        for (uint32_t i = 0; i < myStack.stackSize; i++) {
            temp[i] = myStack.stack[i];
        }

        free(myStack.stack);

        myStack.stack = temp;
}
/*
 *      Copies segment at orgin to segment at destination
 */
static inline void replaceSegment(uint32_t originSegID, uint32_t destinationSegID)
{

        segment *seg_new = malloc(sizeof(*seg_new));

        segment *seg_to_dup = myMem.memory[originSegID];

        seg_new->words = malloc(sizeof(uint32_t) * seg_to_dup->length);

        for(int i = 0; i < seg_to_dup->length; i++) {
            seg_new->words[i] = seg_to_dup->words[i];
        }

        free((myMem.memory[destinationSegID])->words);
        free(myMem.memory[destinationSegID]);

        myMem.memory[destinationSegID] = seg_new;
}


/*
 *      nextAvailSegId will keep track of a stack (Hanson 
 *      Sequence) that contains segids of segments that have
 *      been unmapped.
 *      This function will return the next segid from the 
 *      stack, or, if it is empty, the next available index of 
 *      the segment sequence
 */
static inline uint32_t nextAvailSegId()
{
        if (!myStack.stackSize == 0) {
                uint32_t segId = myStack.stack[myStack.stackSize - 1];
                myStack.stackSize--;
                return segId;
        }
        myMem.memorySize++;
        return myMem.memorySize - 1;
}

/*
 *      removeSegment will deallocate a segment in memory
 *      (setting the pointer at its segid in memory to NULL)
 *      and put the segid of the unmapped segment into a stack
 *      (Hanson sequence) of unmapped/available segids
 *      If removeSegment fails, either because the sequence has not been
 *      created or because the only remaining memory segment is $m[0] this
 *      function will return 1.
 */
static inline int removeSegment(uint32_t segid)
{

        if(myStack.stackSize == myStack.stackCapacity) {
                stackExpand();
        }
        myStack.stack[myStack.stackSize] = segid;
        myStack.stackSize++;

        free((myMem.memory[segid])->words);
        free(myMem.memory[segid]);

        myMem.memory[segid] = NULL;

        return 0;

}

/*
 *      newSegment will allocate memory and create a new
 *      segment in the UM's memory. The segment will be of
 *      length length. The segid will be found using
 *      nextAvailSegId.
 */
static inline uint32_t newSegment(int length)
{

        segment *newSeg = malloc(sizeof(*newSeg));
        newSeg->words = malloc(length * sizeof(uint32_t));
        newSeg->length = length;

        for(int i = 0; i < length; i++) {
            newSeg->words[i] = 0;
        }

        uint32_t segid = nextAvailSegId();

        if (segid == myMem.memoryCapacity){
            memoryExpand();
        }
        myMem.memory[segid] = newSeg;

        return segid;
}



/*  
 *      input will take the next value in stdin and return it
 *      EOF will return ~0
 */
static inline uint32_t IOinput() 
{
        int input = fgetc(stdin);
        if (input == EOF) {
                return ~0;
        } else {
                return (uint32_t) input;
        }
}

/*
 *      putWord will take in a uint32_t word and a segid
 *      and put the word in the indicated index of the
 *      requested segid. If index == null, put given word in
 *      next available index.
 *      If the segid requested has not been created, or 
 *      the index requested is out of bounds this function will
 *      call an error
 */
static inline int putWord(uint32_t word, uint32_t segid, uint32_t offset)
{
               (myMem.memory[segid])->words[offset] = word;
               return 0;
}

/*      
 *      getWord will retrieve the word at the offset index
 *      of the array located at the segid in the segmented 
 *      memory sequence.
 *      If there is no word at the offset requested, this will
 *      return an error.
 */
static inline uint32_t getWord(uint32_t segid, uint32_t offset)
{
        return (myMem.memory[segid])->words[offset];
}

/***************************ALU FUNCTIONS**********************/

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
        printf("%c", *(regs.c));        
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







/*
 *      Execute runs a loop that increments the program counter
 *      and performs each operation as requested by the 
 *      instruction word pointed too by the program counter.
 *      This function will return an integer to indicate if the
 *      program succeeded or failed.
 */
static inline int execute(int* pc, uint32_t* registers)
{

        int program_status = 0;


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
                                program_status = 1;
                        } else if (opcode == 11) {
                                program_status = (ALUinput(registersUsed));
                        } else {
                                program_status =  1;
                        }
                }
        }
        return 0;
}


/*
 *      This will initialize and allocate memory for an array  
 *      of registers and call newMemory in the
 *      memoryManager
 *      Only segment zero of the segmented memory will be
 *      mapped and all registers are zero. Additionally the 
 *      program counter will point to $m[0][0]
 */
static inline void initialize(FILE *fp, int *pc, uint32_t* registers) 
{

        *pc = 0;

        for (int i = 0; i < 8; i++) {
                registers[i] = (uint32_t) 0;
        }

        fseek(fp, 0, SEEK_END);
        int programLength = ftell(fp);
        rewind(fp);

        newMemory(programLength);

        int currentLine = 0;
        int c;

        uint32_t newInstruction;
        while((c = fgetc(fp)) != EOF) {
                newInstruction = 0;
                ungetc(c, fp);

                newInstruction = Bitpack_newu(newInstruction, 8, 24, fgetc(fp));
                newInstruction = Bitpack_newu(newInstruction, 8, 16, fgetc(fp));
                newInstruction = Bitpack_newu(newInstruction, 8, 8, fgetc(fp));
                newInstruction = Bitpack_newu(newInstruction, 8, 0, fgetc(fp));

                putWord(newInstruction, 0, currentLine++);
        }
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
        uint32_t* registers = malloc(sizeof(uint32_t) * 8);
        
        initialize(fp, pc, registers);
        execute(pc, registers);

        fclose(fp);

        free(pc);
        free(registers);

        deleteMemory();

}






