#ifndef UTILS_INCLUDE_UTILS_QUEUE_HPP_
#define UTILS_INCLUDE_UTILS_QUEUE_HPP_

#include <mutex>
#include <deque>
#include <condition_variable>

template <class T>
class MessageQueue
{
public:
    void send(T &&msg);
    T receive();

private:

    std::deque<T> m_queue;
    std::condition_variable m_cond;
    std::mutex m_mut;
    
};

#endif // UTILS_INCLUDE_UTILS_QUEUE_HPP_