#include <iostream>
#include <vector>

enum Operation {
    SET, // set data to a register R0 = 10
    MOV, // move/set a regsiter to another R1 = R2
    ADD, // add second register to the first register R0 += R1 
    SUB, // subtract first register from the second R0 -= R1
    CMP, // compare two registers
    JMP, // set the program counter (pc), jump to an instruction adress pc = 10 
    JZ,  // jump if the zero flag (zf) is set
    HLT, // stop the program
    NOP  // no operation
};

struct Instruction {
    Operation operation;
    int operand1; // register
    int operand2; // register, number, adress
};

class CPU
{
private:
    int registers[4];      // Registers
    std::vector<Instruction> memory; // Memory
    int pc = 0;      // Program Counter
    
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

            break;  

        case MOV:       
            registers[instruction.operand1] = registers[instruction.operand2];          // Move/Set a register to another regsiter R0 = R1
            pc++;                                                                       // Increment the program counter

            break;      

        case ADD:       
            registers[instruction.operand1] += registers[instruction.operand2];         // Add a register to another register R1 += R0
            pc++;                                                                       // Increment the program counter
            break;      

        case SUB:       
            registers[instruction.operand1] -= registers[instruction.operand2];         // Subtract a regiter from the other register R1 -= R2
            pc++;                                                                       // Increment the program counter
            break;
        
        case CMP:
            zf = (registers[instruction.operand1] == registers[instruction.operand2]);  // Compare two register R0 ?= R2 
            pc++;                                                                       // Increment the program counter
            break;
        
        case JMP:
            pc = instruction.operand1;                                                  // Set the program counter to another instruction
            break;
        
        case JZ:
            zf ? pc = instruction.operand1 : pc++;                                      // Set the program counter if the zero flag is set  
            break;
        
        case HLT:
            halted = true;                                                              // Stop the program
            break;

        case NOP:
                                                                                        // Nothing happends
            pc++;                                                                       // Increment the program counter
            break;
        
        default:
            break;
        }
    }
public:
    void LoadProgram(std::vector<Instruction> program) {
        memory = program;
    }

    void Run()
    {
        while (!halted)
        {
            execute(memory[pc]);
        }
    }
};

int main()
{
    CPU cpu1;
    cpu1.LoadProgram(
        {
            {SET, 0, 10},
            {SET, 1, 5},
            {ADD, 0, 1},
            {HLT, 0, 0}
        }
    );

    cpu1.Run();

    return 0;
}