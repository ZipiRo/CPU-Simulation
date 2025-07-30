#include <iostream>
#include <cstdint>

#include "MemoryControler.h"
#include "Instructions.h"

const int REGISTER_COUNT = 4;

class CPU
{
private:
    uint32_t registers[REGISTER_COUNT]; // Registers
    MEMC* memory;                       // Memory Controler

    uint32_t PC;                        // Program Counter / Memory Address Pointer

    // FLAGS
    bool HALTED;                        // Halt Flag 
    bool ZF;                            // Zero Flag
    
    // FUNCTIONS
    void execute(uint32_t instruction)
    {
        uint8_t operation = (instruction >> 24) & 0xFF; // shift 24 bits to the right then keep the last 8 bits
        uint8_t registerA = (instruction >> 20) & 0xF;  // shift 20 bits to the right then keep the last 4 bits
        uint8_t registerB = (instruction >> 16) & 0xF;  // shift 16 bits to the right then keep the last 4 bits
        uint16_t immediate = instruction & 0xFFFF;      // keep the last 16 bits

        switch (operation)
        {
        case MVR:       
            registers[registerA] = registers[registerB];                        // Move/Set a register to another regsiter RA = RB
            PC += 4;                                                            // Move the program counter to the next instruction

            std::cout << "MVR -> R" << int(registerA) << "[" << registers[registerA] << "] = R" << int(registerB) << "[" << registers[registerB] << "]\n";  

            break;      
        case ADDR:       
            registers[registerA] += registers[registerB];                       // Add a register to another register RA += RB
            PC += 4;                                                            // Move the program counter to the next instruction

            std::cout << "ADDR -> R" << int(registerA) << "[" << registers[registerA] << "] += R" << int(registerB) << "[" << registers[registerB] << "]\n";  

            break;      
        case SUBR:       
            registers[registerA] -= registers[registerB];                       // Subtract a regiter from the other register RA -= RB
            PC += 4;                                                            // Move the program counter to the next instruction

            std::cout << "SUBR -> R" << int(registerA) << "[" << registers[registerA] << "] -= R" << int(registerB) << "[" << registers[registerB] << "]\n";  

            break;
        case MVI:       
            registers[registerA] = immediate;                                   // Set a register to an immediate RA = imm
            PC += 4;                                                            // Move the program counter to the next instruction

            std::cout << "MVI -> R" << int(registerA) << "[" << registers[registerA] << "] = I[" << immediate << "]\n";  

            break;      
        case ADDI:       
            registers[registerA] += immediate;                                  // Add an immediate to a register RB += imm
            PC += 4;                                                            // Move the program counter to the next instruction

            std::cout << "ADDI -> R" << int(registerA) << "[" << registers[registerA] << "] += I[" << immediate << "]\n";  

            break;      
        case SUBI:       
            registers[registerA] -= immediate;                                  // Subtract a regiter from the other register RA -= imm
            PC += 4;                                                            // Move the program counter to the next instruction

            std::cout << "SUBI -> R" << int(registerA) << "[" << registers[registerA] << "] -= I[" << immediate << "]\n";  

            break;
        case CMP:
            ZF = (registers[registerA] == registers[registerB]);                // Compare two register RA ?= RB 
            PC += 4;                                                            // Move the program counter to the next instruction

            std::cout << "CMP -> R" << int(registerA) << "[" << registers[registerA] << "] ?= I" << int(registerB) << "[" << registers[registerB] << "]\n";  

            break;
        case JMP:
            PC = immediate;                                                     // Set the program counter to another instruction

            std::cout << "JMP -> PC = I[" << immediate << "]\n"; 

            break;
        case JZ:
            if(ZF) PC = immediate;                                              // Set the program counter to other instruction if the zero flag is true  
            else PC += 4;    
                                 
            std::cout << "JZ -> PC = I[" << immediate << "]\n";

            break;
        case JNZ:
            if(!ZF) PC = immediate;                                             // Set the program counter to other instruction if the zero flag is false  
            else PC += 4;    

            std::cout << "JNZ -> PC = I[" << immediate << "]\n";
                                 
            break;
        case HLT:
            HALTED = true;                                                      // Stop the program

            std::cout << "Program halted\n";

            break;
        case LOADR:
            registers[registerA] = memory->read(registers[registerB]);          // Load some value from an adress in the memory to a register 
            PC += 4;                                                            // Move the program counter to the next instruction
            
            std::cout << "LOADR -> R" << int(registerA) << " = Memory(R" << int(registerB) << "[" << registers[registerB] << "])\n";  

            break;
        case STORER:
            memory->write(registers[registerB], registers[registerA]);          // Store some value from a register in memory to an adress
            PC += 4;                                                            // Move the program counter to the next instruction
            
            std::cout << "STORER -> Memory(R" << int(registerB) << "[" << registers[registerB] << "])" << " = R" << int(registerA) << "[" << registers[registerA] << "]\n";                                                                              

            break;
        case LOADI:
            registers[registerA] = memory->read(immediate);                     
            PC += 4;                                                            // Move the program counter to the next instruction
            
            std::cout << "LOADI -> R" << int(registerA) << " = Memory(I[" << immediate << "])\n";  

            break;
        case STOREI:
            memory->write(immediate, registers[registerA]);                     
            PC += 4;                                                            // Move the program counter to the next instruction
                                   
            std::cout << "STOREI -> Memory(I[" << immediate << "])" << " = R" << int(registerA) << "[" << registers[registerA] << "]\n";     

            break;
        case ANDR:
            registers[registerA] &= registers[registerB];
            PC += 4;                                                            // Move the program counter to the next instruction
                            
            std::cout << "ANDR -> R" << int(registerA) << "[" << registers[registerA] << "] &= R" << int(registerB) << "[" << registers[registerB] << "]\n";           
            
            break;
        case ANDI:
            registers[registerA] &= immediate;             
            PC += 4;                                                            // Move the program counter to the next instruction
                                                                                 
            std::cout << "ANDR -> R" << int(registerA) << "[" << registers[registerA] << "] &= I[" << immediate << "]\n";   

            break;
        case ORR:
            registers[registerA] |= registers[registerB];
            PC += 4;                                                            // Move the program counter to the next instruction
                                                                   
            std::cout << "ANDR -> R" << int(registerA) << "[" << registers[registerA] << "] |= R" << int(registerB) << "[" << registers[registerB] << "]\n";   

            break;
        case ORI:
            registers[registerA] |= immediate;                                  
            PC += 4;                                                            // Move the program counter to the next instruction
                                                   
            std::cout << "ANDR -> R" << int(registerA) << "[" << registers[registerA] << "] |= I[" << immediate << "]\n";   

            break;
        case LSR:
            registers[registerA] >>= immediate;                                 // Shift immediate bits to the right
            PC += 4;                                                            // Move the program counter to the next instruction
                                                                         
            std::cout << "ANDR -> R" << int(registerA) << "[" << registers[registerA] << "] >>= I[" << immediate << "]\n";   

            break;
        case LSL:
            registers[registerA] <<= immediate;                                 // Shift immediate bits to the left
            PC += 4;                                                            // Move the program counter to the next instruction

            std::cout << "ANDR -> R" << int(registerA) << "[" << registers[registerA] << "] <<= I[" << immediate << "]\n";   
                                                                                        
            break;
        case NOP:
            PC += 4;                                                             // Move the program counter to the next instruction

            break;
        default:
            break;
        }
    }

    void PrintState() 
    {
        std::cout << "PC: " << PC << '\n';
        std::cout << "Registers:\n";
        for(int i = 0; i < REGISTER_COUNT; ++i)
            std::cout << "R" << i << " = " << int(registers[i]) << '\n';
        std::cout << '\n';
    }

public:
    CPU(MEMC* memory)
    {
        this->memory = memory;      // Add memory controler
        PC = 0x0;                   // Set the program counter to adress 0 
        HALTED = 0;                 // Set the halted flag to false 
        ZF = 0;                     // Set the zero flag to false
    }

    void run()
    {
        while (!HALTED)                     // Execute the instruction until halt
        {   
            execute(memory->read(PC));      // Execute the instruction at the specified memory adress

            PrintState();                   // Debug CPU values
        }
    }
};