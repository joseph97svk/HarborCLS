#ifndef THREAD_HPP
#define THREAD_HPP

#include <thread>

class Thread {
 protected:
  std::unique_ptr<std::thread> thread;

 public:
  Thread() = default;

  virtual ~Thread() = default;

  void start() {
    this->thread = std::make_unique<std::thread>(&Thread::run, this);
  }

  void waitToFinish() {
    this->thread->join();
  }

 protected:
  virtual void run() = 0;
};

#endif