#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100

// Function prototypes
void readIntermediateFile();
void readSymbolTable();
void processIntermediateCode();
void generateObjectCode();
void writeToObjectFile();
void writeToOutputFile(char *line);
void displayFiles();

// File pointers
FILE *intermediateFile, *symtabFile, *outputFile, *objectFile;

// Structures to hold relevant data
struct symbol {
    char label[MAX_LEN];
    int address;
};

struct mnemonic {
    char code[MAX_LEN];
    char machineCode[MAX_LEN];
};

struct symbol symtab[MAX_LEN];
struct mnemonic mnemonics[MAX_LEN];

int symCount = 0;
int mnemonicCount = 0;
int startAddr, finalAddr, currentAddr;

void readSymbolTable() {
    symtabFile = fopen("symtab.txt", "r");
    if (!symtabFile) {
        printf("Error opening symbol table file.\n");
        exit(1);
    }

    while (fscanf(symtabFile, "%s %d", symtab[symCount].label, &symtab[symCount].address) != EOF) {
        symCount++;
    }
    fclose(symtabFile);
}

void readIntermediateFile() {
    intermediateFile = fopen("intermediate.txt", "r");
    if (!intermediateFile) {
        printf("Error opening intermediate file.\n");
        exit(1);
    }
}

void writeToOutputFile(char *line) {
    fprintf(outputFile, "%s\n", line);
}

void writeToObjectFile(char *line) {
    fprintf(objectFile, "%s\n", line);
}

void processIntermediateCode() {
    char line[MAX_LEN], label[MAX_LEN], opcode[MAX_LEN], operand[MAX_LEN];
    int address;

    readSymbolTable();
    readIntermediateFile();
    
    outputFile = fopen("output.txt", "w");
    objectFile = fopen("objectcode.txt", "w");

    if (!outputFile || !objectFile) {
        printf("Error creating output or object file.\n");
        exit(1);
    }

    fscanf(intermediateFile, "%s %s %d", label, opcode, &startAddr);
    fprintf(objectFile, "H^%s^%06X^%06X\n", label, startAddr, finalAddr - startAddr);
    currentAddr = startAddr;

    while (fscanf(intermediateFile, "%d %s %s %s", &address, label, opcode, operand) != EOF) {
        currentAddr = address;
        if (strcmp(opcode, "START") == 0) {
            fprintf(outputFile, "%s\t%s\t%s\n", label, opcode, operand);
        } else if (strcmp(opcode, "END") == 0) {
            writeToObjectFile("E^00");
            break;
        } else {
            // Processing other instructions...
            writeToOutputFile("Processing...");
            generateObjectCode(opcode, operand);
        }
    }

    fclose(intermediateFile);
    fclose(outputFile);
    fclose(objectFile);
}

void generateObjectCode(char *opcode, char *operand) {
    char objectCode[MAX_LEN];
    int i, address;

    // Match opcode with mnemonic table
    for (i = 0; i < mnemonicCount; i++) {
        if (strcmp(opcode, mnemonics[i].code) == 0) {
            strcpy(objectCode, mnemonics[i].machineCode);
            break;
        }
    }

    // Check if operand is a symbol
    for (i = 0; i < symCount; i++) {
        if (strcmp(operand, symtab[i].label) == 0) {
            address = symtab[i].address;
            break;
        }
    }

    // Write object code
    sprintf(objectCode, "%s%04X", objectCode, address);
    writeToObjectFile(objectCode);
}

void displayFiles() {
    char line[MAX_LEN];
    FILE *file;

    // Display intermediate file
    file = fopen("intermediate.txt", "r");
    printf("Intermediate File Content:\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);

    // Display symbol table
    file = fopen("symtab.txt", "r");
    printf("Symbol Table:\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);

    // Display output file
    file = fopen("output.txt", "r");
    printf("Output File:\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);

    // Display object code file
    file = fopen("objectcode.txt", "r");
    printf("Object Code:\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

int main() {
    processIntermediateCode();
    displayFiles();
    return 0;
}
