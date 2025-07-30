#include <iostream>

#include "CPU.h"

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