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
        HuffmanTree::generateHuffman(m_fixedLiteralLengthTree, bitLengths, 0);
    }
    /* Distance Tree */
    // NOTE: 30-31 never occur, so don't include them
    {
        std::vector<uint32_t> bitLengths;
        bitLengths.reserve(31);
        for (size_t i = 0; i < 31; i++) {
            bitLengths.push_back(5);
        }
        HuffmanTree::generateHuffman(m_fixedDistanceTree, bitLengths, 0);
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
            UNUSED(notLength);
            for (size_t i = 0; i < length; i++)
                m_decodedBytes.push_back(m_inputStream.getByte());            
        }
        else if (blockType == BTYPE_DYNAMIC_HUFFMAN) {
            std::cout << "Decoding Dynamic Block...\n";
            decodeDynamicBlock();
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

            // Since as we push_back, the vector grows, indexing at size()-distance will do the trick.
            for (size_t i = 0; i < length; i++) {
                m_decodedBytes.push_back(m_decodedBytes[m_decodedBytes.size()-distance]);
            }
        }
        symbol = literalLengthTree.getValue(m_inputStream);
    }
}

void DeflateDecoder::decodeDynamicBlock() {
    // https://datatracker.ietf.org/doc/html/rfc1951 (page 13)
    uint16_t nLiteralLengthCodes = m_inputStream.getBits(5) + 257;
    uint16_t nDistanceCodes = m_inputStream.getBits(5) + 1;
    uint16_t nCodeLengthCodes = m_inputStream.getBits(4) + 4;

    // nCodeLengthCodes 3-bit bit lengths for the code length tree will follow
    // this tree is used to decode the followign distance and literal/length bit lengths
    const uint8_t lengthOrder[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    // zero out a vector of lengths, so any lengths that are omitted from the stream remain 0.
    std::vector<uint32_t> codeLengthLengths(19, 0);
    std::cout << "# Code Length Codes = " << nCodeLengthCodes << "\n";
    for (size_t i = 0; i < nCodeLengthCodes; i++) {
        codeLengthLengths[lengthOrder[i]] = m_inputStream.getBits(3);
    }

    HuffmanTree codeLengthTree;
    HuffmanTree::generateHuffman(codeLengthTree, codeLengthLengths, 0);

    // Code Lengths encoded with the codeLengthTree for the Literal/Length Tree follow.
    // After that, code lengths encoded with the codeLengthTree for the Distance Tree occurr,
    // However, since some bytes mean, repeat last value n times, those code lengths can rely
    // on eachother. To solve that the last value is stored

    std::vector<uint32_t> literalLengthCodes;
    literalLengthCodes.reserve(nLiteralLengthCodes);
    std::vector<uint32_t> distanceCodes;
    distanceCodes.reserve(nDistanceCodes);
    uint16_t lastSymbol = -1;

    // since repeats can occur use length of both arrays as guard
    while ((literalLengthCodes.size() + distanceCodes.size()) < nLiteralLengthCodes + nDistanceCodes) {
        uint16_t symbol = codeLengthTree.getValue(m_inputStream);
        if (symbol < 16) {
            if (literalLengthCodes.size() < nLiteralLengthCodes)
                literalLengthCodes.push_back(symbol);
            else
                distanceCodes.push_back(symbol);
            lastSymbol = symbol;
        } else if (symbol == 16) { // repeat last symbol 3 - 6 times
            uint8_t repeats = m_inputStream.getBits(2) + 3;
            for (uint8_t i = 0; i < repeats; i++) {
                if (literalLengthCodes.size() < nLiteralLengthCodes)
                    literalLengthCodes.push_back(lastSymbol);
                else
                    distanceCodes.push_back(lastSymbol);
            // last symbol remains the same
            }
        } else if (symbol == 17) { // repeat zero 3 - 10 times
            uint8_t repeats = m_inputStream.getBits(3) + 3;
            for (uint8_t i = 0; i < repeats; i++) {
                if (literalLengthCodes.size() < nLiteralLengthCodes)
                    literalLengthCodes.push_back(0);
                else
                    distanceCodes.push_back(0);
            }
            lastSymbol = 0; // set to zero from this operation
        } else if (symbol == 18) { // repeat zero 11 - 138
            uint8_t repeats = m_inputStream.getBits(7) + 11;
            for (uint8_t i = 0; i < repeats; i++) {
                if (literalLengthCodes.size() < nLiteralLengthCodes)
                    literalLengthCodes.push_back(0);
                else
                    distanceCodes.push_back(0);
            }
            lastSymbol = 0; // set to zero from this operation
        } else {
            assert(false && "unreachable.");
        }
    }

    std::cout << "Creating Literal/Length Tree...\n";
    HuffmanTree literalLengthTree;
    HuffmanTree::generateHuffman(literalLengthTree, literalLengthCodes, 0);
    std::cout << "Creating Distance Tree...\n";
    HuffmanTree distanceTree;
    HuffmanTree::generateHuffman(distanceTree, distanceCodes, 0);

    decodeBlock(literalLengthTree, distanceTree);
}