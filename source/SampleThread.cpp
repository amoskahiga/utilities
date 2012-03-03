#include "SampleThread.h"

/**
 * Constructor
 *
 * @param buffer that stores the sampled data
 * @param mutex object used to synchronize rea/write access
 */
SampleThread::SampleThread(boost::circular_buffer<unsigned char>& buffer, QMutex& mutex, QObject *parent) :
    QThread(parent),
    m_buffer(buffer),
    m_file(0),
    m_stopped(true),
    m_mutex(mutex)
{
}

/**
 * Sample the file continously and stores the results in the internal buffer
 */
void SampleThread::run(){

    if (!m_file)
    {
        throw "Error: File handle invalid!";
    }

    m_stopped = false;

    // Sample data from our stream
    int c = 0;  // Note: This need to be bigger than a char so that we can differentiate 0xFF from EOF (-1)
    while(!m_stopped)
    {
        // Sample data continously while it's available
        while ((c = fgetc(m_file)) != EOF && !m_stopped) {
            m_mutex.lock();
            m_buffer.push_back(static_cast<unsigned char>(c));
            m_mutex.unlock();
        }

        // Wait a little for new data to arrive. We sleep to avoid hoarding the CPU unneccessarily.
        sleep(1);
    }
}

/**
 * Set file used to stream data
 *
 * @param file that will be read from when the thread is started. This may be a normal file or pipe.
 */
void SampleThread::setFile(FILE* file)
{
    m_file = file;
}

/**
 * Set the thread to stop sampling and exit the thread when possible
 */
void SampleThread::stop()
{
    m_stopped = true;
}
