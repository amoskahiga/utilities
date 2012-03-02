#ifndef UTILITY_H
#define UTILITY_H

#include <QBitArray>
#include <QVector>

/**
 * Helper functions
 */
class Utility
{
public:
    static QBitArray toBits(const char* input, size_t size);
};

#endif // UTILITY_H
