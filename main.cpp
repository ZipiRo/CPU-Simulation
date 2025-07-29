#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>

// INSTRUCTION SET
#define MVR     0x00   // set a regsiter to another
#define MVI     0x01   // set a register to an immediate
#define ADDR    0x02   // add registerB to the registerB
#define ADDI    0x03   // add to registerA an immediate
#define SUBR    0x04   // subtract registerB from the secondA
#define SUBI    0x05   // subtract an immediate from registerA
#define CMP     0x06   // compare two registers
#define JMP     0x07   // set the program counter to jump to an instruction adress 
#define JZ      0x08   // jump if the zero flag is true
#define JNZ     0x09   // jump if the zero flag is false
#define HLT     0x0A   // stop the program
#define NOP     0x0B   // no operation
#define LOAD    0x0C   // load from memory adress in registerA
#define STORE   0x0D   // store registerA in memory adress 
// INSTRUCTION SET

const int REGISTER_COUNT = 4;
const int MEMORY_SIZE_BYTES = 1024; // 1 KB

class RAM
{
private:
    std::vector<uint8_t> data;

public:
    RAM(size_t size_in_bytes) 
    {
        data = std::vector<uint8_t>(size_in_bytes);
    }

    uint32_t read(size_t adress) const 
    {
        if(adress + 3 >= data.size()) 
            throw std::out_of_range("RAM access out of range");

        return (data[adress])
        | (data[adress + 1] << 8)
        | (data[adress + 2] << 16)
        | (data[adress + 3] << 24);
    }

    void write(size_t adress, uint32_t value)
    {
        if(adress + 3 >= data.size()) 
            throw std::out_of_range("RAM write out of range");
        
        data[adress] = value & 0xFF;
        data[adress + 1] = (value >> 8) & 0xFF;
        data[adress + 2] = (value >> 16) & 0xFF;
        data[adress + 3] = (value >> 24) & 0xFF;
    }

    size_t size()
    {
        return data.size();
    }
};

class ROM
{
private:
    std::vector<uint8_t> data;

public:
    ROM() {}
    ROM(char* romfile)
    {
        std::ifstream rom(romfile, std::ios::binary);
        if(!rom) throw std::runtime_error("Failed to open ROM file");

        uint8_t byte;
        while(rom.read((char*)(&byte), sizeof(uint8_t))) 
            data.push_back(byte);
    }

    uint32_t read(size_t adress) const 
    {
        if(adress >= data.size()) throw std::out_of_range("ROM access out of range");

        return (data[adress])
        | (data[adress + 1] << 8)
        | (data[adress + 2] << 16)
        | (data[adress + 3] << 24);
    }

    size_t size()
    {
        return data.size();
    }
};

class MEMC
{
private:
    ROM* rom;
    RAM* ram;

public:
    uint32_t ROM_PARTITION_END;
    uint32_t RAM_PARTITION_END;

    MEMC() {}
    MEMC(ROM* rom, RAM* ram)
    {
        this->rom = rom;
        this->ram = ram; 

        ROM_PARTITION_END = rom->size();
        RAM_PARTITION_END = ROM_PARTITION_END + ram->size();
    }

    uint32_t operator[](uint16_t adress) const
    {
        return read(adress);
    }

    uint32_t read(uint16_t adress) const
    {
        if(adress < ROM_PARTITION_END)
            return rom->read(adress);
        else if(adress < RAM_PARTITION_END)
            return ram->read(adress - ROM_PARTITION_END);
        else 
            throw std::out_of_range("Memory acces out of range");
    }

    void write(uint16_t adress, uint32_t value) 
    {
        if(adress < ROM_PARTITION_END)
            throw std::runtime_error("Cannot write to ROM");
        else if(adress < RAM_PARTITION_END)
            ram->write(adress - ROM_PARTITION_END, value);
        else 
            throw std::out_of_range("Memory write out of range");
    }
};

class CPU
{
private:
    uint8_t registers[REGISTER_COUNT];  // Registers
    MEMC* memory;                       // Memory

    uint32_t PC = 0;                    // Program Counter

    // FLAGS
    bool HALTED = false;                // Halt Flag 
    bool ZF = false;                    // Zero Flag
    
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
            registers[registerA] = registers[registerB];             // Move/Set a register to another regsiter RA = RB
            PC++;                                                    // Increment the program counter

            break;      
        case ADDR:       
            registers[registerA] += registers[registerB];            // Add a register to another register RA += RB
            PC++;                                                    // Increment the program counter

            break;      
        case SUBR:       
            registers[registerA] -= registers[registerB];            // Subtract a regiter from the other register RA -= RB
            PC++;                                                    // Increment the program counter

            break;
        case MVI:       
            registers[registerA] = immediate;                        // Set a register to an immediate RA = imm
            PC++;                                                    // Increment the program counter

            break;      
        case ADDI:       
            registers[registerA] += immediate;                       // Add an immediate to a register RB += imm
            PC++;                                                    // Increment the program counter

            break;      
        case SUBI:       
            registers[registerA] -= immediate;                       // Subtract a regiter from the other register RA -= imm
            PC++;                                                    // Increment the program counter

            break;
        case CMP:
            ZF = (registers[registerA] == registers[registerB]);     // Compare two register RA =? RB 
            PC++;                                                    // Increment the program counter

            break;
        case JMP:
            PC = immediate;                                          // Set the program counter to another instruction

            break;
        case JZ:
            if(ZF) PC = immediate;                                   // Set the program counter to other instruction if the zero flag is true  
            else PC++;    
                                 
            break;
        case JNZ:
            if(!ZF) PC = immediate;                                  // Set the program counter to other instruction if the zero flag is false  
            else PC++;    
                                 
            break;
        case HLT:
            HALTED = true;                                           // Stop the program

            break;
        case LOAD:
            registers[registerA] = memory->read(immediate);           // Load some value from an adress in the memory to a register 
            PC++;                                                    // Increment the program counter
            
            break;
        case STORE:
            memory->write(immediate, registers[registerA]);           // Store some value from a register in memory to an adress
            PC++;                                                    // Increment the program counter
                                                                                        
            break;
        case NOP:
            PC++;                                                    // Increment the program counter

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
        this->memory = memory;
    }

    // void LoadProgram(uint32_t* program, int program_size) 
    // {
    //     if(CODE_END - CODE_START < program_size)                // The program should fit in the code partition
    //     {
    //         std::cout << "Program is too big!";
    //         return;
    //     }

    //     for(int i = 0; i < program_size; i++)
    //         memory[CODE_START + i] = program[i];                // Copy the program to memory

    //     HALTED = false;                                         // Reset the HALTED flag 
    //     PC = 0;                                                 // Reset the program counter 
    // }

    void run()
    {
        while (!HALTED)                                             // Execute the instruction until halt
        {   
            execute(memory->read(memory->ROM_PARTITION_END + PC * 4));  // Execute the instruction at the specified memory adress

            PrintState();                                           // Debug CPU values
        }
    }
};

uint32_t EncInstr(uint32_t operation, uint32_t registerA, uint32_t registerB, uint32_t immediate = 0x0)
{
    uint32_t instruction = 
      (operation << 24)       // 8 bits -> operation
    | (registerA << 20)       // 4 bits -> registerA
    | (registerB << 16)       // 4 bits -> registerB 
    | (immediate & 0xFFFF);   // 16 bits -> immediate

    return instruction;
} 

// TO DO: bootloader

int main()
{
    ROM* rom = new ROM("storage.img");
    RAM* ram = new RAM(MEMORY_SIZE_BYTES);
    MEMC* memory_controler = new MEMC(rom, ram);

    CPU cpu(memory_controler);

    // uint32_t program1[] =
    // {
    //     EncInstr(MVI, 0, 0, 10),        // 0
    //     EncInstr(MVI, 1, 0, 5),         // 1
    //     EncInstr(ADDR, 0, 1),           // 2
    //     EncInstr(MVI, 1, 0, 0),         // 3
    //     EncInstr(ADDR, 1, 0),           // 4
    //     EncInstr(CMP, 0, 1),            // 5  
    //     EncInstr(JZ, 8, 0),             // 6  
    //     EncInstr(SUBR, 1, 0),           // 7  
    //     EncInstr(HLT, 0, 0)             // 8  
    // };

    // uint32_t program3[] = 
    // {
    //     EncInstr(MVI, 0, 0, 1),                 // 0
    //     EncInstr(MVI, 1, 0, 100),               // 1
    //     EncInstr(MVI, 3, 0, 5),                 // 2 
        
    //     EncInstr(ADDI, 0, 0, 1),                // 3
    //     EncInstr(ADDI, 3, 0, 5),                // 4
    //     EncInstr(CMP, 0, 1),                    // 5
    //     EncInstr(JZ, 0, 0, 8),                  // 6
    //     EncInstr(JMP, 0, 0, 3),                 // 7

    //     EncInstr(STORE, 3, 0, DATA_START),      // 8
    //     EncInstr(HLT, 0, 0)                     // 9
    // };

    // uint32_t program2[] =
    // {
    //     EncInstr(MVI, 0, 0, 100),   // 0
    //     EncInstr(MVI, 1, 0, 5),     // 1

    //     EncInstr(SUBI, 0, 0, 1),    // 2
    //     EncInstr(CMP, 0, 1),        // 3
    //     EncInstr(JZ, 0, 0, 6),      // 4
    //     EncInstr(JMP, 0, 0, 2),     // 5

    //     EncInstr(HLT, 0, 0)         // 6
    // };

    // uint32_t bootloader[] =
    // {
    //     EncInstr(MVI, 0, 0, 0),     // 0
    //     EncInstr(MVI, 1, 0, 0),     // 1 
    //     EncInstr(MVI, 2, 0, 0),     // 2
    //     EncInstr(MVI, 3, 0, 0),     // 3
        
        
    // };

    // int program_size = sizeof(program2) / sizeof(program2[0]);

    // cpu.LoadProgram(program2, program_size);
    cpu.run();

    return 0;
}