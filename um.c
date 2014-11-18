#include <stdint.h>
#include <stdio.h>
#include "memoryManager.h"
#include "ioModule.h"
#include "alu.h"



void initialize(FILE *fp, int* programCounter, uint32_t* registers);
int execute(int* programCounter, uint32_t* registers);


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




/*
 *      This will initialize and allocate memory for an array  
 *      of registers and call newMemory in the
 *      memoryManager
 *      Only segment zero of the segmented memory will be
 *      mapped and all registers are zero. Additionally the 
 *      program counter will point to $m[0][0]
 */
void initialize(FILE *fp, int *pc, uint32_t* registers) 
{

        *pc = 0;

        for (int i = 0; i < 8; i++) {
                registers[i] = (uint32_t) 0;
        }

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
int execute(int* pc, uint32_t* registers)
{

        int program_status = 0;

        while(program_status != 1) {

                *pc += 1;
                program_status = performOperation(getWord(0, *pc - 1), 
                                registers, pc);

        }

        return 0;

}

