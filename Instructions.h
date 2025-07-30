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
#define LOADR   0x0C   // load from memory registerB adress in registerA
#define LOADI   0x0D   // load from memory immediate adress in registerA
#define STORER  0x0E   // store registerA in memory registerB adress 
#define STOREI  0x0F   // store registerA in memory immediate adress
#define ANDR    0x10   // bitwise AND operation registerA AND= registerB 
#define ANDI    0x11   // bitwise AND operarion registerA AND= immediate
#define ORR     0x12   // bitwise OR operation regiserA OR= regiseterB  
#define ORI     0X13   // bitwise OR operation regiserA OR= immediate
#define LSR     0x14   // bitwise Shift Bit to Right registerA >>= immediate  
#define LSL     0x15   // bitwise Shift Bit to Right registerA <<= immediate