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