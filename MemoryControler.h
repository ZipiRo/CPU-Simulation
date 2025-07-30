#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>

const int STORAGE_SIZE_BYTES = 1024 * 32; // 32 KB
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

    uint32_t read(uint32_t adress) const 
    {
        if(adress + 3 >= data.size()) 
            throw std::out_of_range("RAM access out of range");

        return (data[adress])           // Get the first byte  
        | (data[adress + 1] << 8)       // Get the second byte then shift to the left by 1 byte
        | (data[adress + 2] << 16)      // Get the third byte then shift to the left by 2 byte
        | (data[adress + 3] << 24);     // Get the fourth byte then shift to the left by 3 byte
    }

    void write(uint32_t adress, uint32_t value)
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
        // Check if the file is the right size
        std::streamsize rom_file_size;
        std::ifstream file(rom_file_path, std::ios::ate);
        if(!file) throw std::runtime_error("Failed to open file");
        if(file.is_open())
        {
            rom_file_size = file.tellg();
            file.close();
        }

        if(rom_file_size != STORAGE_SIZE_BYTES) 
            throw std::runtime_error("Invalid ROM size " + std::to_string(rom_file_size) + " Bytes");

        // Load the ROM data
        std::ifstream rom(rom_file_path, std::ios::binary);
        if(!rom) throw std::runtime_error("Failed to open ROM file");

        uint8_t byte;
        while(rom.read((char*)(&byte), sizeof(uint8_t))) 
            data.push_back(byte);
    }

    uint32_t read(uint32_t adress) const 
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

    uint32_t read(uint32_t adress) const
    {
        if(adress < ROM_PARTITION_END)                          
            return rom->read(adress);
        else if(adress < RAM_PARTITION_END)
            return ram->read(adress - ROM_PARTITION_END);   // Reading from RAM at adress offset
        else 
            throw std::out_of_range("Memory acces out of range" );
    }

    void write(uint32_t adress, uint32_t value) 
    {
        if(adress < ROM_PARTITION_END)
            throw std::runtime_error("Cannot write to ROM");
        else if(adress < RAM_PARTITION_END)
            ram->write(adress - ROM_PARTITION_END, value);   // Writing to RAM memory at adress offset
        else 
            throw std::out_of_range("Memory write out of range");
    }
};
