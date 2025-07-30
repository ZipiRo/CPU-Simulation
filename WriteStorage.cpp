#include <iostream>
#include <cstdint>
#include <fstream>

#include "Instructions.h"

uint32_t EncInstr(uint32_t operation, uint32_t registerA, uint32_t registerB, uint32_t immediate = 0x0)
{
    uint32_t instruction = 
      (operation << 24)       // 8 bits -> operation
    | (registerA << 20)       // 4 bits -> registerA
    | (registerB << 16)       // 4 bits -> registerB 
    | (immediate & 0xFFFF);   // 16 bits -> immediate

    return instruction;
} 

int main()
{
    std::ofstream ROM("storage.img", std::ios::binary); 

    uint32_t bootloader[] =
    {
        // Set registers
        EncInstr(MVI, 0, 0, 0x8000),  // 0   RAM first adress
        EncInstr(MVI, 1, 0, 0x80),    // 1   ROM program adress
        EncInstr(MVI, 2, 0, 0x00),    // 2   Load register
        EncInstr(MVI, 3, 0, 0xFF),    // 3   Stop loop thing 
        
        // Load program to RAM
        EncInstr(LOADR, 2, 1),        // 4   LOAD instruction from ROM in R2
        EncInstr(STORER, 2, 0),       // 5   STORE instruction from ROM in RAM
        EncInstr(ADDI, 1, 0, 4),      // 6   Increment ROM adress
        EncInstr(ADDI, 0, 0, 4),      // 7   Increment RAM adress 
        EncInstr(LOADR, 2, 1),        // 8   Load new memory adress 
        EncInstr(ANDI, 2, 0, 0xFF),   // 9   Get just the first byte 
        EncInstr(CMP, 2, 3),          // 10  Compare the new addres value to see if the program is finished  
        EncInstr(JNZ, 0, 0, 0x10),       // 11  Loop until program is finished

        // Reset the registeres
        EncInstr(MVI, 0, 0, 0x00),    // 12  
        EncInstr(MVI, 1, 0, 0x00),    // 13 
        EncInstr(MVI, 2, 0, 0x00),    // 14
        EncInstr(MVI, 3, 0, 0x00),    // 15

        // Start program
        EncInstr(JMP, 0, 0, 0x8000)   // 16  Jump to memory program
    };

    size_t bootloader_size = sizeof(bootloader) / sizeof(bootloader[0]);

    // Bootloader Code Partition 

    for(int i = 0; i < bootloader_size; i++)
    {
        uint8_t bytes[4];
        bytes[0] = bootloader[i] & 0xFF;
        bytes[1] = (bootloader[i] >> 8) & 0xFF;
        bytes[2] = (bootloader[i] >> 16) & 0xFF;
        bytes[3] = (bootloader[i] >> 24) & 0xFF;

        ROM.write((char*)(bytes), 4);
    }

    // Padding 

    for(size_t i = bootloader_size * 4; i < 0x80; i++)
    {
        uint8_t byte = 0xFF;
        ROM.write((char*)(&byte), sizeof(uint8_t));
    }

    uint32_t program[] = 
    {
        // Set registers
        EncInstr(MVI, 0, 0, 120),     // 0 What to multiply
        EncInstr(MVI, 1, 0, 5),       // 1 How much times to multiply 
        EncInstr(MVI, 2, 0, 0),       // 2 How much times we multiplyed
        EncInstr(MVI, 3, 0, 0),       // 3 The result

        // Multiplying loop 
        EncInstr(ADDI, 2, 0, 1),      // 4 Add to the times multiplyed
        EncInstr(ADDR, 3, 0),         // 5 Add the result 
        EncInstr(CMP, 1, 2),          // 6 Did we multiply R1 times 
        EncInstr(JNZ, 0, 0, 0x8010),  // 8 Jump to start of loop if not 

        // Stop program
        EncInstr(HLT, 0, 0)           // 9
    };

    size_t program_size = sizeof(program) / sizeof(program[0]);

    // Program Code Partition

    for(size_t i = 0; i < program_size; i++)
    {
        uint8_t bytes[4];
        bytes[0] = program[i] & 0xFF;
        bytes[1] = (program[i] >> 8) & 0xFF;
        bytes[2] = (program[i] >> 16) & 0xFF;
        bytes[3] = (program[i] >> 24) & 0xFF;

        ROM.write((char*)(bytes), 4);
    }

    // Padding

    for(size_t i = 0x80 + (program_size * 4); i < 0x8000; i++)
    {
        uint8_t byte = 0xFF;
        ROM.write((char*)(&byte), sizeof(uint8_t));
    }

    return 0;
}