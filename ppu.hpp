#pragma once
#include "bus.hpp"
#include <array>
#include <cstdint>

/*
................XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX...........
*/

class PpuBus {
public:
    int16_t address; // addres is multiplexed with data (its 14 bit I think?)
    int16_t latch;
    void dolatch() { latch = address; }
    uint8_t data() { return address & 0xff; }
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
class Ppu : public Mem {
private:
    int vblank = 1;
    int spriteZeroHit = 0;
    int spriteOverflow = 0;

public:
    virtual uint8_t get(uint16_t addr) override
    {
        // PPUCTRL   $2000  VPHB SINN   NMI enable(V), PPU master / slave(P), sprite height(H), background tile select(B), sprite tile select(S), increment mode(I), nametable select(NN)
        // PPUMASK   $2001 BGRs bMmG color emphasis(BGR), sprite enable(s), background enable(b), sprite left column enable(M), background left column enable(m), greyscale(G)
        // OAMADDR	 $2003	aaaa aaaa	OAM read/write address
        // OAMDATA	 $2004	dddd dddd	OAM data read/write
        // PPUSCROLL $2005	xxxx xxxx	fine scroll position (two writes: X scroll, Y scroll)
        // PPUADDR   $2006	aaaa aaaa	PPU read/write address (two writes: most significant byte, least significant byte)
        // PPUDATA	 $2007	dddd dddd	PPU data read/write
        // OAMDMA	 $4014	aaaa aaaa	OAM DMA high address
        switch (addr) {
        // PPUSTATUS $2002 VSO. .... vblank(V), sprite 0 hit(S), sprite overflow(O); read resets write pair for $2005/$2006
        case 2: {
            // TODO reset write pair for $2005/$2006
            auto status = vblank << 7 | spriteZeroHit << 6 | spriteOverflow << 5;
            vblank = 0; // TODO emulate latch
            return status;
        }
        case 0:
            return 0;
        default:
            return 0; // we should never get here
        }
    }

    // bool backgroundPatternTableAddress
    virtual void set(uint16_t addr, uint8_t value) override
    {
        switch (addr) {
        case 0:
            std::fprintf(stderr, "PPUCTRL %02x\n", value);
            break;
        }
    }

public:
    const uint8_t* chrRom = nullptr;

    // https://www.nesdev.org/wiki/PPU_registers
    // TODO https://www.nesdev.org/wiki/PPU_power_up_state
    std::array<uint8_t, 8> regs;
    // TODO emulate the latch

    // The PPU clock runs 3 tims faster that the CPU clock
    // The are NOT guaranteed to be in sync (CPU tick 0 can be PPU tick 0, 1 or 2)
    // The clock is triggerd
    int tick = 1;
    void clk()
    {
        // The PPU renders 262 scanlines per frame. Each scanline lasts for 341 PPU clock cycles
        // The VBlank flag of the PPU is set at tick 1 (the second tick) of scanline 241, where the VBlank NMI also occurs.
        if (tick == 0) {
            std::fprintf(stderr, "End VBLANK\n");
            vblank = false;
        } else if (!vblank && tick == 240 * 341) {
            std::fprintf(stderr, "Begin VBLANK\n");
            vblank = true;
        }

        tick = tick == 262 * 341 ? 0 : tick + 1; // set tick back to zero each frame

        // The NTSC video signal is made up of 262 scanlines, and 20 of those are spent in vblank state.
        // After the program has received an NMI, it has about 2270 cycles to update the palette, sprites,
        // and nametables as necessary before rendering begins.

        // Each scanline is made up of 341 pixels, 85 of which are horizontal blanking pixels (leaving 256 visible pixels),
        // Im a little confused because 262 - 20 = 242, and there are 240 virtical pixels, So what is the 2 extra lines?

        // https://www.nesdev.org/wiki/PPU_frame_timing
        // https://www.nesdev.org/wiki/PPU_rendering
    }

public:
};