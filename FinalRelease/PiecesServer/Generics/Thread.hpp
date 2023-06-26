#ifndef THREAD_HPP
#define THREAD_HPP

#include <thread>

class Thread {
  std::thread* thread;

 public:
  Thread()
  : thread(nullptr){
  }

  ~Thread() {
    if (this->thread == nullptr) {
      return;
    }

    delete this->thread;
  }

  void start() {
    this->thread = new std::thread(&Thread::run, this);
  }

  virtual void run();

  void waitToFinish() {
    this->thread->join();

    delete this->thread;
    this->thread = nullptr;
  }
};

#endif