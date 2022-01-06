
#include "include/Utils/Queue.hpp"


// template <typename T>
// T MessageQueue<T>::receive()
// {

//     std::unique_lock<std::mutex> mut(m_mut);

//     // wait for a new message
//     m_cond.wait(mut, [this]{return !m_queue.empty();});

//     // move the message
//     T tmp = std::move(m_queue.front());
//     m_queue.pop_front();

//     // pop the message 
//     m_queue.pop_front();

//     // unlock 
//     mut.unlock();

//     return tmp;
// }

// template <typename T>
// void MessageQueue<T>::send(T &&msg)
// {

//     std::unique_lock<std::mutex> mut(m_mut);

//     // push to the queue 
//     m_queue.emplace_back(std::move(msg));

//     // unlock 
//     mut.unlock();

//     // notify 
//     m_cond.notify_one();

// }