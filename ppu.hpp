#pragma once
#include <cstdint>
#include <array>



class PpuBus {
public:
    uint16_t address; // addres is multiplexed with data
    uint8_t data() { return address&0xff; }


};

// The PPU exposes eight memory-mapped registers to the CPU.
// These nominally sit at $2000 through $2007 in the CPU's address space,
// but because their addresses are incompletely decoded, they're mirrored in every 8 bytes
// from $2008 through $3FFF. For example, a write to $3456 is the same as a write to $2006.

// The PPU has an internal data bus that it uses for communication with the CPU.
// This bus, called _io_db in Visual 2C02 and PPUGenLatch in FCEUX,[1] behaves as
// an 8-bit dynamic latch due to capacitance of very long traces that run to various
// parts of the PPU. Writing any value to any PPU port, even to the nominally read-only
// PPUSTATUS, will fill this latch. Reading any readable port (PPUSTATUS, OAMDATA, or PPUDATA)
// also fills the latch with the bits read. Reading a nominally "write-only" register returns
// the latch's current value, as do the unused bits of PPUSTATUS. This value begins to decay
// after a frame or so, faster once the PPU has warmed up, and it is likely that values with
// alternating bit patterns (such as $55 or $AA) will decay faster.[2]
class Ppu {
public:
    // https://www.nesdev.org/wiki/PPU_registers
    // TODO https://www.nesdev.org/wiki/PPU_power_up_state
    std::array<uint8_t, 8> regs;
    // TODO emulate the latch
    void write(uint16_t addr, uint8_t value)
    {
        // addr is realitve to 0x2000
        switch(addr & 0x07) {
            case 0:
                // PPUCTRL
                break;
            case 1:
                // PPUMASK
                break;
            case 2:
                // PPUSTATUS
                break;
            case 3:
                // OAMADDR
                break;
            case 4:
                // OAMDATA
                break;
            case 5:
                // PPUSCROLL
                break;
            case 6:
                // PPUADDR
                break;
            case 7:
                // PPUDATA
                break;
        }
    }


    // The PPU clock runs 3 tims faster that the CPU clock
    // The are NOT guaranteed to be in sync (CPU tick 0 can be PPU tick 0, 1 or 2)
    // The clock is triggerd

    void clk() {
        // The NTSC video signal is made up of 262 scanlines, and 20 of those are spent in vblank state.
        // After the program has received an NMI, it has about 2270 cycles to update the palette, sprites,
        // and nametables as necessary before rendering begins.

        // Each scanline is made up of 341 pixels, 85 of which are horizontal blanking pixels (leaving 256 visible pixels),
        // Im a little confused because 262 - 20 = 242, and there are 240 virtical pixels, So what is the 2 extra lines?

        // https://www.nesdev.org/wiki/PPU_frame_timing
        // https://www.nesdev.org/wiki/PPU_rendering

    }
};