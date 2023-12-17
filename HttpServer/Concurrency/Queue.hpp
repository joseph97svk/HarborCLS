#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <mutex>
#include <queue>
#include <pthread.h>
#include <semaphore.h>

typedef sem_t Semaphore;

template <typename dataType>
class Queue {
  std::mutex canAccess;
  Semaphore canConsume{};
  std::queue<dataType> queue;

 public:
  /**
   * Constructs a Queue object.
   */
  Queue() {
    sem_init(&this->canConsume, 0, 0); // Initialize the semaphore for consumer blocking
  }

  /**
   * Destructs a Queue object.
   */
  ~Queue() {
    sem_destroy(&canConsume); // Destroy the semaphore
  }

  /**
   * Pushes an element to the _queue.
   * @param data The element to be pushed to the _queue.
   */
  void push(const dataType& data) {
    this->canAccess.lock(); // Acquire the lock to access the _queue
    this->queue.push(data); // Push the data to the _queue
    this->canAccess.unlock(); // Release the lock
    sem_post(&this->canConsume); // Signal the semaphore to unblock consumers
  }

  /**
   * Pops an element from the _queue.
   * If the _queue is empty, the calling thread will block until an element becomes available.
   * @return The element popped from the _queue.
   */
  dataType pop() {
    sem_wait(&this->canConsume); // Wait until there is an element available in the _queue
    this->canAccess.lock(); // Acquire the lock to access the _queue
    dataType resultData = this->queue.front(); // Get the front element of the _queue
    this->queue.pop(); // Remove the front element from the _queue
    this->canAccess.unlock(); // Release the lock
    return resultData; // Return the popped element
  }
};

#endif