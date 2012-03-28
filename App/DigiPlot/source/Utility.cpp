#include <cmath>

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
            bits[offset + j] = (c >> (byteSize - 1 - j)) & 1;
        }
    }
    return bits;
}

/**
 * Convert the supplied double array to a bit array. Each input value is converted to a bit: 1 if input value > 0,
 * and 0 otherwise.
 *
 * @param input double values
 * @return bit array
 */
QBitArray Utility::toBits(const QVector<double>& input)
{
    QBitArray bits(input.size(), false);

    for (int i = 0; i < input.size(); ++i) {
        bits[i] = input[i];
    }
    return bits;
}

/**
 * Convert the supplied double array to a bit array. Each input value is converted to a bit: 1 if input value > 0,
 * and 0 otherwise.
 *
 * @param input double values
 * @return bit array
 */
QVector<double> Utility::getYValues(const QVector<QPointF>& input)
{
    QVector<double> output(input.size(), 0);

    for (int i = 0; i < input.size(); ++i) {
        output[i] = input[i].y();
    }
    return output;
}

/**
 * Convert the supplied bit array to a char array. Each 8 bits are converted to a char
 *
 * @param input bit values
 * @return char array
 */
QVector<char> Utility::toChars(const QBitArray& input)
{
    const unsigned int byteSize = 8;
    const unsigned int charSize = ceil((double)input.size() / byteSize);
    QVector<char> output(charSize, 0);

    for (int i = 0; i < output.size(); ++i) {
        unsigned char c = 0;
        unsigned int offset = i * byteSize;

        for (unsigned int j = 0; j < byteSize && (offset + j) < input.size(); ++j) {
            c |= input[offset + j] << (byteSize - 1 - j);
        }
        output[i] = c;
    }
    return output;
}
