# nes2040
Experimental NES emulation

I have never written an emulator before, but have always wanted to. This is part of that. I most likely will not finish.

The general idea is "What would it look like someone was to recreated clasic gaming hardware by painstakingly emulating the chips individually and accurately?"

Basically what the FPGA emulation community is (pretty successfully) trying to achieve. But instead, utiilizing small SoCs. Hence the name nes2040, from the rp2040

At this point I dont know what I dont know, so this is just for funzies.

This code does not accualy do anything rite now (Except run the first 8ish instrucstion of super mario brothers, then infinite loop because the PPU does not emit a VBI)