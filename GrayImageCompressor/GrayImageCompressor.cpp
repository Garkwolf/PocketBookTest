#include "GrayImageCompressor.h"
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <vector>

namespace {
    constexpr int cParseStep = 4;
}

void GrayImageCompressor::writeBit(std::ofstream& outFile, std::uint8_t& buffer, std::size_t& bitCount, bool bit) {
    if (bit) {
        buffer |= (1 << (7 - bitCount));
    }
    bitCount++;
    if (bitCount == 8) {
        outFile.put(buffer);
        buffer = 0;
        bitCount = 0;
    }
}

void GrayImageCompressor::flushBuffer(std::ofstream& outFile, std::uint8_t& buffer, std::size_t& bitCount) {
    if (bitCount > 0) {
        outFile.put(buffer);
        buffer = 0;
        bitCount = 0;
    }
}

bool GrayImageCompressor::readBit(std::ifstream& inFile, std::uint8_t& buffer, std::size_t& bitCount) {
    if (bitCount == 8) {
        buffer = inFile.get();
        bitCount = 0;
    }
    bool bit = (buffer & (1 << (7 - bitCount))) != 0;
    bitCount++;
    return bit;
}

void GrayImageCompressor::compress(const RawImageData& image, const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Unable to open file for writing");
    }

    // Write the title and size of the image
    outFile.put('B');
    outFile.put('A');
    outFile.put((image.width >> 8) & 0xFF);
    outFile.put(image.width & 0xFF);
    outFile.put((image.height >> 8) & 0xFF);
    outFile.put(image.height & 0xFF);

    // Compressed data
    std::uint8_t buffer = 0;
    std::size_t bitCount = 0;

    std::vector<bool> rowIndex(image.height, false);
    for (std::uint16_t j = 0; j < image.height; ++j) {
        bool isEmpty = true;
        for (std::uint16_t i = 0; i < image.width; ++i) {
            if (image.data[j * image.width + i] != 0xFF) {
                isEmpty = false;
                break;
            }
        }
        rowIndex[j] = !isEmpty;
        writeBit(outFile, buffer, bitCount, !isEmpty);
    }

    for (std::uint16_t j = 0; j < image.height; ++j) {
        if (rowIndex[j]) {
            for (std::uint16_t i = 0; i < image.width; i += cParseStep) {
                bool isAllWhite = true;
                bool isAllBlack = true;

                for (std::uint16_t k = 0; k < cParseStep && i + k < image.width; ++k) {
                    if (image.data[j * image.width + i + k] != 0xFF) {
                        isAllWhite = false;
                    }
                    if (image.data[j * image.width + i + k] != 0x00) {
                        isAllBlack = false;
                    }
                }

                if (isAllWhite) {
                    writeBit(outFile, buffer, bitCount, 0); // 0
                }
                else if (isAllBlack) {
                    writeBit(outFile, buffer, bitCount, 1); // 10
                    writeBit(outFile, buffer, bitCount, 0);
                }
                else {
                    writeBit(outFile, buffer, bitCount, 1); // 11
                    writeBit(outFile, buffer, bitCount, 1);
                    for (std::uint16_t k = 0; k < cParseStep && i + k < image.width; ++k) {
                        for (int b = 7; b >= 0; --b) {
                            writeBit(outFile, buffer, bitCount, (image.data[j * image.width + i + k] >> b) & 1);
                        }
                    }
                }
            }
        }
    }

    // Reset buffer
    flushBuffer(outFile, buffer, bitCount);

    outFile.close();
}

void GrayImageCompressor::decompress(RawImageData& image, const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Unable to open file for reading");
    }

    // Read the title and size of the image
    char id1 = inFile.get();
    char id2 = inFile.get();
    if (id1 != 'B' || id2 != 'A') {
        throw std::runtime_error("Invalid file format");
    }

    image.width = (inFile.get() << 8) | inFile.get();
    image.height = (inFile.get() << 8) | inFile.get();

    image.data = std::make_unique<unsigned char[]>(image.width * image.height);

    // Read rows indexes
    std::vector<bool> rowIndex(image.height);
    std::uint8_t buffer = 0;
    std::size_t bitCount = 8; // Set 8 to read full byte

    for (std::uint16_t j = 0; j < image.height; ++j) {
        rowIndex[j] = readBit(inFile, buffer, bitCount);
    }

    // Decompress data
    for (std::uint16_t j = 0; j < image.height; ++j) {
        if (!rowIndex[j]) {
            // Empty row
            std::memset(&image.data[j * image.width], 0xFF, image.width);
        }
        else {
            // Nonempty row
            for (std::uint16_t i = 0; i < image.width; i += cParseStep) {
                bool firstBit = readBit(inFile, buffer, bitCount);
                if (!firstBit) {
                    // 4 white pixels
                    std::memset(&image.data[j * image.width + i], 0xFF, 4);
                }
                else {
                    bool secondBit = readBit(inFile, buffer, bitCount);
                    if (!secondBit) {
                        // 4 black pixels
                        std::memset(&image.data[j * image.width + i], 0x00, 4);
                    }
                    else {
                        // Another pixels
                        for (std::uint16_t k = 0; k < cParseStep && i + k < image.width; ++k) {
                            std::uint8_t pixel = 0;
                            for (int b = 7; b >= 0; --b) {
                                if (readBit(inFile, buffer, bitCount)) {
                                    pixel |= (1 << b);
                                }
                            }
                            image.data[j * image.width + i + k] = pixel;
                        }
                    }
                }
            }
        }
    }

    inFile.close();
}