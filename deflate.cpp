#include "deflate.h"
#include <assert.h>

DeflateDecoder::DeflateDecoder(BitReader& stream) : m_inputStream(stream) {

    // Pre-Initialize Fixed Huffman Trees
    generateFixedTrees();
}

void DeflateDecoder::generateFixedTrees() {
    std::cout << "Creating Fixed Huffman Trees...\n";
    
    /* Literal/Length Tree*/
    // See: https://datatracker.ietf.org/doc/html/rfc1951 (3.2.6)
    {
        std::vector<uint32_t> bitLengths;
        bitLengths.reserve(288);
        for (size_t i = 0; i < (0b10111111-0b00110000+1); i++) {
            bitLengths.push_back(8);
        }
        for (size_t i = 0; i < (0b111111111-0b110010000+1); i++) {
            bitLengths.push_back(9);
        }
        for (size_t i = 0; i < (0b0010111-0b0000000+1); i++) {
            bitLengths.push_back(7);
        }
        for (size_t i = 0; i < (0b11000111-0b11000000+1); i++) {
            bitLengths.push_back(8);
        }
        HuffmanTree::generateHuffman(m_fixedLiteralLengthTree, bitLengths, 0, 286);
    }
    /* Distance Tree */
    // NOTE: 30-31 never occur, so don't include them
    {
        std::vector<uint32_t> bitLengths;
        bitLengths.reserve(31);
        for (size_t i = 0; i < 31; i++) {
            bitLengths.push_back(5);
        }
        HuffmanTree::generateHuffman(m_fixedDistanceTree, bitLengths, 0, 29);
    }
}

std::vector<uint8_t> DeflateDecoder::decode() {    
    bool isEndBlock = false;
    while(!isEndBlock) { 
        isEndBlock = (m_inputStream.getBit() == 1);
        uint8_t blockType = m_inputStream.getBits(2);
        if (blockType == BTYPE_NO_COMPRESSION) {
            std::cout << "Decoding Literal Block...\n";
            uint32_t length = m_inputStream.getBytes(2);
            uint32_t notLength = m_inputStream.getBytes(2); // = ~length
            for (size_t i = 0; i < length; i++)
                m_decodedBytes.push_back(m_inputStream.getByte());            
        }
        else if (blockType == BTYPE_DYNAMIC_HUFFMAN) {
            // TODO
            std::cout << "Decoding Dynamic Block...\n";
            assert(false && "Not Implemented.");
        }
        else if (blockType == BTYPE_FIXED_HUFFMAN) {
            decodeBlock(m_fixedLiteralLengthTree, m_fixedDistanceTree);
        }
        else {
            assert(false && "Unreachable.");
        }
    }
    return m_decodedBytes;
}

void DeflateDecoder::decodeBlock(HuffmanTree& literalLengthTree, HuffmanTree& distanceTree) {
    uint16_t symbol = literalLengthTree.getValue(m_inputStream);
    while (symbol != 256) {
        if (symbol < 256) { // literal byte
            m_decodedBytes.push_back(symbol);
        } else { // LZ77 pair (consult https://datatracker.ietf.org/doc/html/rfc1951 (3.2.7))
            // -257 to get true index
            uint32_t length = StartLength[symbol-257] + m_inputStream.getBits(LengthExtraBits[symbol-257]);
            uint16_t nextSymbol = distanceTree.getValue(m_inputStream); // extra bits symbol
            uint32_t distance = StartDist[nextSymbol] + m_inputStream.getBits(DistanceExtraBits[nextSymbol]);
            std::cout << "LZW (length, distance) pair = (" << length << ", " << distance << ")\n"; 
            
            // Since as we push_back, the vector grows, just by indexing at -distance will do the trick.
            for (size_t i = 0; i < length; i++)
                m_decodedBytes.push_back(m_decodedBytes[-distance]);
        }
        symbol = literalLengthTree.getValue(m_inputStream);
    }
}