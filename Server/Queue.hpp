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
  Semaphore canConsume;
  std::queue<dataType> queue;

 public:
  Queue() {
    sem_init(&this->canConsume, 0, 0);
  }

  ~Queue() {
    sem_destroy(&canConsume);
  }

  void push(const dataType& data) {
    this->canAccess.lock();
    this->queue.push(data);
    this->canAccess.unlock();
    sem_post(&this->canConsume);
  }

  dataType pop() {
    sem_wait(&this->canConsume);
    this->canAccess.lock();
    dataType& resultData = this->queue.front();
    this->queue.pop();
    this->canAccess.unlock();
    return resultData;
  }
};

#endif