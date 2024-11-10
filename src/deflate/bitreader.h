/*
 * Part of the Imalaia3 Utility Classes.
 * Copyright (c): Imalaia3 2024-
 */
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <iostream>

class BitReader {
public:
    BitReader() {};
    
    /*
     * bytes: pointer to source data.
     * bitOrder: order of reading bits. 1 = shift left by 0..7, -1 shift left by 7..0.
     */
    BitReader(uint8_t* bytes, int32_t bitOrder = 1) : m_bytes(bytes) {
        m_bitOrder = bitOrder;
        if (m_bitOrder == 1)
            m_bitOffset = 0;
        else if (m_bitOrder == -1)
            m_bitOffset = 7;
        else
            assert(false && "Invalid bit order!");
    }

    // Get a whole byte while discarding remaining bits of last byte
    uint8_t getByte() {
        m_bitOffset = 0;
        m_byteOffset++; // discard last byte
        return m_bytes[m_byteOffset];
    }

    uint32_t getBytes(uint32_t nBytes) {
        uint32_t result = 0;
        for (size_t i = 0; i < nBytes; i++) {
            result |= getByte() << 8*i;
        }
        return result;  
    }
    

    uint8_t getBit() {
        if (m_bitOffset > 7) { // will also work on negatives.
            if (m_bitOrder == 1)
                m_bitOffset = 0;
            else if (m_bitOrder == -1)
                m_bitOffset = 7;

            m_byteOffset++;
        }
        uint8_t result = (m_bytes[m_byteOffset] & (1 << m_bitOffset)) >> m_bitOffset;
        m_bitOffset += m_bitOrder;
        return result;
    }

    uint32_t getBits(uint32_t nBits) {
        uint32_t result = 0;
        for (size_t i = 0; i < nBits; i++) {
            result |= getBit() << i;
        }
        return result;        
    }

    size_t getByteOffset() { return m_byteOffset; }
    uint8_t getBitOffset() { return m_bitOffset; }

private:
    uint8_t* m_bytes = nullptr;
    int32_t m_bitOrder;
    size_t m_byteOffset = 0;
    uint8_t m_bitOffset = 0;
};