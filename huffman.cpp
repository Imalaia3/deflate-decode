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


void HuffmanTree::generateHuffman(HuffmanTree& dst, std::vector<uint32_t> lengths, uint32_t lowerBounds, uint32_t upperBounds) {
    std::cout << "lengths.size() = " << lengths.size() << "\n";
    std::cout << "lowerBounds = " << lowerBounds << "\n";
    std::cout << "upperBounds = " << upperBounds << "\n";
    std::cout << "range = " << upperBounds-lowerBounds+2 << "\n";

    for (auto &&l : lengths)
    {
        std::cout << l << " ";
    }
    std::cout << "\n";
    

    assert(lengths.size() == (upperBounds-lowerBounds+2)); // +2: include lowerBounds & upperBounds

    uint32_t maxBitLength = 0;
    for (auto&& i : lengths) {
        if (i > maxBitLength)
            maxBitLength = i;
    }

    /* Calculate how many times each bit count occurs in the given lengths list. */
    //FIXME: this can be done way faster.
    //FIXME: a length vector can contain zeroes, which should be treated as the symbol not existing at all
    std::vector<uint32_t> occurrences;
    for (size_t i = 0; i < maxBitLength+1; i++) {
        uint32_t total = 0;
        for (auto&& length : lengths) {
            if (length == i && length != 0 )
                total++;
        }
        std::cout << "Occurance " << i << ' ' << total << "\n";
        occurrences.push_back(total);
    }
    
    /* Calculate The value of each bitcount so that they are unique
     * If more than one value have the same bitcount, this is used
     * as a "base" and 1 is added per each occurrence.
     */
    std::vector<uint32_t> codeBases;
    for (size_t i = 0; i < maxBitLength+1; i++) {
        uint32_t lastBase = (i == 0) ? 0 : codeBases[i-1]; // Base of last bit length
        uint32_t lastContained = (i == 0) ? 0 : occurrences[i-1]; // how many times was the last base used
        std::cout << "Code Base " << i << ' ' << (lastBase+lastContained)*2 << "\n";
        codeBases.push_back((lastBase+lastContained)*2); // shifted left by one so we get a different prefix
    }

    std::vector<uint32_t> deltas(maxBitLength, 0);
    for (size_t i = 0; i < lengths.size(); i++) {
        auto length = lengths[i];
        if (length != 0) {
            auto base = codeBases[length];
            dst.addValue(base+deltas[length], length, lowerBounds+i);
            deltas[length] += 1;
        }
    }
        
}