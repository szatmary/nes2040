#pragma once
#include <array>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

// http://fms.komkon.org/EMUL8/NES.html
class Rom {
public:
    std::vector<uint8_t> data;
    bool open(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        data.resize(size);
        if (!file.read((char*)data.data(), data.size())) {
            return false;
        }
        return true;
    }

    int programRomSize() const
    {
        return 16384 * data[4];
    }

    uint8_t* prgRomBegin()
    {
        return data.data() + 16;
    }

    uint8_t* prgRomEnd()
    {
        return prgRomBegin() + programRomSize();
    }

    // // TODO view?
    // std::vector<uint8_t> getProgramRom() const
    // {
    //     std::vector<uint8_t> rom;
    //     for (int i = 0; i < programRomSize(); i++) {
    //         rom.push_back(data[16 + i]);
    //     }
    //     return rom;
    // }

    int characterRomSize() const
    {
        return 8192 * data[5];
    }

    using tile = std::array<uint8_t, 8 * 8>;
    tile getTile(int x) const
    {
        tile chr;
        auto o = 16 + programRomSize();
        o += x * 16;
        for (int x = 0; x < 8; x++, o++) {
            chr[x] = (data[o] >> 7 & 1) | (data[o + 8] >> 6 & 2);
            chr[x + 8] = (data[o] >> 6 & 1) | (data[o + 8] >> 5 & 2);
            chr[x + 16] = (data[o] >> 5 & 1) | (data[o + 8] >> 4 & 2);
            chr[x + 24] = (data[o] >> 4 & 1) | (data[o + 8] >> 3 & 2);
            chr[x + 32] = (data[o] >> 3 & 1) | (data[o + 8] >> 2 & 2);
            chr[x + 40] = (data[o] >> 2 & 1) | (data[o + 8] >> 1 & 2);
            chr[x + 48] = (data[o] >> 1 & 1) | (data[o + 8] >> 0 & 2);
            chr[x + 56] = (data[o] >> 0 & 1) | (data[o + 8] << 1 & 2);
        }
        return chr;
    }

    void dumpCharacterRom(const std::string& file) const
    {
        std::ofstream out(file, std::ios::binary);
        std::vector<tile> tiles;
        // TODO use characterRomSize!
        for (int i = 0; i < 256; i++) {
            tiles.push_back(getTile(i));
        }
        out << "/* XPM */\nstatic char * XFACE[] = {\n";
        out << "\"8 2048 4 1\",\n";
        out << "\"a c None\",\n";
        out << "\"b c #ff0000\",\n";
        out << "\"c c #00ff00\",\n";
        out << "\"d c #0000ff\",\n";
        for (const auto& tile : tiles) {
            for (int x = 0; x < 8; x++) {
                out << "\"";
                for (int y = 0; y < 8; y++) {
                    out << char('a' + tile[(x * 8) + y]);
                }
                out << "\",\n";
            }
        }

        out << "};\n";
    }
};