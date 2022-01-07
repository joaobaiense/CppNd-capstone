#ifndef UTILS_INCLUDE_UTILS_QUEUE_HPP_
#define UTILS_INCLUDE_UTILS_QUEUE_HPP_

#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>

template <class T>
class MessageQueue {
 public:
  // void send(T &&msg);
  // T receive();

  T receive() {
    std::unique_lock<std::mutex> mut(m_mut);

    // wait for a new message
    m_cond.wait(mut, [this] { return !m_queue.empty(); });

    // std::cout << "Message received" << std::endl;

    // move the message
    T tmp = std::move(m_queue.front());

    // pop the message
    m_queue.pop_front();

    // unlock
    mut.unlock();

    return tmp;
  }

  void send(T &&msg) {
    // std::cout << "Sending message" << std::endl;

    std::unique_lock<std::mutex> mut(m_mut);

    // push to the queue
    m_queue.emplace_back(std::move(msg));

    // unlock
    mut.unlock();

    // notify
    m_cond.notify_one();
  }

 private:
  std::deque<T> m_queue;
  std::condition_variable m_cond;
  std::mutex m_mut;
};

#endif  // UTILS_INCLUDE_UTILS_QUEUE_HPP_