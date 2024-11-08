#pragma once
#include "bitreader.h"
#include "huffman.h"
#include "vector"

#define BTYPE_NO_COMPRESSION  0
#define BTYPE_FIXED_HUFFMAN   1
#define BTYPE_DYNAMIC_HUFFMAN 2

/* Extra Bit and Value Tables (https://datatracker.ietf.org/doc/html/rfc1951 (Page 12))*/

//Lengths
// Codes 257-285 inclusive
const uint32_t LengthExtraBits[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
const uint32_t StartLength[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};

// Distances
// Codes 0-29 inclusive
const uint32_t DistanceExtraBits[] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
const uint32_t StartDist[] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};

/* Code Length Bit Lengths Order */
// In what order does each bitLength appear
// https://datatracker.ietf.org/doc/html/rfc1951 (Page 14)
const uint32_t CodeLengthBitLengthOrder[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};


class DeflateDecoder {
public:
    DeflateDecoder(BitReader& stream);
    // DeflateDecoder(uint8_t* inputData);
    std::vector<uint8_t> decode();
    bool isComplete() { return m_decodingComplete; }
private:
    void generateFixedTrees();
    void decodeBlock(HuffmanTree& literalLengthTree, HuffmanTree& distanceTree);
    void decodeDynamicBlock();

    std::vector<uint8_t> m_decodedBytes;
    BitReader& m_inputStream;
    HuffmanTree m_fixedLiteralLengthTree, m_fixedDistanceTree;
    bool m_decodingComplete = false;
};