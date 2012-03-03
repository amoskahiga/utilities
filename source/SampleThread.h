#ifndef SAMPLETHREAD_H
#define SAMPLETHREAD_H

#include <cstdio>
#include <boost/circular_buffer.hpp>
#include <QMutex>
#include <QThread>

/**
 * Class to sample data from a file stream and store it in a supplied buffer.
 * NOTE: The boost::circular_buffer that is used is not thread safe; the supplied mutex is used to control access.
 */
class SampleThread : public QThread
{
    Q_OBJECT
public:
    SampleThread(boost::circular_buffer<unsigned char>& buffer, QMutex& mutex, QObject *parent = 0);
    void setFile(FILE* file);
    void stop();

protected:
    void run();

private:
    boost::circular_buffer<unsigned char>& m_buffer;
    FILE* m_file;
    bool m_stopped;
    QMutex& m_mutex;
};

#endif // SAMPLETHREAD_H
