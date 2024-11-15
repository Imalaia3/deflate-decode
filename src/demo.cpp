/* A simple gzip DEFLATE decompression command line
 * tool to demonstrate the capabilities of this library.
 * In the future, gzip compression may be supported.
 */
#include <iostream>
#include "deflate/deflate.h"
#include "deflate/bitreader.h"
#include <string>

struct GzipCommonHeader {
    uint8_t magic[2];
    uint8_t method;
    // FLG Byte
    union flagsUnion {
        uint8_t data;
        struct _bits {
            bool FTEXT: 1;
            bool FHCRC: 1;
            bool FEXTRA: 1;
            bool FNAME: 1;
            bool FCOMMENT: 1;
            bool reserved: 3;
        } __attribute__((packed));
        _bits bits;
    };
    flagsUnion flags;
    uint32_t time; // Modification time
    uint8_t extraFlags;
    uint8_t osType;
} __attribute__((packed));

std::string readStringFromFile(FILE* stream, uint8_t terminator = '\0') {
    assert(stream);
    std::string dst = "";
    uint8_t b;

    fread(&b, 1, 1, stream);
    while (b != terminator) {
        dst += b;
        fread(&b, 1, 1, stream);
    }
    return dst;
}

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        printf("Syntax: ./demo <gzip file>\n");
        return EXIT_FAILURE;
    }

    FILE* fp = fopen(argv[1], "rb");
    if (!fp) {
        printf("Failed to open \"%s\"!\n", argv[1]);
        return EXIT_FAILURE;
    }

    GzipCommonHeader header{};
    assert(sizeof(header) == 10);
    fread(&header, 10, 1, fp);

    if (header.magic[0] != 0x1f && header.magic[1] != 0x8b) {
        printf("Invalid header!\n");
        fclose(fp);
        return EXIT_FAILURE;
    }
    if (header.method != 8) {
        printf("File does not use the DEFLATE compression method!\n");
        fclose(fp);
        return EXIT_FAILURE;
    }

    printf("Header flags: \n");
    printf("\tFTEXT: %i\n", header.flags.bits.FTEXT);
    printf("\tFHCRC: %i\n", header.flags.bits.FHCRC);
    printf("\tFEXTRA: %i\n", header.flags.bits.FEXTRA);
    printf("\tFNAME: %i\n", header.flags.bits.FNAME);
    printf("\tFCOMMENT: %i\n", header.flags.bits.FCOMMENT);
    printf("\treserved: %i\n", header.flags.bits.reserved);

    std::string originalFileName = "";
    std::string fileComment = "No comment available";
    uint16_t headerCrc16 = 0;

    if (header.flags.bits.FNAME) {
        originalFileName = readStringFromFile(fp);
    }
    if (header.flags.bits.FCOMMENT) {
        fileComment = readStringFromFile(fp);
    }
    
    if (originalFileName == "") {
        printf("No original filename present. Default is output.bin\n");
        originalFileName = "output.bin";
    }
    printf("Original Filename: %s\n", originalFileName.c_str());
    printf("Comment: %s\n", fileComment.c_str());

    if (header.flags.bits.FHCRC) {
        fread(&headerCrc16, 2, 1, fp);
    }

    // this is not recommended. The correct way is to read until a deflate END block is reached, but 
    // BitReader does not (yet) support that.
    uint64_t begin = ftell(fp);
    fseek(fp, 0, SEEK_END);
    uint64_t end = ftell(fp);
    fseek(fp, begin, SEEK_SET);
    uint64_t payloadSize = end-begin+1;

    printf("Allocating a %li byte buffer...\n", payloadSize);
    uint8_t* payload = new uint8_t[payloadSize];
    fread(payload, payloadSize, 1, fp);
    
    BitReader r(payload, 1);
    DeflateDecoder decoder(r);
    auto result = decoder.decode();
    
    printf("Writing result to %s...\n", originalFileName.c_str());
    FILE* out = fopen(originalFileName.c_str(), "wb");
    for (auto &&byte : result)
        fwrite(&byte, 1, 1, out);
    fclose(out);

    delete[] payload;
    fclose(fp);
    return EXIT_SUCCESS;
}
