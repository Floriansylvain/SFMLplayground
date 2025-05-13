#include <thread>

namespace ThreadUtils {
inline size_t calculateSafeWorkerThreads(const size_t reserve = 2) {
  const size_t total = std::thread::hardware_concurrency();
  return total > reserve ? total - reserve : 1;
}
}  // namespace ThreadUtils
