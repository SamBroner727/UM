#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bitpack.h"
#include "memoryManager.h"


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
void loadProgram(FILE *fp);

/* 
 *      output will take int val and output to stdout
 */
void IOoutput(uint32_t val);

/*  
 *      input will take the next value in stdin and return it
 *      EOF will return ~0
 */
uint32_t IOinput();
