
#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  explicit ThreadPool(size_t numThreads);
  ~ThreadPool();

  [[nodiscard]] size_t getThreadCount() const { return workers.size(); }

  void resize(size_t newSize);

  template <class F, class... Args>
  auto enqueue(F&& f, Args&&... args)
      -> std::future<std::invoke_result_t<F, Args...>>;

 private:
  void stopAllWorkers();
  void startWorkers(size_t numWorkers);

  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;
  std::mutex queueMutex;
  std::condition_variable condition;
  bool stop{false};
};

template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<std::invoke_result_t<F, Args...>> {
  using return_type = std::invoke_result_t<F, Args...>;

  auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task->get_future();
  {
    std::unique_lock lock(queueMutex);
    if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");

    tasks.emplace([task] { (*task)(); });
  }

  condition.notify_one();
  return res;
}