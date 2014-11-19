#include "memoryManager.h"

/*
Speed Notes

Stack:
    Struct with an int length and a uint32_t array.
*/


Stack_T stack;


typedef struct segment {
        uint32_t *words;
        int length;
} segment;

typedef struct mem_struct {
        uint32_t memorySize;
        uint32_t memoryCapacity;
        segment **memory;

} mem_struct;

void expand();

mem_struct myMem;

/*
 *      nextAvailSegId will keep track of a stack (Hanson 
 *      Sequence) that contains segids of segments that have
 *      been unmapped.
 *      This function will return the next segid from the 
 *      stack, or, if it is empty, the next available index of 
 *      the segment sequence
 */
uint32_t nextAvailSegId()
{
        if (!Stack_empty(stack)) {
                uint32_t * tempID = (uint32_t *)Stack_pop(stack);
                uint32_t removedId = *tempID;

                free(tempID);

                return removedId;
        }
        myMem.memorySize++;
        return myMem.memorySize - 1;
}

/*
 *      newMemory initializes sequence memory and the stack stack
 *      Afterwords, it is asserted that enough memory was created
 *      and SegmentZero is initialized with the proper length.
 */
extern void newMemory(int segZeroLength)
{

        myMem.memory = malloc(sizeof(segment *) * 2);
        myMem.memoryCapacity = 2;

        initializeSegmentZero(segZeroLength);

        myMem.memorySize = 1;

        stack = Stack_new();
        assert(stack);
}

/*
 *      newSegment will allocate memory and create a new
 *      segment in the UM's memory. The segment will be of
 *      length length. The segid will be found using
 *      nextAvailSegId.
 */
extern uint32_t newSegment(int length)
{

        segment *newSeg = malloc(sizeof(*newSeg));
        newSeg->words = malloc(length * sizeof(uint32_t));
        newSeg->length = length;

        for(int i = 0; i < length; i++) {
            newSeg->words[i] = 0;
        }

        uint32_t segid = nextAvailSegId();

        if (segid == myMem.memoryCapacity){
            expand();
        }
        myMem.memory[segid] = newSeg;

        return segid;
}

/*
 *      initializeSegmentZero is a function that specifically
 *      initializes array $m[0] and the sequence that
 *      represents the segmented memory.
 */
void initializeSegmentZero(int segZeroLength)
{
        
        segment* segmentZero = malloc(sizeof(*segmentZero));
        segmentZero->words = malloc(segZeroLength * sizeof(uint32_t));
        segmentZero->length = segZeroLength;

        myMem.memory[0] = segmentZero;
}

/*
 *      This function will remove the sequence and free all of
 *      the memory allocated to any remaining segments
 */
extern void deleteMemory()
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

        while(!Stack_empty(stack)) {
            free(Stack_pop(stack));
        }
        Stack_free(&stack);
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
extern int removeSegment(uint32_t segid)
{

        uint32_t *segid_ptr = malloc(sizeof (*segid_ptr));

        *segid_ptr = segid;

        Stack_push(stack, (void *) segid_ptr);

        free((myMem.memory[segid])->words);
        free(myMem.memory[segid]);

        myMem.memory[segid] = NULL;

        return 0;

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
 //c, a, b
extern int putWord(uint32_t word, uint32_t segid, uint32_t offset)
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
extern uint32_t getWord(uint32_t segid, uint32_t offset)
{

        return (myMem.memory[segid])->words[offset];

}


/*
 *      Copies segment at orgin to segment at destination
 */
extern void replaceSegment(uint32_t originSegID, uint32_t destinationSegID)
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

void expand()
{
        myMem.memoryCapacity = myMem.memoryCapacity * 2;
        segment* *temp = malloc(myMem.memoryCapacity * (sizeof(segment *)));
        
        for (uint32_t i = 0; i < myMem.memorySize; i++) {
            temp[i] = myMem.memory[i];
        }

        free(myMem.memory);

        myMem.memory = temp;
}