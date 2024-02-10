#include <array>
#include <functional>
#include <iostream>
#include <queue>

#include "bus.hpp"

class Cpu {
public:
    // https://www.nesdev.org/2A03%20technical%20reference.txt
    //           ___  ___
    //          |*  \/   |
    // ROUT <01]        [40<  VCC
    // COUT <02]        [39>  $4016W.0
    // /RES >03]        [38>  $4016W.1
    // A0   <04]        [37>  $4016W.2
    // A1   <05]        [36>  /$4016R
    // A2   <06]        [35>  /$4017R
    // A3   <07]        [34>  R/W
    // A4   <08]        [33<  /NMI
    // A5   <09]        [32<  /IRQ
    // A6   <10]  2A03  [31>  PHI2
    // A7   <11]        [30<  ---
    // A8   <12]        [29<  CLK
    // A9   <13]        [28]  D0
    // A10  <14]        [27]  D1
    // A11  <15]        [26]  D2
    // A12  <16]        [25]  D3
    // A13  <17]        [24]  D4
    // A14  <18]        [23]  D5
    // A15  <19]        [22]  D6
    // VEE  >20]        [21]  D7
    //          |________|

    Bus& bus;

    // registers
    uint16_t ProgramCounter = 0x8000; // 0xfffc;
    uint8_t Accumulator = 0;
    uint8_t Xregister = 0;
    uint8_t Yregister = 0;
    uint8_t StackPointer = 0;
    uint8_t Status = 0;
    enum {
        Carry = 0x01,
        Zero = 0x02,
        InterruptDisable = 0x04,
        DecimalMode = 0x08,
        Break = 0x10,
        // 0x20 is unused
        Overflow = 0x40,
        Negative = 0x80,
    };
    bool getFlag(uint8_t flag) { return !!(Status & flag); }
    void setFlag(uint8_t flag, bool value) { Status = value ? Status | flag : Status & ~flag; }

    // void setFlags(uint16_t value)
    // {
    //     setFlag(Negative, value & 0x80);
    //     setFlag(Zero, (value & 0xff) == 0);
    //     setFlag(Carry, value > 255);
    //     // setFlaf(Overflow );// TODO
    // }

    const uint16_t stackBase = 0x0100;

public:
    // Microcode
    // This queue is used to store the microcode that will be executed
    // by the CPU. The microcode is a sequence of functions that will be
    // executed one per cycle to perform the instruction. Each microcode
    // instructiontakes exactly 1 cycle to execute.
    // TODO use faster primitives than std::function and std::queue
    using inst = std::function<void()>;
    std::queue<inst> microcode;

    // The ISA is implemented as a sequence of functions that
    // expand the microcode queue with the steps for each
    // instruction. The instruction fetch and decode
    // consumes 1 cycle, so the number microcode instructions
    // added to the queue is the number of cycles the
    // instruction takes execute minus 1.

    // https://www.masswerk.at/6502/6502_instruction_set.html
    std::array<std::function<void()>, 256> isa = {
        /* 00 */ [&] { std::fprintf(stderr, "Instruction 00 not implemented\n"); printState(); exit(1); },
        /* 01 */ [&] { std::fprintf(stderr, "Instruction 01 not implemented\n"); printState(); exit(1); },
        /* 02 */ [&] { std::fprintf(stderr, "Instruction 02 not implemented\n"); printState(); exit(1); },
        /* 03 */ [&] { std::fprintf(stderr, "Instruction 03 not implemented\n"); printState(); exit(1); },
        /* 04 */ [&] { std::fprintf(stderr, "Instruction 04 not implemented\n"); printState(); exit(1); },
        /* 05 */ [&] { std::fprintf(stderr, "Instruction 05 not implemented\n"); printState(); exit(1); },
        /* 06 */ [&] { std::fprintf(stderr, "Instruction 06 not implemented\n"); printState(); exit(1); },
        /* 07 */ [&] { std::fprintf(stderr, "Instruction 07 not implemented\n"); printState(); exit(1); },
        /* 08 */ [&] { std::fprintf(stderr, "Instruction 08 not implemented\n"); printState(); exit(1); },
        /* 09 */ [&] { std::fprintf(stderr, "Instruction 09 not implemented\n"); printState(); exit(1); },
        /* 0a */ [&] { std::fprintf(stderr, "Instruction 0a not implemented\n"); printState(); exit(1); },
        /* 0b */ [&] { std::fprintf(stderr, "Instruction 0b not implemented\n"); printState(); exit(1); },
        /* 0c */ [&] { std::fprintf(stderr, "Instruction 0c not implemented\n"); printState(); exit(1); },
        /* 0d */ [&] { std::fprintf(stderr, "Instruction 0d not implemented\n"); printState(); exit(1); },
        /* 0e */ [&] { std::fprintf(stderr, "Instruction 0e not implemented\n"); printState(); exit(1); },
        /* 0f */ [&] { std::fprintf(stderr, "Instruction 0f not implemented\n"); printState(); exit(1); },

        /* 10 */ [&] {
            // BPL - Branch on PLus
            // branch on N = 0
            // N	Z	C	I	D	V
            // -	-	-	-	-	-
            // addressing	assembler	opc	bytes	cycles
            // relative	BPL oper	10	2	    2**
            // ** add 1 to cycles if branch occurs on same page
            // ** add 2 to cycles if branch occurs to different page

            static int8_t oper = 0;
            bus.addr = ProgramCounter++;
            microcode.emplace([&] { oper = bus.data;});
            if (!getFlag(Negative)) {
                // if (bus.addr & 0xff00 != ProgramCounter & 0xff00) { // TODO this is almost certainly wrong!
                //     microcode.emplace([] {}); // add 2 to cycles if branch occurs to different page
                // }
                microcode.emplace([&] { ProgramCounter += oper; });
            } },
        /* 11 */ [&] { std::fprintf(stderr, "Instruction 11 not implemented\n"); printState(); exit(1); },
        /* 12 */ [&] { std::fprintf(stderr, "Instruction 12 not implemented\n"); printState(); exit(1); },
        /* 13 */ [&] { std::fprintf(stderr, "Instruction 13 not implemented\n"); printState(); exit(1); },
        /* 14 */ [&] { std::fprintf(stderr, "Instruction 14 not implemented\n"); printState(); exit(1); },
        /* 15 */ [&] { std::fprintf(stderr, "Instruction 15 not implemented\n"); printState(); exit(1); },
        /* 16 */ [&] { std::fprintf(stderr, "Instruction 16 not implemented\n"); printState(); exit(1); },
        /* 17 */ [&] { std::fprintf(stderr, "Instruction 17 not implemented\n"); printState(); exit(1); },
        /* 18 */ [&] { std::fprintf(stderr, "Instruction 18 not implemented\n"); printState(); exit(1); },
        /* 19 */ [&] { std::fprintf(stderr, "Instruction 19 not implemented\n"); printState(); exit(1); },
        /* 1a */ [&] { std::fprintf(stderr, "Instruction 1a not implemented\n"); printState(); exit(1); },
        /* 1b */ [&] { std::fprintf(stderr, "Instruction 1b not implemented\n"); printState(); exit(1); },
        /* 1c */ [&] { std::fprintf(stderr, "Instruction 1c not implemented\n"); printState(); exit(1); },
        /* 1d */ [&] { std::fprintf(stderr, "Instruction 1d not implemented\n"); printState(); exit(1); },
        /* 1e */ [&] { std::fprintf(stderr, "Instruction 1e not implemented\n"); printState(); exit(1); },
        /* 1f */ [&] { std::fprintf(stderr, "Instruction 1f not implemented\n"); printState(); exit(1); },

        /* 20 */ [&] { std::fprintf(stderr, "Instruction 20 not implemented\n"); printState(); exit(1); },
        /* 21 */ [&] { std::fprintf(stderr, "Instruction 21 not implemented\n"); printState(); exit(1); },
        /* 22 */ [&] { std::fprintf(stderr, "Instruction 22 not implemented\n"); printState(); exit(1); },
        /* 23 */ [&] { std::fprintf(stderr, "Instruction 23 not implemented\n"); printState(); exit(1); },
        /* 24 */ [&] { std::fprintf(stderr, "Instruction 24 not implemented\n"); printState(); exit(1); },
        /* 25 */ [&] { std::fprintf(stderr, "Instruction 25 not implemented\n"); printState(); exit(1); },
        /* 26 */ [&] { std::fprintf(stderr, "Instruction 26 not implemented\n"); printState(); exit(1); },
        /* 27 */ [&] { std::fprintf(stderr, "Instruction 27 not implemented\n"); printState(); exit(1); },
        /* 28 */ [&] { std::fprintf(stderr, "Instruction 28 not implemented\n"); printState(); exit(1); },
        /* 29 */ [&] { std::fprintf(stderr, "Instruction 29 not implemented\n"); printState(); exit(1); },
        /* 2a */ [&] { std::fprintf(stderr, "Instruction 2a not implemented\n"); printState(); exit(1); },
        /* 2b */ [&] { std::fprintf(stderr, "Instruction 2b not implemented\n"); printState(); exit(1); },
        /* 2c */ [&] { std::fprintf(stderr, "Instruction 2c not implemented\n"); printState(); exit(1); },
        /* 2d */ [&] { std::fprintf(stderr, "Instruction 2d not implemented\n"); printState(); exit(1); },
        /* 2e */ [&] { std::fprintf(stderr, "Instruction 2e not implemented\n"); printState(); exit(1); },
        /* 2f */ [&] { std::fprintf(stderr, "Instruction 2f not implemented\n"); printState(); exit(1); },

        /* 30 */ [&] { std::fprintf(stderr, "Instruction 30 not implemented\n"); printState(); exit(1); },
        /* 31 */ [&] { std::fprintf(stderr, "Instruction 31 not implemented\n"); printState(); exit(1); },
        /* 32 */ [&] { std::fprintf(stderr, "Instruction 32 not implemented\n"); printState(); exit(1); },
        /* 33 */ [&] { std::fprintf(stderr, "Instruction 33 not implemented\n"); printState(); exit(1); },
        /* 34 */ [&] { std::fprintf(stderr, "Instruction 34 not implemented\n"); printState(); exit(1); },
        /* 35 */ [&] { std::fprintf(stderr, "Instruction 35 not implemented\n"); printState(); exit(1); },
        /* 36 */ [&] { std::fprintf(stderr, "Instruction 36 not implemented\n"); printState(); exit(1); },
        /* 37 */ [&] { std::fprintf(stderr, "Instruction 37 not implemented\n"); printState(); exit(1); },
        /* 38 */ [&] { std::fprintf(stderr, "Instruction 38 not implemented\n"); printState(); exit(1); },
        /* 39 */ [&] { std::fprintf(stderr, "Instruction 39 not implemented\n"); printState(); exit(1); },
        /* 3a */ [&] { std::fprintf(stderr, "Instruction 3a not implemented\n"); printState(); exit(1); },
        /* 3b */ [&] { std::fprintf(stderr, "Instruction 3b not implemented\n"); printState(); exit(1); },
        /* 3c */ [&] { std::fprintf(stderr, "Instruction 3c not implemented\n"); printState(); exit(1); },
        /* 3d */ [&] { std::fprintf(stderr, "Instruction 3d not implemented\n"); printState(); exit(1); },
        /* 3e */ [&] { std::fprintf(stderr, "Instruction 3e not implemented\n"); printState(); exit(1); },
        /* 3f */ [&] { std::fprintf(stderr, "Instruction 3f not implemented\n"); printState(); exit(1); },

        /* 40 */ [&] { std::fprintf(stderr, "Instruction 40 not implemented\n"); printState(); exit(1); },
        /* 41 */ [&] { std::fprintf(stderr, "Instruction 41 not implemented\n"); printState(); exit(1); },
        /* 42 */ [&] { std::fprintf(stderr, "Instruction 42 not implemented\n"); printState(); exit(1); },
        /* 43 */ [&] { std::fprintf(stderr, "Instruction 43 not implemented\n"); printState(); exit(1); },
        /* 44 */ [&] { std::fprintf(stderr, "Instruction 44 not implemented\n"); printState(); exit(1); },
        /* 45 */ [&] { std::fprintf(stderr, "Instruction 45 not implemented\n"); printState(); exit(1); },
        /* 46 */ [&] { std::fprintf(stderr, "Instruction 46 not implemented\n"); printState(); exit(1); },
        /* 47 */ [&] { std::fprintf(stderr, "Instruction 47 not implemented\n"); printState(); exit(1); },
        /* 48 */ [&] { std::fprintf(stderr, "Instruction 48 not implemented\n"); printState(); exit(1); },
        /* 49 */ [&] { std::fprintf(stderr, "Instruction 49 not implemented\n"); printState(); exit(1); },
        /* 4a */ [&] { std::fprintf(stderr, "Instruction 4a not implemented\n"); printState(); exit(1); },
        /* 4b */ [&] { std::fprintf(stderr, "Instruction 4b not implemented\n"); printState(); exit(1); },
        /* 4c */ [&] { std::fprintf(stderr, "Instruction 4c not implemented\n"); printState(); exit(1); },
        /* 4d */ [&] { std::fprintf(stderr, "Instruction 4d not implemented\n"); printState(); exit(1); },
        /* 4e */ [&] { std::fprintf(stderr, "Instruction 4e not implemented\n"); printState(); exit(1); },
        /* 4f */ [&] { std::fprintf(stderr, "Instruction 4f not implemented\n"); printState(); exit(1); },

        /* 50 */ [&] { std::fprintf(stderr, "Instruction 50 not implemented\n"); printState(); exit(1); },
        /* 51 */ [&] { std::fprintf(stderr, "Instruction 51 not implemented\n"); printState(); exit(1); },
        /* 52 */ [&] { std::fprintf(stderr, "Instruction 52 not implemented\n"); printState(); exit(1); },
        /* 53 */ [&] { std::fprintf(stderr, "Instruction 53 not implemented\n"); printState(); exit(1); },
        /* 54 */ [&] { std::fprintf(stderr, "Instruction 54 not implemented\n"); printState(); exit(1); },
        /* 55 */ [&] { std::fprintf(stderr, "Instruction 55 not implemented\n"); printState(); exit(1); },
        /* 56 */ [&] { std::fprintf(stderr, "Instruction 56 not implemented\n"); printState(); exit(1); },
        /* 57 */ [&] { std::fprintf(stderr, "Instruction 57 not implemented\n"); printState(); exit(1); },
        /* 58 */ [&] { std::fprintf(stderr, "Instruction 58 not implemented\n"); printState(); exit(1); },
        /* 59 */ [&] { std::fprintf(stderr, "Instruction 59 not implemented\n"); printState(); exit(1); },
        /* 5a */ [&] { std::fprintf(stderr, "Instruction 5a not implemented\n"); printState(); exit(1); },
        /* 5b */ [&] { std::fprintf(stderr, "Instruction 5b not implemented\n"); printState(); exit(1); },
        /* 5c */ [&] { std::fprintf(stderr, "Instruction 5c not implemented\n"); printState(); exit(1); },
        /* 5d */ [&] { std::fprintf(stderr, "Instruction 5d not implemented\n"); printState(); exit(1); },
        /* 5e */ [&] { std::fprintf(stderr, "Instruction 5e not implemented\n"); printState(); exit(1); },
        /* 5f */ [&] { std::fprintf(stderr, "Instruction 5f not implemented\n"); printState(); exit(1); },

        /* 60 */ [&] { std::fprintf(stderr, "Instruction 60 not implemented\n"); printState(); exit(1); },
        /* 61 */ [&] { std::fprintf(stderr, "Instruction 61 not implemented\n"); printState(); exit(1); },
        /* 62 */ [&] { std::fprintf(stderr, "Instruction 62 not implemented\n"); printState(); exit(1); },
        /* 63 */ [&] { std::fprintf(stderr, "Instruction 63 not implemented\n"); printState(); exit(1); },
        /* 64 */ [&] { std::fprintf(stderr, "Instruction 64 not implemented\n"); printState(); exit(1); },
        /* 65 */ [&] {
            // ADC - Add Memory to Accumulator with Carry
            // A + M + C -> A, C
            // N Z	C	I	D	V
            // + +	+	-	-	+
            // addressing	assembler	opc	bytes	cycles
            // zeropage	ADC oper	    65	2	    3
            bus.addr = ProgramCounter++;
            microcode.emplace([&] { bus.addr = bus.data; });
            microcode.emplace([&] {
                uint16_t temp = Accumulator + bus.data + getFlag(Carry);
                setFlag(Carry, temp > 255);
                setFlag(Zero, (temp & 0xff) == 0);
                setFlag(Negative, temp & 0b10000000);
                setFlag(Overflow, (Accumulator ^ bus.data) & 0x80 && (Accumulator ^ temp) & 0x80); // TODO double check this!
                Accumulator = temp & 0xff;
            }); },
        /* 66 */ [&] { std::fprintf(stderr, "Instruction 66 not implemented\n"); printState(); exit(1); },
        /* 67 */ [&] { std::fprintf(stderr, "Instruction 67 not implemented\n"); printState(); exit(1); },
        /* 68 */ [&] { std::fprintf(stderr, "Instruction 68 not implemented\n"); printState(); exit(1); },
        /* 69 */ [&] {
            // ADC - Add Memory to Accumulator with Carry
            // A + M + C -> A, C
            // N	Z	C	I	D	V
            // +	+	+	-	-	+
            // addressing	assembler	opc	bytes	cycles
            // immediate	ADC #oper	69	2	    2
            microcode.emplace([&] {
                uint16_t temp = Accumulator + bus.data + getFlag(Carry);
                setFlag(Carry, temp > 255);
                setFlag(Zero, (temp & 0xff) == 0);
                setFlag(Negative, temp & 0b10000000);
                setFlag(Overflow, (Accumulator ^ bus.data) & 0x80 && (Accumulator ^ temp) & 0x80); // TODO double check this!
                Accumulator = temp & 0xff;
            }); },
        /* 6a */ [&] { std::fprintf(stderr, "Instruction 6a not implemented\n"); printState(); exit(1); },
        /* 6b */ [&] { std::fprintf(stderr, "Instruction 6b not implemented\n"); printState(); exit(1); },
        /* 6c */ [&] { std::fprintf(stderr, "Instruction 6c not implemented\n"); printState(); exit(1); },
        /* 6d */ [&] { std::fprintf(stderr, "Instruction 6d not implemented\n"); printState(); exit(1); },
        /* 6e */ [&] { std::fprintf(stderr, "Instruction 6e not implemented\n"); printState(); exit(1); },
        /* 6f */ [&] { std::fprintf(stderr, "Instruction 6f not implemented\n"); printState(); exit(1); },

        /* 70 */ [&] { std::fprintf(stderr, "Instruction 70 not implemented\n"); printState(); exit(1); },
        /* 71 */ [&] { std::fprintf(stderr, "Instruction 71 not implemented\n"); printState(); exit(1); },
        /* 72 */ [&] { std::fprintf(stderr, "Instruction 72 not implemented\n"); printState(); exit(1); },
        /* 73 */ [&] { std::fprintf(stderr, "Instruction 73 not implemented\n"); printState(); exit(1); },
        /* 74 */ [&] { std::fprintf(stderr, "Instruction 74 not implemented\n"); printState(); exit(1); },
        /* 75 */ [&] { std::fprintf(stderr, "Instruction 75 not implemented\n"); printState(); exit(1); },
        /* 76 */ [&] { std::fprintf(stderr, "Instruction 76 not implemented\n"); printState(); exit(1); },
        /* 77 */ [&] { std::fprintf(stderr, "Instruction 77 not implemented\n"); printState(); exit(1); },
        /* 78 */ [&] {
            // SEI - Set Interrupt Disable Status
            // 1 -> I
            // N	Z	C	I	D	V
            // -	-	-	1	-	-
            // addressing	assembler	opc	bytes	cycles
            // implied	    SEI	        78	1	    2
            microcode.emplace([&] { setFlag(InterruptDisable, 1); }); },
        /* 79 */ [&] { std::fprintf(stderr, "Instruction 79 not implemented\n"); printState(); exit(1); },
        /* 7a */ [&] { std::fprintf(stderr, "Instruction 7a not implemented\n"); printState(); exit(1); },
        /* 7b */ [&] { std::fprintf(stderr, "Instruction 7b not implemented\n"); printState(); exit(1); },
        /* 7c */ [&] { std::fprintf(stderr, "Instruction 7c not implemented\n"); printState(); exit(1); },
        /* 7d */ [&] { std::fprintf(stderr, "Instruction 7d not implemented\n"); printState(); exit(1); },
        /* 7e */ [&] { std::fprintf(stderr, "Instruction 7e not implemented\n"); printState(); exit(1); },
        /* 7f */ [&] { std::fprintf(stderr, "Instruction 7f not implemented\n"); printState(); exit(1); },

        /* 80 */ [&] { std::fprintf(stderr, "Instruction 80 not implemented\n"); printState(); exit(1); },
        /* 81 */ [&] { std::fprintf(stderr, "Instruction 81 not implemented\n"); printState(); exit(1); },
        /* 82 */ [&] { std::fprintf(stderr, "Instruction 82 not implemented\n"); printState(); exit(1); },
        /* 83 */ [&] { std::fprintf(stderr, "Instruction 83 not implemented\n"); printState(); exit(1); },
        /* 84 */ [&] { std::fprintf(stderr, "Instruction 84 not implemented\n"); printState(); exit(1); },
        /* 85 */ [&] { std::fprintf(stderr, "Instruction 85 not implemented\n"); printState(); exit(1); },
        /* 86 */ [&] { std::fprintf(stderr, "Instruction 86 not implemented\n"); printState(); exit(1); },
        /* 87 */ [&] { std::fprintf(stderr, "Instruction 87 not implemented\n"); printState(); exit(1); },
        /* 88 */ [&] { std::fprintf(stderr, "Instruction 88 not implemented\n"); printState(); exit(1); },
        /* 89 */ [&] { std::fprintf(stderr, "Instruction 89 not implemented\n"); printState(); exit(1); },
        /* 8a */ [&] { std::fprintf(stderr, "Instruction 8a not implemented\n"); printState(); exit(1); },
        /* 8b */ [&] { std::fprintf(stderr, "Instruction 8b not implemented\n"); printState(); exit(1); },
        /* 8c */ [&] { std::fprintf(stderr, "Instruction 8c not implemented\n"); printState(); exit(1); },
        /* 8d */ [&] {
            // STA - Store Accumulator in Memory
            // A -> M
            // N	Z	C	I	D	V
            // -	-	-	-	-	-
            // addressing	assembler	opc	bytes	cycles
            // absolute	STA oper	8D	3	    4
            bus.addr = ProgramCounter++;
            static uint16_t addr;
            microcode.emplace([&] { bus.addr = ProgramCounter++, addr = bus.data; });
            microcode.emplace([&] { addr |= uint16_t(bus.data)<<8; });
            microcode.emplace([&] { bus.addr = addr, bus.data = Accumulator, bus.rw = Bus::WRITE; }); },
        /* 8e */ [&] { std::fprintf(stderr, "Instruction 8e not implemented\n"); printState();    exit(1); },
        /* 8f */ [&] { std::fprintf(stderr, "Instruction 8f not implemented\n"); printState(); exit(1); },

        /* 90 */ [&] { std::fprintf(stderr, "Instruction 90 not implemented\n"); printState(); exit(1); },
        /* 91 */ [&] { std::fprintf(stderr, "Instruction 91 not implemented\n"); printState(); exit(1); },
        /* 92 */ [&] { std::fprintf(stderr, "Instruction 92 not implemented\n"); printState(); exit(1); },
        /* 93 */ [&] { std::fprintf(stderr, "Instruction 93 not implemented\n"); printState(); exit(1); },
        /* 94 */ [&] { std::fprintf(stderr, "Instruction 94 not implemented\n"); printState(); exit(1); },
        /* 95 */ [&] { std::fprintf(stderr, "Instruction 95 not implemented\n"); printState(); exit(1); },
        /* 96 */ [&] { std::fprintf(stderr, "Instruction 96 not implemented\n"); printState(); exit(1); },
        /* 97 */ [&] { std::fprintf(stderr, "Instruction 97 not implemented\n"); printState(); exit(1); },
        /* 98 */ [&] { std::fprintf(stderr, "Instruction 98 not implemented\n"); printState(); exit(1); },
        /* 99 */ [&] { std::fprintf(stderr, "Instruction 99 not implemented\n"); printState(); exit(1); },
        /* 9a */ [&] {
            // TXS - Transfer X to Stack Register
            // X -> SP
            // N	Z	C	I	D	V
            // -	-	-	-	-	-
            // addressing	assembler	opc	bytes	cycles
            // implied	    TXS	        9A	1	    2
            microcode.emplace([&] { StackPointer = Xregister; }); },
        /* 9b */ [&] { std::fprintf(stderr, "Instruction 9b not implemented\n"); printState(); exit(1); },
        /* 9c */ [&] { std::fprintf(stderr, "Instruction 9c not implemented\n"); printState(); exit(1); },
        /* 9d */ [&] { std::fprintf(stderr, "Instruction 9d not implemented\n"); printState(); exit(1); },
        /* 9e */ [&] { std::fprintf(stderr, "Instruction 9e not implemented\n"); printState(); exit(1); },
        /* 9f */ [&] { std::fprintf(stderr, "Instruction 9f not implemented\n"); printState(); exit(1); },

        /* a0 */ [&] { std::fprintf(stderr, "Instruction a0 not implemented\n"); printState(); exit(1); },
        /* a1 */ [&] { std::fprintf(stderr, "Instruction a1 not implemented\n"); printState(); exit(1); },
        /* a2 */ [&] {
            // LDX - Load Index X with Memory
            // M -> X
            // N	Z	C	I	D	V
            // +	+	-	-	-	-
            // addressing      assembler       opc	bytes	cycles
            // immediate       LDX #oper	    A2	2	    2
            bus.addr = ProgramCounter++;
            microcode.emplace([&] {
                Xregister = bus.data;
                setFlag(Zero, Xregister == 0);
                setFlag(Negative, Xregister & 0b10000000);
            }); },
        /* a3 */ [&] { std::fprintf(stderr, "Instruction a3 not implemented\n"); printState(); exit(1); },
        /* a4 */ [&] { std::fprintf(stderr, "Instruction a4 not implemented\n"); printState(); exit(1); },
        /* a5 */ [&] { std::fprintf(stderr, "Instruction a5 not implemented\n"); printState(); exit(1); },
        /* a6 */ [&] { std::fprintf(stderr, "Instruction a6 not implemented\n"); printState(); exit(1); },
        /* a7 */ [&] { std::fprintf(stderr, "Instruction a7 not implemented\n"); printState(); exit(1); },
        /* a8 */ [&] { std::fprintf(stderr, "Instruction a8 not implemented\n"); printState(); exit(1); },
        /* a9 */ [&] {
            // LDA - Load Accumulator with Memory
            // M -> A
            // N	Z	C	I	D	V
            // +	+	-	-	-	-
            // addressing      assembler       opc	bytes	cycles
            // immediate       LDA #oper	    A9	2	    2
            bus.addr = ProgramCounter++;
            microcode.emplace([&] {
                Accumulator = bus.data;
                setFlag(Zero, Accumulator == 0);
                setFlag(Negative, Accumulator & 0b10000000);
            }); },
        /* aa */ [&] { std::fprintf(stderr, "Instruction aa not implemented\n"); printState(); exit(1); },
        /* ab */ [&] { std::fprintf(stderr, "Instruction ab not implemented\n"); printState(); exit(1); },
        /* ac */ [&] { std::fprintf(stderr, "Instruction ac not implemented\n"); printState(); exit(1); },
        /* ad */ [&] {
            // LDA - Load Accumulator with Memory
            // M -> A
            // N	Z	C	I	D	V
            // +	+	-	-	-	-
            // addressing      assembler       opc	bytes	cycles
            // absolute        LDA oper	       AD	3	    4
            bus.addr = ProgramCounter++;
            static uint16_t lo = 0;
            microcode.emplace([&] { bus.addr = ProgramCounter++, lo = bus.data; });
            microcode.emplace([&] { bus.addr = (uint16_t(bus.data) << 8) | lo; });
            microcode.emplace([&] {
                Accumulator = bus.data;
                setFlag(Zero, Accumulator == 0);
                setFlag(Negative, Accumulator & 0b10000000);
            }); },
        /* ae */ [&] { std::fprintf(stderr, "Instruction ae not implemented\n"); printState(); exit(1); },
        /* af */ [&] { std::fprintf(stderr, "Instruction af not implemented\n"); printState(); exit(1); },

        /* b0 */ [&] { std::fprintf(stderr, "Instruction b0 not implemented\n"); printState(); exit(1); },
        /* b1 */ [&] { std::fprintf(stderr, "Instruction b1 not implemented\n"); printState(); exit(1); },
        /* b2 */ [&] { std::fprintf(stderr, "Instruction b2 not implemented\n"); printState(); exit(1); },
        /* b3 */ [&] { std::fprintf(stderr, "Instruction b3 not implemented\n"); printState(); exit(1); },
        /* b4 */ [&] { std::fprintf(stderr, "Instruction b4 not implemented\n"); printState(); exit(1); },
        /* b5 */ [&] { std::fprintf(stderr, "Instruction b5 not implemented\n"); printState(); exit(1); },
        /* b6 */ [&] { std::fprintf(stderr, "Instruction b6 not implemented\n"); printState(); exit(1); },
        /* b7 */ [&] { std::fprintf(stderr, "Instruction b7 not implemented\n"); printState(); exit(1); },
        /* b8 */ [&] { std::fprintf(stderr, "Instruction b8 not implemented\n"); printState(); exit(1); },
        /* b9 */ [&] { std::fprintf(stderr, "Instruction b9 not implemented\n"); printState(); exit(1); },
        /* ba */ [&] { std::fprintf(stderr, "Instruction ba not implemented\n"); printState(); exit(1); },
        /* bb */ [&] { std::fprintf(stderr, "Instruction bb not implemented\n"); printState(); exit(1); },
        /* bc */ [&] { std::fprintf(stderr, "Instruction bc not implemented\n"); printState(); exit(1); },
        /* bd */ [&] {
            // LDA - Load Accumulator with Memory
            // M -> A
            // N	Z	C	I	D	V
            // +	+	-	-	-	-
            // addressing      assembler       opc	bytes	cycles
            // absolute,X      LDA oper,X	    BD	3	    4*
            // * add 1 to cycles if page boundary is crossed
            bus.addr = ProgramCounter++;
            static uint16_t lo = 0;
            microcode.emplace([&] { bus.addr = ProgramCounter++, lo = bus.data; });
            microcode.emplace([&] { bus.addr = Xregister + ((uint16_t(bus.data) << 8) | lo); });
            // TODO add 1 to cycles if page boundary is crossed
            microcode.emplace([&] {
                Accumulator = bus.data;
                setFlag(Zero, Accumulator == 0);
                setFlag(Negative, Accumulator & 0b10000000);
            }); },
        /* be */ [&] { std::fprintf(stderr, "Instruction be not implemented\n"); printState(); exit(1); },
        /* bf */ [&] { std::fprintf(stderr, "Instruction bf not implemented\n"); printState(); exit(1); },

        /* c0 */ [&] { std::fprintf(stderr, "Instruction c0 not implemented\n"); printState(); exit(1); },
        /* c1 */ [&] { std::fprintf(stderr, "Instruction c1 not implemented\n"); printState(); exit(1); },
        /* c2 */ [&] { std::fprintf(stderr, "Instruction c2 not implemented\n"); printState(); exit(1); },
        /* c3 */ [&] { std::fprintf(stderr, "Instruction c3 not implemented\n"); printState(); exit(1); },
        /* c4 */ [&] { std::fprintf(stderr, "Instruction c4 not implemented\n"); printState(); exit(1); },
        /* c5 */ [&] { std::fprintf(stderr, "Instruction c5 not implemented\n"); printState(); exit(1); },
        /* c6 */ [&] { std::fprintf(stderr, "Instruction c6 not implemented\n"); printState(); exit(1); },
        /* c7 */ [&] { std::fprintf(stderr, "Instruction c7 not implemented\n"); printState(); exit(1); },
        /* c8 */ [&] { std::fprintf(stderr, "Instruction c8 not implemented\n"); printState(); exit(1); },
        /* c9 */ [&] { std::fprintf(stderr, "Instruction c9 not implemented\n"); printState(); exit(1); },
        /* ca */ [&] { std::fprintf(stderr, "Instruction ca not implemented\n"); printState(); exit(1); },
        /* cb */ [&] { std::fprintf(stderr, "Instruction cb not implemented\n"); printState(); exit(1); },
        /* cc */ [&] { std::fprintf(stderr, "Instruction cc not implemented\n"); printState(); exit(1); },
        /* cd */ [&] { std::fprintf(stderr, "Instruction cd not implemented\n"); printState(); exit(1); },
        /* ce */ [&] { std::fprintf(stderr, "Instruction ce not implemented\n"); printState(); exit(1); },
        /* cf */ [&] { std::fprintf(stderr, "Instruction cf not implemented\n"); printState(); exit(1); },

        /* d0 */ [&] {
            // BNE - Branch on Result not Zero
            // branch on Z=0
            // N	Z	C	I	D	V
            // -	-	-	-	-	-
            // addressing	assembler	opc	bytes	cycles
            // relative	BNE oper	D0	2	2**
            // ** add 1 to cycles if branch occurs on same page
            // ** add 2 to cycles if branch occurs to different page
            bus.addr = ProgramCounter++;
            static uint8_t addr = 0;
            microcode.emplace([&] { addr = bus.data; });
            if (!getFlag(Zero)) {
                ProgramCounter += int8_t(addr);
                if (bus.addr & 0xff00 != ProgramCounter & 0xff00) { // TODO this is almost certainly wrong!
                    microcode.emplace([] {}); // add 2 to cycles if branch occurs to different page
                }
                microcode.emplace([&] {});
            } },
        /* d1 */ [&] { std::fprintf(stderr, "Instruction d1 not implemented\n"); printState(); exit(1); },
        /* d2 */ [&] { std::fprintf(stderr, "Instruction d2 not implemented\n"); printState(); exit(1); },
        /* d3 */ [&] { std::fprintf(stderr, "Instruction d3 not implemented\n"); printState(); exit(1); },
        /* d4 */ [&] { std::fprintf(stderr, "Instruction d4 not implemented\n"); printState(); exit(1); },
        /* d5 */ [&] { std::fprintf(stderr, "Instruction d5 not implemented\n"); printState(); exit(1); },
        /* d6 */ [&] { std::fprintf(stderr, "Instruction d6 not implemented\n"); printState(); exit(1); },
        /* d7 */ [&] { std::fprintf(stderr, "Instruction d7 not implemented\n"); printState(); exit(1); },
        /* d8 */ [&] {
            // CLD - Clear Decimal Mode
            // 0 -> D
            // N	Z	C	I	D	V
            // -	-	-	-	0	-
            // addressing	assembler	opc	bytes	cycles
            // implied	    CLD	        D8	1	    2
            microcode.emplace([&] { setFlag(DecimalMode,0); }); },
        /* d9 */ [&] { std::fprintf(stderr, "Instruction d9 not implemented\n"); printState(); exit(1); },
        /* da */ [&] { std::fprintf(stderr, "Instruction da not implemented\n"); printState(); exit(1); },
        /* db */ [&] { std::fprintf(stderr, "Instruction db not implemented\n"); printState(); exit(1); },
        /* dc */ [&] { std::fprintf(stderr, "Instruction dc not implemented\n"); printState(); exit(1); },
        /* dd */ [&] { std::fprintf(stderr, "Instruction dd not implemented\n"); printState(); exit(1); },
        /* de */ [&] { std::fprintf(stderr, "Instruction de not implemented\n"); printState(); exit(1); },
        /* df */ [&] { std::fprintf(stderr, "Instruction df not implemented\n"); printState(); exit(1); },

        /* e0 */ [&] { std::fprintf(stderr, "Instruction e0 not implemented\n"); printState(); exit(1); },
        /* e1 */ [&] { std::fprintf(stderr, "Instruction e1 not implemented\n"); printState(); exit(1); },
        /* e2 */ [&] { std::fprintf(stderr, "Instruction e2 not implemented\n"); printState(); exit(1); },
        /* e3 */ [&] { std::fprintf(stderr, "Instruction e3 not implemented\n"); printState(); exit(1); },
        /* e4 */ [&] { std::fprintf(stderr, "Instruction e4 not implemented\n"); printState(); exit(1); },
        /* e5 */ [&] { std::fprintf(stderr, "Instruction e5 not implemented\n"); printState(); exit(1); },
        /* e6 */ [&] { std::fprintf(stderr, "Instruction e6 not implemented\n"); printState(); exit(1); },
        /* e7 */ [&] { std::fprintf(stderr, "Instruction e7 not implemented\n"); printState(); exit(1); },
        /* e8 */ [&] { std::fprintf(stderr, "Instruction e8 not implemented\n"); printState(); exit(1); },
        /* e9 */ [&] { std::fprintf(stderr, "Instruction e9 not implemented\n"); printState(); exit(1); },
        /* ea */ [&] { std::fprintf(stderr, "Instruction ea not implemented\n"); printState(); exit(1); },
        /* eb */ [&] { std::fprintf(stderr, "Instruction eb not implemented\n"); printState(); exit(1); },
        /* ec */ [&] { std::fprintf(stderr, "Instruction ec not implemented\n"); printState(); exit(1); },
        /* ed */ [&] { std::fprintf(stderr, "Instruction ed not implemented\n"); printState(); exit(1); },
        /* ee */ [&] { std::fprintf(stderr, "Instruction ee not implemented\n"); printState(); exit(1); },
        /* ef */ [&] { std::fprintf(stderr, "Instruction ef not implemented\n"); printState(); exit(1); },

        /* f0 */ [&] {
        // BEQ - Branch on Result Zero
        // branch on Z=1
        // N	Z	C	I	D	V
        // -	-	-	-	-	-
        // addressing	assembler	opc	bytes	cycles
        // relative	BEQ oper	F0	2	2**
        // ** add 1 to cycles if branch occurs on same page
        // ** add 2 to cycles if branch occurs to different page
        bus.addr = ProgramCounter++;
        static int8_t addr = 0;
        microcode.emplace([&] { addr = bus.data; });
        if (getFlag(Zero)) {
            if (bus.addr & 0xff00 != ProgramCounter & 0xff00) { // TODO this is almost certainly wrong!
                microcode.emplace([] {}); // add 2 to cycles if branch occurs to different page
            }
            microcode.emplace([&] {
                ProgramCounter += int8_t(addr);
            });
        } },
        /* f1 */ [&] { std::fprintf(stderr, "Instruction f1 not implemented\n"); printState(); exit(1); },
        /* f2 */ [&] { std::fprintf(stderr, "Instruction f2 not implemented\n"); printState(); exit(1); },
        /* f3 */ [&] { std::fprintf(stderr, "Instruction f3 not implemented\n"); printState(); exit(1); },
        /* f4 */ [&] { std::fprintf(stderr, "Instruction f4 not implemented\n"); printState(); exit(1); },
        /* f5 */ [&] { std::fprintf(stderr, "Instruction f5 not implemented\n"); printState(); exit(1); },
        /* f6 */ [&] { std::fprintf(stderr, "Instruction f6 not implemented\n"); printState(); exit(1); },
        /* f7 */ [&] { std::fprintf(stderr, "Instruction f7 not implemented\n"); printState(); exit(1); },
        /* f8 */ [&] { std::fprintf(stderr, "Instruction f8 not implemented\n"); printState(); exit(1); },
        /* f9 */ [&] { std::fprintf(stderr, "Instruction f9 not implemented\n"); printState(); exit(1); },
        /* fa */ [&] { std::fprintf(stderr, "Instruction fa not implemented\n"); printState(); exit(1); },
        /* fb */ [&] { std::fprintf(stderr, "Instruction fb not implemented\n"); printState(); exit(1); },
        /* fc */ [&] { std::fprintf(stderr, "Instruction fc not implemented\n"); printState(); exit(1); },
        /* fd */ [&] { std::fprintf(stderr, "Instruction fd not implemented\n"); printState(); exit(1); },
        /* fe */ [&] { std::fprintf(stderr, "Instruction fe not implemented\n"); printState(); exit(1); },
        /* ff */ [&] { std::fprintf(stderr, "Instruction ff not implemented\n"); printState(); exit(1); },
    };

    void clk()
    {
        if (microcode.empty()) {
            bus.rw = Bus::READ;
            bus.addr = ProgramCounter++;
            microcode.emplace([&] { isa[bus.data](); });
            return;
        }

        microcode.front()();
        microcode.pop();
    }

public:
public:
    Cpu(Bus& bus)
        : bus(bus)
    {
        bus.rw = Bus::READ;
        bus.addr = 0xfffc;
        static uint16_t lo = 0;
        // TODO https://www.nesdev.org/wiki/CPU_power_up_state
        // TODO I think is is the same as a reset?
        // TODO the bus should be clocked of the CPU clock pin, not the global clock
        microcode.emplace([] {}); // Do one cycle in case the bus got the clock tick first
        microcode.emplace([&] { bus.addr = 0xfffd, lo = bus.data; });
        microcode.emplace([&] { ProgramCounter = uint16_t(bus.data) << 8 | lo; });
    }

    void printState()
    {
        std::fprintf(stderr, "-----------------------\n");
        std::fprintf(stderr, "Bus Address:    %04x\n", bus.addr);
        std::fprintf(stderr, "Bus Data:         %02x\n", bus.data);
        std::fprintf(stderr, "Bus RW:            %d\n", bus.rw);
        std::fprintf(stderr, "-----------------------\n");
        std::fprintf(stderr, "ProgramCounter: %04x\n", ProgramCounter);
        std::fprintf(stderr, "Accumulator:      %02x\n", Accumulator);
        std::fprintf(stderr, "Xregister:        %02x\n", Xregister);
        std::fprintf(stderr, "Yregister:        %02x\n", Yregister);
        std::fprintf(stderr, "StackPointer:     %02x\n", StackPointer);
        std::fprintf(stderr, "Status:           %02x (%c%c%c%c%c%c%c%c)\n", Status,
            // I think is wrong?
            (Status & Negative) ? 'N' : '-',
            (Status & Zero) ? 'Z' : '-',
            (Status & Carry) ? 'C' : '-',
            (Status & InterruptDisable) ? 'I' : '-',
            (Status & DecimalMode) ? 'D' : '-',
            (Status & Overflow) ? 'V' : '-',
            (Status & Break) ? 'B' : '-',
            (Status & 0b00000001) ? '1' : '-');
    }
};