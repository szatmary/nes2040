#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

/*
    https : // www.nesdev.org/wiki/CPU_memory_map
    0x8000 - 0xffff - 32kB
*/

struct Mem {
    // Pointers to the underlying memory
    uint8_t* beg = 0;
    uint8_t* end = 0;

    // Offset into the memory map
    uint16_t oset = 0;
    uint16_t size = 0;
};

// https://www.nesdev.org/wiki/CPU_memory_map
class Bus {
private:
public:
    static const bool READ = 1;
    static const bool WRITE = 0;

    uint16_t addr = 0;
    uint8_t data = 0;
    bool rw = READ;

    std::array<uint8_t, 2048> ram;
    std::pair<uint8_t*, uint8_t*> prgRom;
    // std::pair<uint8_t*, uint8_t*> ppu;

    // TODO this need to be more generic!
    uint8_t read(uint16_t addr)
    {
        if (addr < 0x2000) {
            std::fprintf(stderr, "Reading from RAM at %04x (%02x)\n", addr, ram[addr & 0x07ff]);
            return ram[addr & 0x07ff];
        }
        // if (addr >= 0x2000 && addr < 0x4000) {
        //     std::fprintf(stderr, "Reading from PPU at %04x (%02x)\n", addr, ppu.first[addr - 0x2000]);
        //     return ppu.first[(addr - 0x2000) & 0x07];
        // }
        if (addr >= 0x8000) {
            std::fprintf(stderr, "Reading from CART at %04x (%02x)\n", addr, prgRom.first[addr - 0x8000]);
            return prgRom.first[addr - 0x8000];
        }

        std::fprintf(stderr, "Reading from unknown at %04x\n", addr);
        return 0;
    }

    void write(uint16_t addr, uint8_t value)
    {
        if (addr < 0x2000) {
            ram[addr & 0x07ff] = value;
        }
        // if (addr >= 0x2000 && addr < 0x4000) {
        //     ppu.first[(addr - 0x2000) & 0x07] = value;
        // }
    }

    void clk()
    {
        if (rw == READ) {
            data = read(addr);
        } else {
            write(addr, data);
        }
    }
};

// template <int P>
// struct Mem : public std::array<uint8_t, 256 * P> {
//     constexpr int pages() { return P; }

//     virtual uint8_t read(uint16_t address) = 0;
//     virtual void write(uint16_t address, uint8_t value) = 0;
//     virtual size_t size() = 0;
// };

// template <int P>
// struct Ram : public Mem<P> {
//     Ram(int pages)
//     {
//         data.resize(pages * 256);
//     }

//     uint8_t read(uint16_t address) override
//     {

//         return data[address];
//     }

//     void write(uint16_t address, uint8_t value) override
//     {
//         data_[address] = value;
//     }

//     size_t size() override
//     {
//         return size_;
//     }
// }

// struct {
//     uint16_t size;
//     std::shared_ptr<Mem> mem;
// };

/*
    $0000-$00FF Zero Page
    $0100-$01FF Stack
    $0200-$07FF RAM
    $0800–$07FF

    0000 0000 0000 0000 - 0000 0111 1111 1111 // mask: 0xf800 == 0x0000 // 2kB internal RAM

    if (addr & 0xf000 == 0x0000) {
        arrr ~= 0x0800;
        return ram[addr & 0x07ff];
    }


   // 2kB internal RAM
    0000 0000 0000 0000 - 0000 0000 1111 1111 // mask: addr & 0xfff0 == 0x0000 // zero page
    0000 0001 0000 0000 - 0000 0001 1111 1111 // mask: addr & 0xffe0 == 0x0000 // stack
    0000 0010 0000 0000 - 0000 0011 1111 1111 // mask: addr & 0xfd00 == 0x0000 // ram
    0000 0010 0000 0000 - 0000 0011 1111 1111 // mask: addr & 0xfd00 == 0x0000 // ram




    $1000-$17FF
    0000 1000 0000 0000 - 0000 1111 1111 1111 // mask: addr & 0x8000 == 0x8000
*/
