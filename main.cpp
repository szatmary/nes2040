#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "bus.hpp"
#include "cart.hpp"
#include "clock.hpp"
#include "cpu.hpp"
#include "ppu.hpp"

// class Tile : public std::array<uint8_t, 16> {
// public:
//     Tile() { fill(0); }

//     // returns a value between 0 and 3
//     char pixel(int x, int y) const
//     {
//         // Is this correct?
//         return (at(y * 2) >> (7 - x) & 1) | (at(y * 2 + 1) >> (7 - x) & 2);
//     }
// };

// class Cpu {
// private:
// public:
//     // tickes every cycle
//     void tick();
// };

int main(int argc, char** argv)
{
    Bus bus;
    Rom cart;
    Clock clock;

    cart.open(argv[1]);
    bus.prgRom = { cart.prgRomBegin(), cart.prgRomEnd() };

    Cpu cpu(bus);
    clock.addDivizor(12, [&]() {
        cpu.clk();
        bus.clk();
    });

    clock.run();
    return 0;
}

// reset;g++ -std=c++17 main.cpp  && ./a.out rom.nes