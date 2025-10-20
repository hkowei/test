/**
 * Project 2
 * LC-2K Linker
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXSIZE 500
#define MAXLINELENGTH 1000
#define MAXFILES 6

static inline void printHexToFile(FILE *, int);

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry {
	char label[7];
	char location;
	unsigned int offset;
};

struct RelocationTableEntry {
    unsigned int file;
	unsigned int offset;
	char inst[6];
	char label[7];
};

struct FileData {
	unsigned int textSize;
	unsigned int dataSize;
	unsigned int symbolTableSize;
	unsigned int relocationTableSize;
	unsigned int textStartingLine; // in final executable
	unsigned int dataStartingLine; // in final executable
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symbolTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles {
	unsigned int textSize;
	unsigned int dataSize;
	unsigned int symbolTableSize;
	unsigned int relocationTableSize;
	int text[MAXSIZE * MAXFILES];
	int data[MAXSIZE * MAXFILES];
	SymbolTableEntry symbolTable[MAXSIZE * MAXFILES];
	RelocationTableEntry relocTable[MAXSIZE * MAXFILES];
};

int filenum = 0;
int Stack = 0;

void resolve_local(FileData *);
int isLocal(char *);
void initMasterFile(CombinedFiles *, FileData [], int);
void resolve_global(FileData*, FileData []);
int findsymbol(char *, FileData []); // return final address of a global label
static inline void write_low16_to_text(int *textArray, unsigned int index, unsigned int low16) {
    unsigned int orig = (unsigned int) textArray[index];
    unsigned int newWord = (orig & 0xFFFF0000u) | (low16 & 0xFFFFu);
    textArray[index] = (int)newWord;
}


int main(int argc, char *argv[]) {
	char *inFileStr, *outFileStr;
	FILE *inFilePtr, *outFilePtr; 
	unsigned int i, j;

    if (argc <= 2 || argc > 8 ) {
        printf("error: usage: %s <MAIN-object-file> ... <object-file> ... <output-exe-file>, with at most 5 object files\n",
				argv[0]);
		exit(1);
	}

	outFileStr = argv[argc - 1];

	outFilePtr = fopen(outFileStr, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileStr);
		exit(1);
	}

	FileData files[MAXFILES];

  // read in all files and combine into a "master" file
	for (i = 0; i < argc - 2; ++i) {                                                
		inFileStr = argv[i+1];

		inFilePtr = fopen(inFileStr, "r");
		printf("opening %s\n", inFileStr);

		if (inFilePtr == NULL) {
			printf("error in opening %s\n", inFileStr);
			exit(1);
		}

		char line[MAXLINELENGTH];
		unsigned int textSize, dataSize, symbolTableSize, relocationTableSize;

		// parse first line of file
		fgets(line, MAXSIZE, inFilePtr);
		sscanf(line, "%d %d %d %d",
				&textSize, &dataSize, &symbolTableSize, &relocationTableSize);

		files[i].textSize = textSize;
		files[i].dataSize = dataSize;
		files[i].symbolTableSize = symbolTableSize;
		files[i].relocationTableSize = relocationTableSize;
																								 
		// read in text section
		// int instr;
		// for (j = 0; j < textSize; ++j) {
		// 	fgets(line, MAXLINELENGTH, inFilePtr);
		// 	instr = strtol(line, NULL, 0);
		// 	files[i].text[j] = instr;
		// }
		unsigned long uinstr;
		for (j = 0; j < textSize; ++j) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			uinstr = strtoul(line, NULL, 0);        // use unsigned parse
			files[i].text[j] = (int)uinstr;         // store bit pattern correctly
		}

		// read in data section
		// int data;
		// for (j = 0; j < dataSize; ++j) {
		// 	fgets(line, MAXLINELENGTH, inFilePtr);
		// 	data = strtol(line, NULL, 0);
		// 	files[i].data[j] = data;
		// }
		unsigned long udata;
		for (j = 0; j < dataSize; ++j) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			udata = strtoul(line, NULL, 0);         // use unsigned parse
			files[i].data[j] = (int)udata;          // store bit pattern correctly
		}
																								//printf("fuck %d\n", i);
		// read in the symbol table
		char label[7];
		char type;
		unsigned int addr;
		for (j = 0; j < symbolTableSize; ++j) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%s %c %d",
					label, &type, &addr);
			files[i].symbolTable[j].offset = addr;
			strcpy(files[i].symbolTable[j].label, label);
			files[i].symbolTable[j].location = type;
		}
																								//printf("fuck %d\n", i);
		// read in relocation table
		char opcode[7];
		for (j = 0; j < relocationTableSize; ++j) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%d %s %s",
					&addr, opcode, label);
			files[i].relocTable[j].offset = addr;
			strcpy(files[i].relocTable[j].inst, opcode);
			strcpy(files[i].relocTable[j].label, label);
			files[i].relocTable[j].file	= i;
		}
		fclose(inFilePtr);
	} // end reading files

	// *** INSERT YOUR CODE BELOW ***
	//    Begin the linking process
	//    Happy coding!!!


	filenum = argc - 2;

	// startingline
	int startingline = 0;
	for(int i=0; i < argc - 2; i++){
		files[i].textStartingLine = startingline;
		startingline += files[i].textSize;
	}
	for(int i=0; i < argc - 2; i++){
		files[i].dataStartingLine = startingline;
		startingline += files[i].dataSize;
	}

	
	CombinedFiles masterfile;                        //            initialization??
	memset(&masterfile, 0, sizeof(masterfile));
	//initMasterFile(&masterfile, files, argc - 2);

	// for(int i=0; i < argc - 2; i++){
	// 	masterfile.textSize += files[i].textSize;
	// 	masterfile.dataSize += files[i].dataSize;
	// 	masterfile.symbolTableSize += files[i].symbolTableSize;
	// 	masterfile.relocationTableSize += files[i].relocationTableSize;
	// }
	// // text
	// int special_index = 0;
	// for(int k=0; k < argc - 2; k++){
	// 	for(int i=0; i < files[k].textSize;i++){
	// 		masterfile.text[special_index++] = files[k].text[i];
	// 	}
	// }
	// // data
	// special_index = 0;
	// for(int k=0; k < argc - 2; k++){
	// 	for(int i=0; i < files[k].dataSize;i++){
	// 		masterfile.data[special_index++] = files[k].data[i];
	// 	}
	// }
	// // symbol table
	// special_index = 0;
	// for(int k=0; k< argc - 2; k++){
	// 	for(int i=0; i<files[k].symbolTableSize; i++){
	// 		masterfile.symbolTable[special_index++] = files[k].symbolTable[i];
	// 	}
	// }
	// // reloc table
	// special_index = 0;
	// for(int k=0; k< argc - 2; k++){
	// 	for(int i=0; i<files[k].relocationTableSize; i++){
	// 		masterfile.relocTable[special_index++] = files[k].relocTable[i];
	// 	}
	// }
	
	for(int i=0; i < argc-2; i++){
		resolve_local(&files[i]);
	}

	initMasterFile(&masterfile, files, argc - 2);
	Stack = masterfile.textSize + masterfile.dataSize;   //  init Stack
	memset(&masterfile, 0, sizeof(masterfile));

	for(int i=0; i<argc - 2; i++){
		resolve_global(&files[i], files);
	}
	initMasterFile(&masterfile, files, argc - 2);

	for (i = 0; i < masterfile.textSize; ++i) printHexToFile(outFilePtr, masterfile.text[i]);
	for (i = 0; i < masterfile.dataSize; ++i) printHexToFile(outFilePtr, masterfile.data[i]);
	fclose(outFilePtr);



    /* here is an example of using printHexToFile. This will print a
       machine code word / number in the proper hex format to the output file */
    //printHexToFile(outFilePtr, 123);

} // main

// Prints a machine code word in the proper hex format to the file
static inline void 
printHexToFile(FILE *outFilePtr, int word) {
    fprintf(outFilePtr, "0x%08X\n", word);
}

void resolve_local(FileData *objfile){
	for(int i=0 ; i < objfile->relocationTableSize; i++){
		int incre = 0;
		if(isLocal(objfile->relocTable[i].label)){              // is local label
			if(strcmp(objfile->relocTable[i].inst, ".fill") == 0){  //    D type
 				int reloc_off = objfile->relocTable[i].offset;
				int fill_val = objfile->data[reloc_off];
				if(fill_val > objfile->textSize - 1){      // label in D
					incre = objfile->dataStartingLine - objfile->textSize;
				}else{                                     // label is in T
					incre = objfile->textStartingLine;
				}
				objfile->data[reloc_off] += incre;
			}else{                                                  //    T type, sw or lw
				int reloc_off = objfile->relocTable[i].offset;
				int I_off = objfile->text[reloc_off] & 0xFFFF; // get offset for lwsw
				if(I_off > objfile->textSize - 1){         // label is in D
					incre = objfile->dataStartingLine - objfile->textSize;
				}else{                                     // label is in T
					incre = objfile->textStartingLine;
				}
				//objfile->text[reloc_off] += incre;
				// unsigned int orig = (unsigned int) objfile->text[reloc_off];
				// unsigned int newLow = ((orig & 0xFFFFu) + (incre & 0xFFFFu)) & 0xFFFFu;
				// unsigned int newWord = (orig & 0xFFFF0000u) | newLow;
				// objfile->text[reloc_off] = (int)newWord;
				unsigned int orig16 = (unsigned int)(objfile->text[reloc_off]) & 0xFFFFu;
				unsigned int newLow = (orig16 + (incre & 0xFFFFu)) & 0xFFFFu;
				write_low16_to_text(objfile->text, reloc_off, newLow);

			}
		}
	}
}

int isLocal(char *label){
	if(label[0]>='a' && label[0] <= 'z'){
		return 1;
	}
	return 0;
}

void initMasterFile(CombinedFiles *master, FileData files[], int numFiles) {
    for (int i = 0; i < numFiles; i++) {
        master->textSize += files[i].textSize;
        master->dataSize += files[i].dataSize;
        master->symbolTableSize += files[i].symbolTableSize;
        master->relocationTableSize += files[i].relocationTableSize;
    }
    //  text
    int idx = 0;
    for (int i = 0; i < numFiles; i++) {
        for (int j = 0; j < files[i].textSize; j++) {
            master->text[idx++] = files[i].text[j];
        }
    }
    //  data
    idx = 0;
    for (int i = 0; i < numFiles; i++) {
        for (int j = 0; j < files[i].dataSize; j++) {
            master->data[idx++] = files[i].data[j];
        }
    }
    //  symbolTable
    idx = 0;
    for (int i = 0; i < numFiles; i++) {
        for (int j = 0; j < files[i].symbolTableSize; j++) {
			if(strcmp(files[i].symbolTable[j].label, "Stack") == 0 && files[i].symbolTable[j].location != 'U'){
				//printf("error1: Stack deinfed by object\n");
				exit(1);
			}
			for(int k=0; k<idx; k++){
				if(strcmp(master->symbolTable[k].label, files[i].symbolTable[j].label) == 0){
					if(master->symbolTable[k].location != 'U' && files[i].symbolTable[j].location != 'U'){
						//printf("error2: duplicate defined global\n");
						exit(1);
					}
					goto next_symbol;
				}
			}
            master->symbolTable[idx++] = files[i].symbolTable[j];
			next_symbol:
				continue;
        }
    }
    //  relocationTable
    idx = 0;
    for (int i = 0; i < numFiles; i++) {
        for (int j = 0; j < files[i].relocationTableSize; j++) {
            master->relocTable[idx++] = files[i].relocTable[j];
        }
    }
}

void resolve_global(FileData *objfile, FileData files[]){
	for(int i=0; i<objfile->relocationTableSize; i++){
		//int incre = 0;
		if(!isLocal(objfile->relocTable[i].label)){                  // global
			if(strcmp(objfile->relocTable[i].label, "Stack")!=0){
				if(strcmp(objfile->relocTable[i].inst, ".fill") == 0){		// D type
					int reloc_off = objfile->relocTable[i].offset;
					//int fill_val = objfile->data[reloc_off];
					char glabel[7];
					strcpy(glabel, objfile->relocTable[i].label);
					objfile->data[reloc_off] = (int)findsymbol(glabel, files);
				}else{														// T type
					int reloc_off = objfile->relocTable[i].offset;
					//int I_off = objfile->text[reloc_off] & 0xFFFF;
					char glabel[7];
					strcpy(glabel, objfile->relocTable[i].label);
					//objfile->text[reloc_off] += findsymbol(glabel, files);  //  findsymbol is offset of the last 16 bits
					unsigned int newAddr = (unsigned int)findsymbol(glabel, files);
					write_low16_to_text(objfile->text, reloc_off, newAddr & 0xFFFFu);

				}
			}else{         // labe is stack
				if(strcmp(objfile->relocTable[i].inst, ".fill") == 0){		// D type
					int reloc_off = objfile->relocTable[i].offset;
					//int fill_val = objfile->data[reloc_off];
					objfile->data[reloc_off] = Stack;
				}else{														// T type
					int reloc_off = objfile->relocTable[i].offset;
					//int I_off = objfile->text[reloc_off] & 0xFFFF;
					//objfile->text[reloc_off] += Stack;  //  offset is modified as Stack

					write_low16_to_text(objfile->text, reloc_off, (unsigned int)Stack & 0xFFFFu);
				}

			}
		}
	}
}

int findsymbol(char* glabel, FileData files[]){                                       //printf("fuck fuck \n");
	for(int i=0; i<filenum; i++){            // file
		for(int j=0; j < files[i].symbolTableSize; j++){    // symbol
			// if(strcmp(files[i].symbolTable[j].label, "Stack") == 0){
			// 	printf("error1: Stack deinfed by object\n");
			// 	exit(1);
			// }
			if(strcmp(files[i].symbolTable[j].label, glabel) == 0){
				if(files[i].symbolTable[j].location == 'T'){
					return files[i].textStartingLine + files[i].symbolTable[j].offset;
				}else if(files[i].symbolTable[j].location == 'D'){
					return files[i].dataStartingLine + files[i].symbolTable[j].offset;
				} //  don't care about 'U'
			}
		}                                                                             //printf("fuck fuck %d\n",i);
	}
	//printf("error3: cannot find the global label location in all files\n");
	exit(1);
	return 0;
}// return final address of a global label

