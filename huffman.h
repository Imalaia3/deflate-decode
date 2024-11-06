#pragma once
#include "bitreader.h"
#include "binary_tree.h"
#include <vector>

class HuffmanTree {
public:
    HuffmanTree(){}
    void addValue(uint32_t binary, uint32_t nBits, uint16_t meaning);
    
    uint16_t getValue(BitReader& reader);
    BinaryTree<uint16_t>* getTree() { return &tree; }
    
    static void generateHuffman(HuffmanTree& dst, std::vector<uint32_t> lengths, uint32_t lowerBounds, uint32_t upperBounds);
private:
    BinaryTree<uint16_t> tree = BinaryTree<uint16_t>(0);  
};