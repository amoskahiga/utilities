#ifndef SAMPLETHREAD_H
#define SAMPLETHREAD_H

#include <cstdio>
#include <boost/circular_buffer.hpp>
#include <QThread>

/**
 * Class to sample data from a file stream and store it in a supplied buffer.
 * NOTE: The boost::circular_buffer that is used is not thread safe and care must be taken when using this class.
 */
class SampleThread : public QThread
{
    Q_OBJECT
public:
    SampleThread(boost::circular_buffer<char>& buffer, QObject *parent = 0);
    void setFile(FILE* file);
    void stop();

protected:
    void run();

private:
    boost::circular_buffer<char>& m_buffer;
    FILE* m_file;
    bool m_stopped;
};

#endif // SAMPLETHREAD_H
