#pragma once

#include <string>
#include <cstdint>
#include <memory>

struct RawImageData {
    std::uint16_t width;
    std::uint16_t height;
    std::unique_ptr<unsigned char[]> data;

    RawImageData(int w = 0, int h = 0) : width(w), height(h), data(nullptr) {}
};

class GrayImageCompressor {
public:
    static void compress(const RawImageData& image, const std::string& filename);
    static void decompress(RawImageData& image, const std::string& filename);

private:
    static void writeBit(std::ofstream& outFile, std::uint8_t& buffer, std::size_t& bitCount, bool bit);
    static void flushBuffer(std::ofstream& outFile, std::uint8_t& buffer, std::size_t& bitCount);
    static bool readBit(std::ifstream& inFile, std::uint8_t& buffer, std::size_t& bitCount);
};
