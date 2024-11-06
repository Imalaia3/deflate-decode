#include <iostream>
#include "deflate.h"
#include "bitreader.h"
#include <cstring>

struct zlibMetadata {
    // CMF Byte
    uint8_t method: 4; // compression method
    uint8_t cinfo: 4;  // log2(LZW_WINDOW_SIZE) if > 7 error
    // FLG Byte
    uint8_t fcheck: 5; // checksum of metadata
    uint8_t fdict: 1;  // does a dictionary exist?
    uint8_t flevel: 2; // compression level
    
    
} __attribute__((packed));


int main(int argc, char const *argv[]) {
    const uint8_t hello[] = {0x78, 0x9c, 0xf3, 0x48, 0xcd, 0xc9, 0xc9, 0x57, 0x70, 0x71, 0x75, 0xf3, 0x71, 0x0c, 0x71, 0x55, 0x08, 0xcf, 0x2f, 0xca, 0x49, 0x51, 0x04, 0x00, 0x43, 0x3c, 0x06, 0x53};
    const uint8_t lipsum[] = { 0x78,0x9c,0x5d,0x56,0x5b,0x8e,0xe3,0x36,0x10,0xfc,0xf7,0x29,0xfa,0x00,0x82,0xee,0x10,0x64,0x13,0x20,0x40,0x5e,0xc0,0x22,0xf9,0xa7,0xa5,0xb6,0xa7,0x03,0x8a,0xd4,0xf2,0x61,0xcc,0xf1,0x53,0xcd,0xa7,0x3c,0xc0,0x00,0x1e,0x5b,0x14,0x59,0x5d,0x5d,0x55,0xcd,0xdb,0xed,0x77,0x1f,0xf8,0x20,0x39,0x63,0x3e,0x68,0xf7,0xd6,0x07,0x8a,0x92,0xc8,0x1c,0x9c,0x16,0xda,0xbc,0x8b,0xbc,0x25,0x4e,0x39,0x90,0xd9,0xe5,0x94,0xb8,0x89,0x7b,0x12,0x5b,0x49,0x2b,0x7d,0xf3,0x8e,0x37,0x3a,0x7d,0x48,0xa6,0xbd,0xce,0x99,0x52,0x90,0x98,0xe4,0x47,0x66,0x32,0x79,0x4b,0x3e,0xac,0xf4,0x6b,0x8e,0x1b,0x53,0x4e,0xc4,0x4e,0x0e,0x3a,0x8c,0xe5,0x98,0xcd,0x6e,0x88,0x83,0x8f,0xf8,0x9a,0x92,0x44,0x8a,0x7e,0xd7,0xdf,0xc9,0x24,0x8a,0xbc,0x93,0x65,0xbf,0xd2,0x4f,0x16,0xdb,0x18,0x20,0x73,0x64,0xcd,0x96,0x63,0x05,0x83,0x9f,0x12,0xed,0x59,0xe8,0x83,0xdd,0x1e,0x38,0x00,0xea,0x6e,0x4e,0xb9,0xe7,0xb9,0x09,0x5e,0x28,0x75,0xac,0xf4,0x87,0xc9,0x80,0x43,0xce,0x3b,0x02,0x14,0xfc,0xd1,0x83,0xc3,0xc1,0x2e,0xe5,0x63,0xb9,0x20,0xd1,0xed,0x70,0xec,0x42,0x11,0x50,0x51,0xa4,0x82,0x38,0x56,0xfa,0x27,0xf5,0xb7,0x9c,0x44,0xbb,0xd0,0x4b,0x5e,0x1c,0x02,0x96,0xcb,0x86,0x0d,0x08,0x9c,0x80,0x0f,0x7a,0x31,0x1e,0x71,0x96,0x78,0xf8,0x9d,0x92,0xb8,0x4d,0xf6,0xec,0x12,0x29,0xf4,0x95,0x7e,0x73,0x40,0x7d,0xe0,0x89,0xa7,0x33,0x07,0x60,0xec,0x34,0x28,0x6b,0x49,0xb0,0xf7,0xa2,0x35,0xbf,0x18,0x9c,0xdd,0xb3,0xc5,0xae,0xb5,0xd4,0xbb,0x35,0x6e,0x57,0x8e,0x7f,0xce,0xc1,0xdc,0x45,0xe9,0xe7,0x80,0x85,0x20,0x66,0xa1,0x90,0x53,0xc0,0x4a,0x6c,0xd5,0x96,0x91,0x59,0x28,0xf1,0x71,0xe6,0x42,0x60,0xc5,0xbc,0xd2,0x5f,0x61,0x13,0x7a,0x99,0x20,0xf8,0xd9,0x99,0xe4,0xb5,0x27,0x27,0xe3,0xbf,0x42,0x16,0x27,0x10,0xf0,0x44,0x61,0x28,0x27,0xd2,0x69,0x02,0xce,0x10,0x30,0x43,0x87,0x77,0x89,0xe3,0x82,0x57,0xd0,0x36,0x3d,0x38,0x08,0x0a,0x06,0x36,0x34,0x2b,0xc7,0xc1,0xe9,0xa8,0x74,0x51,0x50,0xe0,0x26,0x19,0xa6,0x47,0x80,0x3a,0xc4,0x5a,0xa3,0x65,0x88,0x13,0xb3,0x94,0x43,0x0c,0x41,0x4c,0xb3,0x2a,0x32,0x38,0x61,0x29,0x8d,0x1e,0xed,0x20,0xc7,0x8a,0x2f,0x07,0x2c,0x56,0x55,0x45,0xf4,0x94,0x5d,0x17,0x4f,0x34,0x4f,0x29,0x3a,0xc9,0x16,0xfa,0xda,0x04,0x4d,0x56,0x09,0x82,0xbb,0xd1,0xf4,0xa3,0xc2,0x3a,0x3f,0x4c,0xe0,0x84,0x26,0xbd,0xf8,0x43,0x51,0x9b,0x95,0xfe,0x84,0x86,0xfa,0x37,0xc2,0x6e,0x16,0xa2,0x89,0x7a,0x58,0xd1,0xec,0x7a,0xbb,0xa1,0x49,0xb2,0x93,0x57,0xba,0x0e,0x6f,0xad,0xa0,0x78,0xeb,0xef,0x60,0xb1,0x90,0x53,0x14,0xb8,0x0c,0xec,0x2e,0x5b,0xdd,0xf4,0xef,0x60,0x38,0x2a,0x5d,0x63,0x67,0x65,0x81,0x3f,0x01,0xa9,0x5b,0x02,0xa7,0x64,0xdd,0x40,0x49,0x8e,0xa2,0xfd,0xba,0xb6,0xd3,0x6c,0x5b,0x3e,0xa2,0xc1,0xd1,0x68,0x38,0xce,0x24,0x13,0xb6,0x3c,0x45,0x5f,0xcd,0x83,0x2a,0xb9,0xf1,0x53,0x09,0x59,0x14,0xe9,0xb4,0x58,0xa5,0x75,0x2e,0x62,0xec,0xf0,0x0d,0x52,0x54,0x23,0xa2,0x14,0x55,0x15,0x37,0x3d,0x54,0xbe,0xa7,0xe6,0x8b,0x1c,0x8b,0x40,0x27,0xa7,0x52,0x9c,0xa7,0x1f,0x63,0xdd,0x4a,0xdf,0xf1,0x1a,0xec,0x50,0x04,0xbd,0x5c,0x1b,0xf0,0x84,0x84,0x04,0x9d,0x0c,0x7b,0xd6,0xa7,0x85,0x36,0xa0,0xce,0xe5,0x74,0xcb,0xfa,0x5f,0x43,0x73,0x97,0x3b,0xcc,0x8a,0x55,0xff,0xe5,0x98,0x7c,0x59,0xdb,0x74,0x61,0x0d,0xc2,0x47,0x83,0xa6,0x47,0x4e,0xd7,0x71,0x95,0xc3,0x61,0x3e,0xa5,0x88,0xae,0x46,0x4d,0x77,0x59,0xd3,0x4a,0xaa,0x3d,0xd3,0x86,0x1c,0x9b,0x0f,0x27,0x5f,0xf8,0x42,0x5b,0xcb,0xd1,0xdd,0x7d,0x83,0x0d,0x55,0x58,0xd5,0x04,0xca,0x6c,0x4e,0x2e,0x3d,0x5d,0x4a,0x4c,0x3c,0xa0,0x5b,0x74,0x43,0x01,0x0a,0xda,0x15,0x0c,0x24,0x95,0xed,0x4b,0x9c,0x09,0xa3,0x53,0x43,0xfa,0x2b,0xfd,0x2b,0x2f,0x73,0xd4,0x68,0x7a,0x19,0x95,0x8e,0xc6,0x01,0x4a,0x29,0xd2,0x5a,0x6a,0xa5,0x33,0xa8,0xaa,0xb7,0x4f,0x08,0x54,0xb2,0x9e,0x5f,0x71,0x4c,0x85,0x8c,0x50,0x80,0x4f,0xad,0x65,0xf5,0x21,0x0a,0xbd,0xaa,0x66,0x1a,0xac,0x72,0x00,0xa6,0xa7,0x85,0xc6,0x43,0xe0,0x9a,0x81,0x02,0x68,0x4f,0xac,0x6c,0x4c,0x93,0x95,0xa7,0xaa,0x15,0x19,0xd9,0xd8,0x9c,0x9a,0x2b,0x09,0xf3,0xf2,0x36,0xa7,0xd3,0xbc,0x69,0x75,0x92,0x32,0xa5,0x57,0x12,0x56,0x83,0x6f,0xbc,0x01,0x24,0xb6,0x44,0x39,0x78,0xd8,0xf0,0xde,0xae,0x67,0xa4,0xea,0xaf,0x1a,0x43,0x30,0x47,0x7e,0x56,0xd4,0x57,0x1f,0x8e,0xac,0xa8,0xfc,0x14,0x18,0xa3,0x5f,0xaf,0x6a,0x74,0x7c,0x4c,0xb1,0x55,0x67,0xe1,0x79,0x25,0x58,0x03,0xf5,0xad,0xe4,0xf1,0x72,0xe9,0x85,0x7a,0x41,0xfa,0x90,0x58,0x5a,0x52,0x4d,0xcf,0x54,0x4d,0x8e,0x12,0xd5,0xda,0x1e,0xc8,0x35,0xf2,0xa1,0xbf,0x18,0x81,0x0b,0x52,0x8f,0xc9,0x34,0xc9,0x5b,0x88,0x39,0x78,0xf8,0x77,0x79,0x53,0xde,0x08,0x1e,0xfe,0xc4,0x16,0x97,0xf6,0xd1,0x87,0xb2,0x68,0x4a,0xb2,0x86,0xbb,0x5c,0x28,0x44,0x7e,0x70,0xf5,0x0b,0x62,0x85,0x53,0x0f,0xe5,0xee,0xd0,0x07,0xfc,0x10,0x35,0x4e,0xda,0xa0,0x69,0x28,0x2e,0xd9,0x33,0x70,0x8d,0x62,0x2e,0xc9,0x8a,0x04,0x29,0x41,0x6f,0xcb,0x78,0x3f,0xc1,0x6d,0x21,0xf6,0x3d,0x85,0x2a,0x19,0x55,0x4b,0x26,0x35,0x53,0x7e,0x31,0x94,0x3c,0x20,0xe0,0x66,0x99,0xde,0x81,0x42,0x8c,0x62,0xab,0x2d,0x7b,0x65,0x7b,0xe6,0x64,0xf4,0x6c,0xc5,0x3b,0xf3,0xa9,0x2c,0x9b,0xba,0xbc,0xdd,0x74,0x97,0x42,0x63,0x51,0x5c,0xd3,0x66,0x02,0x5d,0x6f,0xf3,0x7d,0x36,0x50,0x97,0xaa,0x2f,0xdf,0xe7,0x81,0x0e,0x6c,0xad,0xe6,0x9a,0x47,0x45,0x0a,0x2d,0x80,0x72,0x51,0x8c,0x2b,0xc3,0xae,0x3d,0xc1,0x2b,0xb5,0x86,0x96,0xac,0x5b,0x0e,0x2a,0x22,0x27,0xf7,0x8f,0xaf,0xce,0xaf,0x35,0x85,0x0f,0xef,0x8a,0x1c,0x01,0x01,0x1e,0xc1,0x6e,0x08,0xeb,0x37,0x71,0x37,0xf2,0xf0,0x4e,0x9b,0x1b,0x13,0x4d,0x4f,0x2c,0x8f,0xb4,0x09,0x5c,0x47,0xbe,0x26,0xcf,0x52,0x5a,0xdc,0xab,0xd3,0x58,0x9d,0x9e,0xd7,0x3c,0x28,0x99,0x55,0x45,0x5d,0xa4,0xfb,0x78,0x94,0xd3,0x60,0x34,0x39,0xd0,0x8e,0x5d,0x3a,0x73,0x82,0xdb,0xca,0x94,0x5c,0x4c,0x35,0xa7,0xeb,0x1d,0xa1,0x38,0x7c,0x79,0xbb,0x8b,0xc8,0xb8,0x69,0x29,0x69,0x63,0xf6,0x28,0xc4,0x3a,0x77,0x7e,0x49,0x3a,0xe8,0x66,0xe7,0x46,0x64,0xf7,0xdd,0x74,0xe9,0xa4,0xb4,0xdd,0x8b,0x26,0x28,0x41,0x6f,0xbe,0xe7,0x88,0xfb,0x05,0xae,0x13,0x11,0x17,0x0d,0x9f,0xa0,0x48,0xa9,0xb0,0x1a,0xd7,0x95,0x85,0x54,0x19,0x1b,0xc9,0x6f,0xe6,0x74,0x2f,0x37,0xc2,0x4b,0xba,0x0e,0xa1,0xab,0xa0,0xeb,0x58,0xdc,0x9a,0x58,0xaf,0xf9,0x71,0xd1,0xeb,0x17,0xeb,0xcd,0xb9,0xa5,0x63,0x05,0x80,0x7b,0x6e,0xa2,0xa0,0x07,0xe7,0xa7,0x80,0xae,0x19,0xcf,0xf5,0x3e,0x30,0x4f,0x3d,0x7d,0xcc,0x1c,0xde,0x6e,0x35,0x7a,0xfd,0x32,0xd7,0x3b,0xac,0x76,0xa9,0x5f,0x41,0x90,0x51,0x73,0xfc,0x8e,0xa9,0xd1,0xaf,0x63,0x55,0x10,0xfd,0x0e,0x59,0x9d,0xa6,0xbf,0xd4,0xd4,0xec,0x68,0xaa,0x90,0xda,0xd4,0xed,0x33,0xb1,0x05,0x64,0xb9,0x5e,0xe4,0x6b,0x44,0xd4,0x2c,0x5a,0xe9,0x7f,0xf8,0x86,0x53,0x84 };
    BitReader reader((uint8_t*)&hello, 1);

    /* Use a zlib container as example */

    std::cout << "Parsing zlib container metadata" << "\n";
    zlibMetadata zlibMeta;
    uint32_t zlibAsUint32 = reader.getBits(16);
    memcpy(&zlibMeta,&zlibAsUint32,2);
    uint32_t metaChecksum = ((zlibAsUint32 & 0x00FF) << 8) | ((zlibAsUint32 & 0xFF00) >> 8);
    {
        assert(zlibMeta.method == 8); // only one specified by the spec
        assert(zlibMeta.cinfo <= 7); // window size can be max (2^15)
        assert(metaChecksum % 31 == 0);
        assert(zlibMeta.fdict == 0); // unsupported
        
        printf("zlib Metadata:\n");
        printf("\tCompression Method: %i\n",zlibMeta.method);
        printf("\tlog2(windowSize)-8 : %i\n",zlibMeta.cinfo);
        printf("\tMetadata Checksum: %i\n", metaChecksum % 31);
        printf("\tHas Dictionary: %i\n",zlibMeta.fdict);
        printf("\tCompression Level: %i\n",zlibMeta.flevel);
    }

    DeflateDecoder decoder(reader);
    std::vector<uint8_t> decompressedBytes = decoder.decode();
    
    /* TODO: ADLER32 Follows in stream */

    for (size_t i = 0; i < decompressedBytes.size(); i++) {
        std::cout << decompressedBytes[i] << "-";
    }
    std::cout << "\n";
    return 0;
}
