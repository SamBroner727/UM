#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mem.h"
#include "seq.h"
#include "stack.h"
#include "assert.h"

 /*
  *      nextAvailSegId will keep track of a stack (Hanson 
  *      Sequence) that contains segids of segments that have
  *      been unmapped.
  *      This function will return the next segid from the 
  *      stack, or, if it is empty, the next available index of 
  *      the segment sequence
  */
uint32_t nextAvailSegId();


extern void newMemory(int segZeroLength);

/*
 *      newSegment will allocate memory and create a new
 *      segment in the UM's memory. The segment will be of
 *      length length. The segid will be found using
 *      nextAvailSegId.
 */
extern uint32_t newSegment(int length);

/*
 *      initializeSegmentZero is a function that specifically
 *      initializes array $m[0] and the sequence that
 *      represents the segmented memory.
 */
void initializeSegmentZero(int segZeroLength);

/*
 *      This function will remove the sequence and free all of
 *      the memory allocated to any remaining segments
 */
extern void deleteMemory();


/*
 *      removeSegment will deallocate a segment in memory
 *      (setting the pointer at its segid in memory to NULL)
 *      and put the segid of the unmapped segment into a stack
 *      (Hanson sequence) of unmapped/available segids
 *      If removeSegment fails, either because the sequence has not been
 *      created or because the only remaining memory segment is $m[0] this
 *      function will return 1.
 */
extern int removeSegment(uint32_t segid);

/*
 *      putWord will take in a uint32_t word and a segid
 *      and put the word in the indicated index of the
 *      requested segid. If index == null, put given word in
 *      next available index.
 *      If the segid requested has not been created, or 
 *      the index requested is out of bounds this function will
 *      call an error
 */
extern int putWord(uint32_t word, uint32_t segid, uint32_t offset);

/*      
 *      getWord will retrieve the word at the offset index
 *      of the array located at the segid in the segmented 
 *      memory sequence.
 *      If there is no word at the offset requested, this will
 *      return an error.
 */
extern uint32_t getWord(uint32_t segid, uint32_t offset);


/*
 *      Copies segment at orgin to segment at destination
 *
 *
 */
extern void replaceSegment(uint32_t originSegID, uint32_t destinationSegID);


