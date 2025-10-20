/*
 * Project 1
 * EECS 370 LC-2K Instruction-level simulator
 *
 * Make sure to NOT modify printState or any of the associated functions
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//DO NOT CHANGE THE FOLLOWING DEFINITIONS 

// Machine Definitions
#define MEMORYSIZE 65536 /* maximum number of words in memory (maximum number of lines in a given file)*/
#define NUMREGS 8 /*total number of machine registers [0,7]*/

// File Definitions
#define MAXLINELENGTH 1000 /* MAXLINELENGTH is the max number of characters we read */

typedef struct 
stateStruct {
    int pc;
    int mem[MEMORYSIZE];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);

static inline int convertNum(int32_t);

int 
main(int argc, char **argv)
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s , please ensure you are providing the correct path", argv[1]);
        perror("fopen");
        exit(2);
    }


    /* read the entire machine-code file into memory */
    for (state.numMemory=0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
		    if (state.numMemory >= MEMORYSIZE) {
			      fprintf(stderr, "exceeded memory size\n");
			      exit(2);
		    }
		    if (sscanf(line, "%x", state.mem+state.numMemory) != 1) {
			      fprintf(stderr, "error in reading address %d\n", state.numMemory);
			      exit(2);
		    }
		    printf("memory[%d]=0x%08X\n", state.numMemory, state.mem[state.numMemory]);
    }



    // initialization of regs and pc
    state.pc = 0;
    for(int i=0; i < NUMREGS; i++){    // 8 regs intotal
        state.reg[i] = 0;
    }

    int round = 0;
    // begin state
    while(1){
        printState(&state);

        int instruction = state.mem[state.pc];
        int opcode = (instruction >> 22) & 7;
        int regA = (instruction >> 19) & 7;
        int regB = (instruction >> 16) & 7;

        if(opcode == 0){             // add
            int destR = instruction & 7;
            state.reg[destR] = state.reg[regA] + state.reg[regB];
            state.pc++;
        }else if(opcode == 1){       // nor
            int destR = instruction & 7;
            state.reg[destR] = ~(state.reg[regA] | state.reg[regB]);
            state.pc++; 
        }else if(opcode == 2){       // lw
            int offset = (instruction) & 0xFFFF;
            offset = convertNum(offset);
            state.reg[regB] = state.mem[state.reg[regA] + offset];
            state.pc++;
        }else if(opcode == 3){       // sw
            int offset = instruction & 0xFFFF;
            offset = convertNum(offset);
            state.mem[offset + state.reg[regA]] = state.reg[regB];
            state.pc++;
        }else if(opcode == 4){       // beq
            int offset = instruction & 0xFFFF;
            offset = convertNum(offset);
            if(state.reg[regA] == state.reg[regB]){
                state.pc = state.pc + 1 + offset;                       //  there are probably some issues here
            }else{
                state.pc++;
            }
            
        }else if(opcode == 5){       // jalr
            state.reg[regB] = state.pc + 1;
            state.pc = state.reg[regA];
        }else if(opcode == 6){       // halt
            printf("machine halted\n");
            printf("total of %d instructions executed\n", round+1);
            printf("final state of machine:\n");
            state.pc++;
            printState(&state);

            exit(0);
        }else if(opcode == 7){       // noop
            state.pc++;
        }else{
            printf("can't find opcode\n");
            exit(1);
        }
        

    }











    //Your code ends here! 

    return(0);
}

/*
* DO NOT MODIFY ANY OF THE CODE BELOW. 
*/

void printState(stateType *statePtr) {
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] 0x%08X\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
	  for (i=0; i<NUMREGS; i++) {
	      printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	  }
    printf("end state\n");
}

// convert a 16-bit number into a 32-bit Linux integer
static inline int convertNum(int num) 
{
    return num - ( (num & (1<<15)) ? 1<<16 : 0 );
}

/*
* Write any helper functions that you wish down here. 
*/
