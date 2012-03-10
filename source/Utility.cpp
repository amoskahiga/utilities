#include "Utility.h"

/**
 * Convert the supplied characters to a bit array
 *
 * @param input buffer to be converted
 * @param size of input buffer
 */
QBitArray Utility::toBits(const char* input, size_t size)
{
    const unsigned int byteSize = 8;
    QBitArray bits(size * byteSize, false);

    for (size_t i = 0; i < size; ++i) {
        unsigned char c = input[i];
        unsigned int offset = i * byteSize;

        for (unsigned int j = 0; j < byteSize; ++j) {
            volatile int i = (c >> (byteSize - 1 - j)) & 1;
            bits[offset + j] = (c >> (byteSize - 1 - j)) & 1;
        }
    }
    return bits;
}
