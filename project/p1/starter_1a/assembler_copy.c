/**
 * Project 1
 * Assembler code fragment for LC-2K
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//Every LC2K file will contain less than 1000 lines of assembly.
#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
static void checkForBlankLinesInCode(FILE *inFilePtr);
static inline int isNumber(char *);
static inline void printHexToFile(FILE *, int);

void I_type_test(char *, char *, char *, bool, int);
//bool isBeq(char *);
int find_label_address(char *);

typedef struct{
    char label_name[MAXLINELENGTH];
    int address;
}Label;
Label label_table[MAXLINELENGTH];
int labelnum=0;



int
main(int argc, char **argv)
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }

    // Check for blank lines in the middle of the code.
    checkForBlankLinesInCode(inFilePtr);

    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

// first round
    int addressline = 0;
    while(1){
        if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
            break;
            /* reached end of file */
        }
        if(strlen(label) > 0){
            for(int i=0; i<labelnum; i++){
                if(strcmp(label, label_table[i].label_name) == 0){ // if equal
                    //printf("same label\n");
                    exit(1);
                }
            }
        label_table[labelnum].address = addressline;
        strcpy(label_table[labelnum].label_name, label);
        labelnum++;
        }
        addressline++;
    }

    // // check the label_table
    // for(int i=0; i<labelnum; i++){
    //     printf("Label: %s, Address: %d\n", label_table[i].label_name, label_table[i].address);
    // }



// second round

    addressline = 0;
    rewind(inFilePtr);
    while(1){
        if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
            break;
            /* reached end of file */
        }

        int machine_code_line = 0;

        if(!strcmp(opcode, "add")){

            // validation test
            if(isNumber(arg0) == 0 || isNumber(arg1) == 0 || isNumber(arg2) == 0){
                //printf("error: register index is not a number");
                exit(1);
            }
            int regA = atoi(arg0);
            int regB = atoi(arg1);
            int dest = atoi(arg2);
            if((regA>7 || regA<0) || (regB>7 || regB<0) || (dest>7 || dest<0)){
                //printf("error: register index out of range");
                exit(1);
            }

            //operation
            machine_code_line = (0)|(regA<<19)|(regB<<16)|dest;            //  0b0000000|000|regA|regB|0000000000000|dest;
        
        }else if(!strcmp(opcode, "nor")){
            // validation test
            if(isNumber(arg0) == 0 || isNumber(arg1) == 0 || isNumber(arg2) == 0){
                //printf("1error: register index is not a number");
                exit(1);
            }
            int regA = atoi(arg0);
            int regB = atoi(arg1);
            int dest = atoi(arg2);
            if((regA>7 || regA<0) || (regB>7 || regB<0) || (dest>7 || dest<0)){
                //printf("2error: register index out of range");
                exit(1);
            }

            //operation
            machine_code_line = (1<<22)|(regA<<19)|(regB<<16)|dest;

        }else if(!strcmp(opcode, "lw")){

            I_type_test(arg0, arg1, arg2, 0, addressline);

            int regA = atoi(arg0);
            int regB = atoi(arg1);
            int offset = 0;

            if(isNumber(arg2) == 1){                   // offset is number
                offset = atoi(arg2);
            }else{                                     // offset is label   
                offset = find_label_address(arg2);
            }

            machine_code_line = (2<<22)|(regA<<19)|(regB<<16)|(offset);

        }else if(!strcmp(opcode, "sw")){

            I_type_test(arg0, arg1, arg2, 0, addressline);

            int regA = atoi(arg0);
            int regB = atoi(arg1);
            int offset = 0;

            if(isNumber(arg2) == 1){                   // offset is number
                offset = atoi(arg2);
            }else{                                     // offset is label   
                offset = find_label_address(arg2);
            }

            machine_code_line = (3<<22)|(regA<<19)|(regB<<16)|(offset);

        }else if(!strcmp(opcode, "beq")){

            I_type_test(arg0, arg1, arg2, 1, addressline);

            int regA = atoi(arg0);
            int regB = atoi(arg1);
            int offset = 0;

            if(isNumber(arg2) == 1){                   // offset is number
                offset = atoi(arg2);
            }else{                                     // offset is label   
                //printf("label in beq's line: %s, corresponds to offset: %d\n", arg2, find_label_address(arg2)-1-addressline);
                offset = find_label_address(arg2) - 1 - addressline;
            }

            machine_code_line = (4<<22)|(regA<<19)|(regB<<16)|(offset & 0xFFFF);

        }else if(!strcmp(opcode, "jalr")){
            // validation test
            if(isNumber(arg0) == 0 || isNumber(arg1) == 0){
                //printf("error7: register index is not a number");
                exit(1);
            }
            int regA = atoi(arg0);
            int regB = atoi(arg1);
            if((regA>7 || regA<0) || (regB>7 || regB<0)){
                //printf("error8: register index out of range");
                exit(1);
            }

            machine_code_line = (5<<22)|(regA<<19)|(regB<<16);

        }else if(!strcmp(opcode, "halt")){
            machine_code_line = (6<<22);
        }else if(!strcmp(opcode, "noop")){
            machine_code_line = (7<<22);
        }else if(!strcmp(opcode, ".fill")){
            if(isNumber(arg0)){
                machine_code_line = atoi(arg0);
            }else{
                machine_code_line = find_label_address(arg0);
            }
        }else{
            //printf("error: no opcode matched: %s", opcode);
            exit(1);
        }
        
        printHexToFile(outFilePtr, machine_code_line);
        addressline++;

    }

    // /* here is an example for how to use readAndParse to read a line from
    //     inFilePtr */
    // if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
    //     /* reached end of file */
    // }

    // /* this is how to rewind the file ptr so that you start reading from the
    //     beginning of the file */
    // rewind(inFilePtr);

    // /* after doing a readAndParse, you may want to do the following to test the
    //     opcode */
    // if (!strcmp(opcode, "add")) {
    //     /* do whatever you need to do for opcode "add" */
    // }

    // /* here is an example of using isNumber. "5" is a number, so this will
    //    return true */
    // if(isNumber("5")) {
    //     printf("It's a number\n");
    // }

    // /* here is an example of using printHexToFile. This will print a
    //    machine code word / number in the proper hex format to the output file */
    // printHexToFile(outFilePtr, 123);



    return(0);
}

// Returns non-zero if the line contains only whitespace.
static int lineIsBlank(char *line) {
    char whitespace[4] = {'\t', '\n', '\r', ' '};
    int nonempty_line = 0;
    for(int line_idx=0; line_idx < strlen(line); ++line_idx) {
        int line_char_is_whitespace = 0;
        for(int whitespace_idx = 0; whitespace_idx < 4; ++ whitespace_idx) {
            if(line[line_idx] == whitespace[whitespace_idx]) {
                line_char_is_whitespace = 1;
                break;
            }
        }
        if(!line_char_is_whitespace) {
            nonempty_line = 1;
            break;
        }
    }
    return !nonempty_line;
}

// Exits 2 if file contains an empty line anywhere other than at the end of the file.
// Note calling this function rewinds inFilePtr.
static void checkForBlankLinesInCode(FILE *inFilePtr) {
    char line[MAXLINELENGTH];
    int blank_line_encountered = 0;
    int address_of_blank_line = 0;
    rewind(inFilePtr);

    for(int address = 0; fgets(line, MAXLINELENGTH, inFilePtr) != NULL; ++address) {
        // Check for line too long
        if (strlen(line) >= MAXLINELENGTH-1) {
            printf("error: line too long\n");
            exit(1);
        }

        // Check for blank line.
        if(lineIsBlank(line)) {
            if(!blank_line_encountered) {
                blank_line_encountered = 1;
                address_of_blank_line = address;
            }
        } else {
            if(blank_line_encountered) {
                printf("Invalid Assembly: Empty line at address %d\n", address_of_blank_line);
                exit(2);
            }
        }
    }
    rewind(inFilePtr);
}


/*
* NOTE: The code defined below is not to be modifed as it is implimented correctly.
*/

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }

    /* check for line too long */
    if (strlen(line) == MAXLINELENGTH-1) {
	printf("error: line too long\n");
	exit(1);
    }

    // Ignore blank lines at the end of the file.
    if(lineIsBlank(line)) {
        return 0;
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);

    return(1);
}

static inline int
isNumber(char *string)
{
    int num;
    char c;
    return((sscanf(string, "%d%c",&num, &c)) == 1);
}


// Prints a machine code word in the proper hex format to the file
static inline void 
printHexToFile(FILE *outFilePtr, int word) {
    fprintf(outFilePtr, "0x%08X\n", word);
}

void I_type_test(char *arg0, char *arg1, char *arg2, bool isBeq, int PC){
    // validation test of reg
    if(isNumber(arg0) == 0 || isNumber(arg1) == 0){
        //printf("3error: register index is not a number\n");
        exit(1);
    }
    int regA = atoi(arg0);
    int regB = atoi(arg1);
    if((regA>7 || regA<0) || (regB>7 || regB<0)){
        //printf("4error: register index out of range\n");
        exit(1);
    }
    // validation test of offset
    int offset = 0;
    if(isNumber(arg2) == 1){
        offset = atoi(arg2);
    }else{               //  offset is a label
        if(!isBeq){                                   // lw or sw
            offset = find_label_address(arg2);           //printf("offset here: %d", offset);
        }else{                                        // beq
            offset = find_label_address(arg2) - 1 - PC;
        }
    }
    if(offset < -32768 || offset > 32767){
        //printf("5error: offset out of range\n");
        exit(1);
    }
}

int find_label_address(char *label_name){
    for(int i=0; i<labelnum; i++){
        if(strcmp(label_name, label_table[i].label_name) == 0){
            return label_table[i].address;
        }
    }
    //printf("error6: label name cannot be found\n");
    exit(1);
}