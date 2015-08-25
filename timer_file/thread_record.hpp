#include <thread>
#include <chrono>
#include "timer_file.hpp"
#include "readerwriterqueue.h"
#include <string>
#include <unistd.h>

class CThread
{
public:
    CThread(){}

    int open()
    {
        m_thread = std::thread(std::thread(&CThread::th_fun, this));
        return 0;
    }

    void th_fun()
    {
        this->svc();
    }

    void stop()
    {
        m_running_flag = false;
    }

    bool is_stopped()
    {
        return !m_running_flag;
    }

    virtual int svc() = 0;

    ~CThread()
    {
        m_thread.join();
    }

protected:
    bool m_running_flag = true;
    std::thread m_thread;
};

#define sleep_ms(a) std::this_thread::sleep_for(std::chrono::milliseconds(a))

template <class T>
class CRecordThread : public CThread
{
public:
    typedef moodycamel::ReaderWriterQueue<T> QUEUE;

public:
    CRecordThread() : m_delete_queue(false)
    {
    }

    int put_msg(T  &msg )
    {
        m_lock_free_queue->enqueue(msg); // Will allocate memory if the queue is full
        return 0;
    }

    int open(CTimerFileInfo &time_file_info, QUEUE *queue = nullptr)
    {
        if (queue != nullptr)
        {
            m_delete_queue = false;
            m_lock_free_queue = queue;
        }
        else
        {
            m_lock_free_queue = new QUEUE(1000000); // 100w
            m_delete_queue = true;
        }

        m_timer_file = new CTimerFile(time_file_info);
        return CThread::open();
    }

    virtual int svc()
    {
        while(1)
        {
            std::string str;
            while (m_lock_free_queue->try_dequeue(str)) // Returns false if the queue was empty
            {
                // write to file
                m_timer_file->write_data(str.c_str(), str.length());
            }

            if ( !m_running_flag )
            {
                break;
            }

            sleep_ms(5); // 5ms
        }


        if (m_delete_queue && (m_lock_free_queue != nullptr) )
        {
            delete m_lock_free_queue;
            m_lock_free_queue = nullptr;
        }

        if (m_timer_file != nullptr)
        {
             delete m_timer_file;
             m_timer_file = nullptr;
        }

        return 0;
    }

protected:
    //std::unique_ptr<CTimerFile> *m_timer_file;
    CTimerFile *m_timer_file = nullptr;

    bool       m_delete_queue = false;
    QUEUE      *m_lock_free_queue = nullptr;
};
