#include "SampleThread.h"

/**
 * Constructor
 *
 * @param buffer that stores the sampled data
 * @param file to sample from
 */
SampleThread::SampleThread(boost::circular_buffer<char>& buffer, QObject *parent) :
    QThread(parent),
    m_buffer(buffer),
    m_file(0),
    m_stopped(true)
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
        if ((c = fgetc(m_file)) != EOF) {
            m_buffer.push_back(c);
        }
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
