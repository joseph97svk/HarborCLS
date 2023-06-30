#ifndef THREAD_HPP
#define THREAD_HPP

#include <thread>

class Thread {
 protected:
  std::thread* thread = nullptr;

 public:
  Thread() {
  }

  virtual ~Thread() {
    
  };

  void start() {
    this->thread = new std::thread(&Thread::run, this);
  }

  void waitToFinish() {
    this->thread->join();

    delete this->thread;
    this->thread = nullptr;
  }

 protected:
  virtual void run() = 0;
};

#endif