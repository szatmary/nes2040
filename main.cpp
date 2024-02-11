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
    Rom cart;
    cart.open(argv[1]);

    Bus bus;
    auto ppu = std::make_shared<Ppu>();
    ppu->chrRom = cart.chrRomBegin();
    bus.ram = std::make_shared<Ram<2048>>();
    bus.prgRom = std::make_shared<PrgRom>(cart.prgRomBegin(), cart.prgRomSize());
    bus.ppuMem = ppu;

    Cpu cpu(bus);

    Clock clock;
    clock.addDivizor(12, [&]() {
        cpu.clk();
        bus.clk();
    });
    clock.addDivizor(4, [&]() {
        ppu->clk();
    });

    clock.run();
    return 0;
}

// reset;g++ -std=c++17 main.cpp  && ./a.out rom.nes