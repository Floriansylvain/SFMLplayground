
#include "ThreadPool.hpp"

ThreadPool::ThreadPool(const size_t numThreads) : stop(false) {
  startWorkers(numThreads);
}

ThreadPool::~ThreadPool() { stopAllWorkers(); }

void ThreadPool::resize(const size_t newSize) {
  if (newSize == workers.size()) return;

  stopAllWorkers();
  workers.clear();
  stop = false;
  startWorkers(newSize);
}

void ThreadPool::stopAllWorkers() {
  {
    std::unique_lock lock(queueMutex);
    stop = true;
  }
  condition.notify_all();
  for (auto& worker : workers) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

void ThreadPool::startWorkers(const size_t numWorkers) {
  for (size_t i = 0; i < numWorkers; ++i) {
    workers.emplace_back([this] {
      while (true) {
        std::function<void()> task;
        {
          std::unique_lock lock(this->queueMutex);
          this->condition.wait(
              lock, [this] { return this->stop || !this->tasks.empty(); });

          if (this->stop && this->tasks.empty()) return;

          task = std::move(this->tasks.front());
          this->tasks.pop();
        }
        task();
      }
    });
  }
}
