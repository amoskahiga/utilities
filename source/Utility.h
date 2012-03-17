#ifndef UTILITY_H
#define UTILITY_H

#include <QBitArray>
#include <QPointF>
#include <QVector>

/**
 * Helper functions
 */
class Utility
{
public:
    static QBitArray toBits(const char* input, size_t size);
    static QBitArray toBits(const QVector<double>& input);
    static QVector<double> getYValues(const QVector<QPointF>& input);
    static QVector<char> toChars(const QBitArray& input);
};

#endif // UTILITY_H
