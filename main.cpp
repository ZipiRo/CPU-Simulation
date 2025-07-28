#include <iostream>
#include <vector>

enum Operation {
    SET, // set data to a register R0 = 10
    MOV, // move/set a regsiter to another R1 = R2
    ADD, // add second register to the first register R0 += R1 
    SUB, // subtract first register from the second R0 -= R1
    CMP, // compare two registers
    JMP, // set the program counter (pc), jump to an instruction adress pc = 10 
    JZ,  // jump if the zero flag (zf) is true
    JNZ, // jump if the zero flag (zf) is false
    HLT, // stop the program
    NOP,  // no operation
    LOAD, // load some data from the memory
    STORE // store some data in the memory 
};

struct Instruction {
    int operation; // operation
    int operand1;  // register
    int operand2;  // register, number
};

const int REGISTER_COUNT = 4; // How much registers
const int MEMORY_SIZE = 256;  // How much memory in Bytes

const int CODE_START = 0;
const int CODE_END = 192;

const int DATA_START = 193;
const int DATA_END = 254;

class CPU
{
private:
    int registers[REGISTER_COUNT]; // Registers
    int memory[MEMORY_SIZE];       // Memory
    int pc = 0;                    // Program Counter

    // FLAGS
    bool halted = false; // Halt Flag 
    bool zf = false;     // Zero Flag
    
    // FUNCTIONS
    void execute(Instruction &instruction)
    {
        switch (instruction.operation)
        {
        case SET:
            registers[instruction.operand1] = instruction.operand2;                     // Set a register to a value R1 = NUM
            pc++;                                                                       // Increment the program counter
            
            std::cout << "SET R" << instruction.operand1 << " TO " << instruction.operand2 << '\n';

            break;  
        case MOV:       
            registers[instruction.operand1] = registers[instruction.operand2];          // Move/Set a register to another regsiter R0 = R1
            pc++;                                                                       // Increment the program counter
            
            std::cout << "MOV R" << instruction.operand1 << " TO R" << instruction.operand2 << '\n';

            break;      
        case ADD:       
            registers[instruction.operand1] += registers[instruction.operand2];         // Add a register to another register R1 += R0
            pc++;                                                                       // Increment the program counter
            
            std::cout << "ADD R" << instruction.operand2 << " TO R" << instruction.operand1 << '\n';

            break;      
        case SUB:       
            registers[instruction.operand1] -= registers[instruction.operand2];         // Subtract a regiter from the other register R1 -= R2
            pc++;                                                                       // Increment the program counter
            
            std::cout << "SUB R" << instruction.operand2 << " FROM R" << instruction.operand1 << '\n';

            break;
        case CMP:
            zf = (registers[instruction.operand1] == registers[instruction.operand2]);  // Compare two register R0 ?= R2 
            pc++;                                                                       // Increment the program counter
            
            std::cout << "CMP R" << instruction.operand1 << " WITH R" << instruction.operand2 << '\n';

            break;
        case JMP:
            pc = instruction.operand1;                                                  // Set the program counter to another instruction
            
            std::cout << "JMP TO " << instruction.operand1 << " INSTRUCTION\n";

            break;
        case JZ:
            std::cout << "JZ TO " << instruction.operand1 << " INSTRUCTION\n";    
            if(zf)
            {
                pc = instruction.operand1;                                              // Set the program counter to other instruction if the zero flag is true  
                std::cout << "JMP TO " << instruction.operand1 << " INSTRUCTION\n";
            }
            else pc++;    
                                 
            break;
        case JNZ:
            std::cout << "JNZ TO " << instruction.operand1 << " INSTRUCTION\n";    
            if(!zf)
            {
                pc = instruction.operand1;                                              // Set the program counter to other instruction if the zero flag is false  
                std::cout << "JMP TO " << instruction.operand1 << " INSTRUCTION\n";
            }
            else pc++;    
                                 
            break;
        case HLT:
            halted = true;                                                              // Stop the program
            
            std::cout << "PROGRAM HALTED\n";

            break;
        case LOAD:
            registers[instruction.operand1] = memory[instruction.operand2];             // Load some value from an adress in the memory to a register 
            pc++;                                                                       // Increment the program counter
            
            std::cout << "LOAD R" << instruction.operand1 << " FROM MEMORY [" << instruction.operand2 << "]\n";
            
            break;
        case STORE:
            memory[instruction.operand2] = registers[instruction.operand1];             // Store some value from a register in memory to an adress
            pc++;                                                                       // Increment the program counter
                 
            std::cout << "STORE R" << instruction.operand1 << " TO MEMORY [" << instruction.operand2 << "]\n";
            break;
        case NOP:
                                                                                        // Nothing happends
            pc++;                                                                       // Increment the program counter
            
            std::cout << "NO OPERATION\n";

            break;
        default:
            break;
        }
    }
public:
    void LoadProgram(std::vector<Instruction> program) {
        if(program.size() * 3 > CODE_END) return;                     // 1B of memory is the used memory

        for(int i = 0; i < program.size(); i++)
        {
            memory[i * 3] = program[i].operation;               // i * 3 = operation type
            memory[i * 3 + 1] = program[i].operand1;            // i * 3 + 1 = operand one
            memory[i * 3 + 2] = program[i].operand2;            // i * 3 + 2 = operand two
        }

        halted = false;                                                // Reset the halted flag 
        pc = 0;                                                        // Reset the program counter 
    }

    void Run()
    {
        while (!halted)             // Execute the instruction until halt
        {   
            Instruction instruction // Set an instruction from the memory
            { 
                memory[pc * 3],     // Operation
                memory[pc * 3 + 1], // Operand1
                memory[pc * 3 + 2]  // Operand2
            };

            execute(instruction);   // Execute the instruction
        }
    }
};

int main()
{
    CPU cpu1;
    
    std::vector<Instruction> program1(
        {
            {SET, 0, 10},   // 0
            {SET, 1, 5},    // 1
            {ADD, 0, 1},    // 2
            {SET, 1, 0},    // 3
            {ADD, 1, 0},    // 4
            {CMP, 0, 1},    // 5
            {JZ, 8},        // 6
            {SUB, 1, 0},    // 7
            {HLT, 0, 0}     // 8
        }
    );
    
    std::vector<Instruction> program2(
        {
            {SET, 0, 1},                // 0
            {SET, 1, 100},              // 1
            {MOV, 2, 0},                // 2
            {SET, 3, 5},                // 3 
            {ADD, 2, 0},                // 4
            {ADD, 3, 2},                // 5
            {CMP, 2, 1},                // 6
            {JZ, 9, 0},                 // 7
            {JMP, 4, 0},                // 8
            {STORE, 3, DATA_START},     // 9
            {HLT, 0, 0}                 // 10   
        }
    );


    cpu1.LoadProgram(program2);
    cpu1.Run();

    return 0;
}