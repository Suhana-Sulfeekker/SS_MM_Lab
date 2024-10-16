#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100

void pass_one() {
    // Step 1: Declare variables
    char label[MAX_LEN], opcode[MAX_LEN], operand[MAX_LEN];
    char code[MAX_LEN], mnemonic[MAX_LEN];
    int locctr, start, length;
    
    FILE *input, *optab, *symtab, *intermediate, *length_file;
    
    // Step 2: Open necessary files
    input = fopen("input.txt", "r");
    optab = fopen("optab.txt", "r");
    symtab = fopen("symtab.txt", "w");
    intermediate = fopen("intermediate.txt", "w");
    length_file = fopen("length.txt", "w");

    if (!input || !optab || !symtab || !intermediate || !length_file) {
        printf("Error opening files!\n");
        exit(1);
    }

    // Step 3: Read the first line of input
    fscanf(input, "%s %s %s", label, opcode, operand);

    // Step 4: Check if the opcode is "START"
    if (strcmp(opcode, "START") == 0) {
        start = atoi(operand); // Convert operand to integer
        locctr = start;
        fprintf(intermediate, "%s\t%s\t%s\n", label, opcode, operand);
        fscanf(input, "%s %s %s", label, opcode, operand);
    } else {
        locctr = 0;
    }

    // Step 5: Iterate through the input file until "END"
    while (strcmp(opcode, "END") != 0) {
        fprintf(intermediate, "%d\t%s\t%s\t%s\n", locctr, label, opcode, operand);
        
        // If label is not empty, add it to symbol table
        if (strcmp(label, "**") != 0) {
            fprintf(symtab, "%s\t%d\n", label, locctr);
        }
        
        // Step 6: Read from optab to find matching opcode
        rewind(optab);
        fscanf(optab, "%s %s", code, mnemonic);
        int found = 0;
        
        while (!feof(optab)) {
            if (strcmp(opcode, code) == 0) {
                locctr += 3;
                found = 1;
                break;
            }
            fscanf(optab, "%s %s", code, mnemonic);
        }
        
        // Step 7: Update locctr for specific opcodes
        if (!found) {
            if (strcmp(opcode, "WORD") == 0) {
                locctr += 3;
            } else if (strcmp(opcode, "RESW") == 0) {
                locctr += 3 * atoi(operand);
            } else if (strcmp(opcode, "RESB") == 0) {
                locctr += atoi(operand);
            } else if (strcmp(opcode, "BYTE") == 0) {
                locctr += 1;
            } else {
                printf("Invalid opcode: %s\n", opcode);
            }
        }
        
        // Read the next line
        fscanf(input, "%s %s %s", label, opcode, operand);
    }
    
    // Write the last line (END) to the intermediate file
    fprintf(intermediate, "%d\t%s\t%s\t%s\n", locctr, label, opcode, operand);
    
    // Step 8: Close files
    fclose(input);
    fclose(optab);
    fclose(symtab);
    fclose(intermediate);
    
    // Step 9: Write program length
    length = locctr - start;
    fprintf(length_file, "%d\n", length);
    fclose(length_file);
    
    printf("Pass 1 complete.\n");
}

int main() {
    pass_one();
    return 0;
}
