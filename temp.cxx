#ifndef BOUNDEDBUFFER_H
#define BOUNDEDBUFFER_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

// T must implement operator <<

template< typename T>
class BoundedBuffer
{
public:
    BoundedBuffer(size_t size) :m_size(size), m_front(0), m_rear(-1), m_count(0)
    {
        m_queue = new T[m_size];
    }

    void enqueue( const T& obj )
    {
        std::unique_lock<std::mutex> writerLock(m_mutex);

        while ( m_size == m_count )
        {
            writers.wait(writerLock);
        }
        m_rear = (m_rear + 1) % m_size;
        m_queue[m_rear] = obj;
        ++m_count;
        std::cout << std::this_thread::get_id() << " has Enqueued Object =" << obj << std::endl;
        readers.notify_all();
    }

    void dequeue( T& obj )
    {
        std::unique_lock<std::mutex> readerLock(m_mutex);
        while (0 == m_count)
        {
            readers.wait(readerLock);
        }
        obj = m_queue[m_front];
        m_front = (m_front + 1) % m_size;
        --m_count;
        std::cout << std::this_thread::get_id() << " has Dequeued Object =" << obj << std::endl;
        writers.notify_all();
    }

    ~BoundedBuffer()
    {
        if (m_queue)
        {
            delete[] m_queue;
        }
    }

private:
    size_t m_size;
    T * m_queue;
    int m_front, m_rear, m_count;
    std::mutex m_mutex;
    std::condition_variable readers;
    std::condition_variable writers;
};

#endif