#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

bool d_is_set = false;
bool w_is_set = true;
bool reg_reg = false;

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

// Idk how to do bitwise operations so bear with me.
void decodify_first_half(uint8_t* byte) {
    d_is_set = false;
    w_is_set = false;

    // TODO: CREATE A TABLE WITH ALL THE INSTRUCTIONS
    uint8_t value = *byte;
    uint8_t opcode_mask = 0b11111100;
    uint8_t dw_mask = 0b00000011;

    // Is it a mov instruction?
    if ((value & opcode_mask) == 0b10001000) {
        printf("mov ");
    } else {
        printf("ERROR: instruction not recognized!\n");
    }

    // Are either d or w set?
    if ((value & dw_mask) == dw_mask) {
        d_is_set = true;
        w_is_set = true;
    }
    else if ((value & 0b00000010) == 0b00000010) {
        d_is_set = true;
    }
    else if ((value & 0b00000001) == 0b00000001) {
        w_is_set = true;
    }
}

void reg(uint8_t value) {
    if (w_is_set) {
        switch (value) {
            case 0b00000000:
                printf("ax");
                break;
            case 0b00000001:
                printf("cx");
                break;
            case 0b00000010:
                printf("dx");
                break;
            case 0b00000011:
                printf("bx");
                break;
            case 0b00000100:
                printf("sp");
                break;
            case 0b00000101:
                printf("bp");
                break;
            case 0b00000110:
                printf("si");
                break;
            case 0b00000111:
                printf("di");
                break;
        }
    } else {
        switch (value) {
            case 0b00000000:
                printf("al");
                break;
            case 0b00000001:
                printf("cl");
                break;
            case 0b00000010:
                printf("dl");
                break;
            case 0b00000011:
                printf("bl");
                break;
            case 0b00000100:
                printf("ah");
                break;
            case 0b00000101:
                printf("ch");
                break;
            case 0b00000110:
                printf("dh");
                break;
            case 0b00000111:
                printf("bh");
                break;
        }
    }
}

void r_m(uint8_t value) {
    if (reg_reg) {
        reg(value);
    } else {
        printf("NON REG/REG OPERATIONS NOT SUPPORTED!!\n");
    }
}

void decodify_second_half(uint8_t* byte) {
    uint8_t value = *byte;

    uint8_t mod_mask = 0b11000000;
    if ((value & mod_mask) == mod_mask) {
        reg_reg = true;
    }
    uint8_t reg_mask = 0b00111000;
    uint8_t r_m_mask = 0b00000111;

    // Will this work?!?!?!?!
    uint8_t masked_reg_val = value & reg_mask;
    masked_reg_val = masked_reg_val >> 3;

    r_m(value & r_m_mask);
    printf(", ");
    reg(masked_reg_val);
    printf("\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("USAGE: sim8086 <binary path>\n");
        return -1;
    }

    Program prog = read_binary_file(argv[1]);
    
    puts("bits 16\n");
    while (prog.current_instruction != prog.final_instruction) {
        uint8_t* byte = (uint8_t*)prog.current_instruction;

        decodify_first_half(byte);
        byte++;
        decodify_second_half(byte);

        prog.current_instruction++;
    }

    // No idea how to properly free this memory :P
    uint16_t* mem = prog.final_instruction - (prog.file_size / 2);
    free(mem);

    return 0;   
}
