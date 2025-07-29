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
const int STORAGE_SIZE_BYTES = 64536; // 64 KB
const int MEMORY_SIZE_BYTES = 1024; // 1 KB

const char ROM_FILE_PATH[] = "storage.img";

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

        return (data[adress])           // Get the first byte  
        | (data[adress + 1] << 8)       // Get the second byte then shift to the left by 1 byte
        | (data[adress + 2] << 16)      // Get the third byte then shift to the left by 2 byte
        | (data[adress + 3] << 24);     // Get the fourth byte then shift to the left by 3 byte
    }

    void write(size_t adress, uint32_t value)
    {
        if(adress + 3 >= data.size()) 
            throw std::out_of_range("RAM write out of range");
        
        data[adress] = value & 0xFF;                // The first byte is the first value byte
        data[adress + 1] = (value >> 8) & 0xFF;     // The second byte is the second value byte
        data[adress + 2] = (value >> 16) & 0xFF;    // The third byte is the third value byte
        data[adress + 3] = (value >> 24) & 0xFF;    // The fourth byte is the fourth value byte
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
    ROM(const char* rom_file_path)
    {
        int rom_file_size;
        std::ifstream file(rom_file_path, std::ios::ate);
        if(!file) throw std::runtime_error("Failed to open file");
        if(file.is_open())
        {
            std::streamsize size = file.tellg();
            file.close();
        }

        if(rom_file_size != STORAGE_SIZE_BYTES) 
            throw std::runtime_error("Invalid ROM size");

        std::ifstream rom(rom_file_path, std::ios::ate);
        if(!rom) throw std::runtime_error("Failed to open ROM file");

        uint8_t byte;
        while(rom.read((char*)(&byte), sizeof(uint8_t))) 
        {
            data.push_back(byte);
        }
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

        ROM_PARTITION_END = rom->size();                        // ROM PARTITION
        RAM_PARTITION_END = ROM_PARTITION_END + ram->size();    // RAM PARTITION
    }

    uint32_t read(uint16_t adress) const
    {
        if(adress < ROM_PARTITION_END)                          
            return rom->read(adress);
        else if(adress < RAM_PARTITION_END)
            return ram->read(adress - ROM_PARTITION_END);   // Reading from RAM at adress offset
        else 
            throw std::out_of_range("Memory acces out of range");
    }

    void write(uint16_t adress, uint32_t value) 
    {
        if(adress < ROM_PARTITION_END)
            throw std::runtime_error("Cannot write to ROM");
        else if(adress < RAM_PARTITION_END)
            ram->write(adress - ROM_PARTITION_END, value);   // Writing to RAM memory at adress offset
        else 
            throw std::out_of_range("Memory write out of range");
    }
};

class CPU
{
private:
    uint8_t registers[REGISTER_COUNT];  // Registers
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
        this->memory = memory;      // Add memory controler
        PC = 0x00;                // Set the program counter to adress 0 
        HALTED = 0;                 // Set the halted flag to false 
        ZF = 0;                     // Set the zero flag to false
    }

    void run()
    {
        while (!HALTED)                                                 // Execute the instruction until halt
        {   
            execute(memory->read(memory->ROM_PARTITION_END + PC * 4));  // Execute the instruction at the specified memory adress

            PrintState();                                               // Debug CPU values
        }
    }
};

int main()
{
    try 
    {
        ROM* rom = new ROM(ROM_FILE_PATH);              // Initialize ROM object
        RAM* ram = new RAM(MEMORY_SIZE_BYTES);          // Initialize RAM object
        MEMC* memory_controler = new MEMC(rom, ram);    // Initialize MEMORY CONTROLER object

        CPU cpu(memory_controler);                      // Initialize CPU object

        cpu.run();                                      // Start CPU 
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "Runtime error: " << e.what() << '\n';  
    }
    catch(const std::out_of_range& e)
    {
        std::cerr << "Out of range: " << e.what() << '\n';  
    }

    return 0;
}