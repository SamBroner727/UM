#include <ioModule.h>

/*  
 *      Takes in a file pointer that contains a sequence of
 *      characters stored in little endian that represent the 
 *      instructions of the program.
 *
 *      This function loops through the characters and turns 
 *      each 4 characters into an instruction word using 
 *      decodeWord. This instruction is placed at the next
 *      spot in $m[0] using putWord.
 * 
 */
void loadProgram(FILE *fp)
{


        int currentLine = 0;

        int c;

        //TODO
        //uint32_t newInstruction;
        while((c = fgetc(fp)) != EOF) {
                uint32_t newInstruction = 0;
                ungetc(c, fp);

                newInstruction = Bitpack_newu(newInstruction, 8, 24, fgetc(fp));
                newInstruction = Bitpack_newu(newInstruction, 8, 16, fgetc(fp));
                newInstruction = Bitpack_newu(newInstruction, 8, 8, fgetc(fp));
                newInstruction = Bitpack_newu(newInstruction, 8, 0, fgetc(fp));

                putWord(newInstruction, 0, currentLine++);
        }
}


/* 
 *      output will take int val and output to stdout
 */
void IOoutput(uint32_t val)
{
        printf("%c", val);
}

/*  
 *      input will take the next value in stdin and return it
 *      EOF will return ~0
 */
uint32_t IOinput() 
{
        int input = fgetc(stdin);
        if (input == EOF) {
                return ~0;
        } else {
                return (uint32_t) input;
        }
}


