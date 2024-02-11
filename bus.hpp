#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

/*
    https : // www.nesdev.org/wiki/CPU_memory_map
*/

class Mem {
public:
    virtual ~Mem() = default;
    virtual void set(uint16_t addr, uint8_t value) = 0;
    virtual uint8_t get(uint16_t addr) = 0;
};

template <size_t S>
class Ram : public Mem {
private:
    std::array<uint8_t, S> a;

public:
    virtual ~Ram() override = default;
    virtual void set(uint16_t addr, uint8_t value) override { a[addr] = value; }
    virtual uint8_t get(uint16_t addr) override { return a[addr]; }
};

class PrgRom : public Mem {
private:
    uint16_t size = 0;
    const uint8_t* data = 0;

public:
    virtual ~PrgRom() override = default;
    PrgRom(const uint8_t* data, uint16_t size)
        : data(data)
        , size(size)
    {
    }
    virtual void set(uint16_t addr, uint8_t value) override { }
    virtual uint8_t get(uint16_t addr) override { return data[addr]; }
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

    // TODO this need to be more generic
    // probably a vector of memory mapped devices
    std::shared_ptr<Mem> ram;
    std::shared_ptr<Mem> prgRom;
    std::shared_ptr<Mem> ppuMem;

    uint8_t read(uint16_t addr)
    {
        if (addr < 0x2000) {
            std::fprintf(stderr, "Reading from RAM at %04x (%02x)\n", addr, ram->get(addr & 0x07ff));
            return ram->get(addr & 0x07ff);
        }
        if (addr >= 0x2000 && addr < 0x4000) {
            std::fprintf(stderr, "Reading from PPU at %04x (%02x)\n", addr, ppuMem->get((addr - 0x2000) & 0x07));
            return ppuMem->get((addr - 0x2000) & 0x07);
        }
        if (addr >= 0x8000) {
            // std::fprintf(stderr, "Reading from CART at %04x (%02x)\n", addr, prgRom->get(addr - 0x8000));
            return prgRom->get(addr - 0x8000);
        }

        std::fprintf(stderr, "Reading from unknown at %04x\n", addr);
        exit(1);
        return 0;
    }

    void write(uint16_t addr, uint8_t value)
    {
        if (addr < 0x2000) {
            std::fprintf(stderr, "Writing to RAM at %04x (%02x)\n", addr, value);
            return ram->set(addr & 0x07ff, value);
        }
        if (addr >= 0x2000 && addr < 0x4000) {
            std::fprintf(stderr, "Writing to PPU at %04x (%02x)\n", addr, value);
            return ppuMem->set((addr - 0x2000) & 0x07, value);
        }
        std::fprintf(stderr, "Writing to unknown at %04x\n", addr);
        // exit(1);
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
