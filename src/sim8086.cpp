#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct Program {
    uint16_t* current_instruction;
    uint16_t* final_instruction;
    size_t file_size;
};

Program read_binary_file(const char* filename) {
    Program prog = {0};

    // Read file in binary mode.
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Could not open file: %s\n", filename);
        return prog;
    }

    // Get the size of the file.
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);
    prog.file_size = file_size;

    // Dump contents of the file inside a buffer.
    void* buffer = malloc(file_size);
    if (buffer == NULL) {
        printf("Error allocating memory.\n");
        fclose(file);
        return prog;
    }
    fread(buffer, 1, file_size, file);
    fclose(file);

    prog.current_instruction = (uint16_t*)buffer;
    prog.final_instruction = prog.current_instruction + (file_size / 2);

    return prog;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("USAGE: sim8086 <binary path>\n");
        return -1;
    }

    Program prog = read_binary_file(argv[1]);
    
    int i = 1;
    while (prog.current_instruction != prog.final_instruction) {
        printf("%d: %d\n", i, *prog.current_instruction);
        prog.current_instruction++;
        i++;
    }

    // No idea how to properly free this memory :P
    uint16_t* mem = prog.final_instruction - (prog.file_size / 2);
    free(mem);

    return 0;   
}
