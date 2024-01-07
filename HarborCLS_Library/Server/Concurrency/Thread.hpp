#ifndef THREAD_HPP
#define THREAD_HPP

#include <thread>
#include <memory>

namespace HarborCLS {

  class Thread {
  protected:
    std::shared_ptr<std::jthread> thread;

  public:
    Thread() = default;

    virtual ~Thread() = default;

    void start() {
      this->thread = std::make_shared<std::jthread>(&Thread::run, this);
    }

    void waitToFinish() {
      this->thread->join();
    }

  protected:
    virtual void run() = 0;
  };
}
#endif