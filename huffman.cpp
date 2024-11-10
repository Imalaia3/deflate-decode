#include "huffman.h"

void HuffmanTree::addValue(uint32_t binary, uint32_t nBits, uint16_t meaning) {
    auto currentNode = tree.getRoot();
    for (size_t i = 0; i < nBits; i++) {
        uint32_t bit = binary & (1 << (nBits-i-1));
        // Use > 0 because no shift right has occurred.
        if (bit > 0)
            currentNode = (currentNode->right != nullptr) ? currentNode->right : currentNode->addRight(0);
        else
            currentNode = (currentNode->left != nullptr) ? currentNode->left : currentNode->addLeft(0);
    }
    if (!currentNode->isLeaf()) {
        printf("Warning: Wrote %i at %li which is a non-leaf node!\n", meaning, currentNode);
    }
    currentNode->data = meaning;
}
uint16_t HuffmanTree::getValue(BitReader& reader) {
    auto currentNode = tree.getRoot();
    while (!currentNode->isLeaf()) {
        currentNode = (reader.getBit() == 1) ? currentNode->right : currentNode->left; 
    }
    return currentNode->data;
}

// Generate huffman codes with canonical prefixes and add them to a tree
void HuffmanTree::generateHuffman(HuffmanTree& dst, std::vector<uint32_t> lengths, uint32_t startValue) {

    uint32_t maxBitLength = *std::max_element(lengths.begin(), lengths.end());

    /* Calculate how many times each bit count occurs in the given lengths list. */
    // NOTE: Add 1 on maxBitLength so 0 is included
    std::vector<uint32_t> occurrences(maxBitLength+1, 0);
    for (auto &&l : lengths) {
        if (l != 0) {
            occurrences[l]++;
        }
    }
    
    /* Calculate The value of each bitcount so that they are unique
     * If more than one value have the same bitcount, this is used
     * as a "base" and 1 is added per each occurrence.
     */
    std::vector<uint32_t> codeBases;
    for (size_t i = 0; i < maxBitLength+1; i++) {
        uint32_t lastBase = (i == 0) ? 0 : codeBases[i-1]; // Base of last bit length
        uint32_t lastContained = (i == 0) ? 0 : occurrences[i-1]; // how many times was the last base used
        codeBases.push_back((lastBase+lastContained)<<1); // shifted left by one so we get a different prefix
    }

    // FIXME: creating a whole vector for this is pretty unoptimized
    std::vector<uint32_t> deltas(maxBitLength+1, 0);
    for (size_t i = 0; i < lengths.size(); i++) {
        auto length = lengths[i];
        if (length != 0) {
            dst.addValue(codeBases[length]+deltas[length], length, startValue+i);
            deltas[length]++;
        }
    }
}