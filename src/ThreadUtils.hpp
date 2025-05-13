#pragma once
#include <thread>

namespace ThreadUtils {
inline size_t calculateSafeWorkerThreads(const size_t reserve = 2) {
  const size_t total = std::thread::hardware_concurrency();
  return total > reserve ? total - reserve : 1;
}

inline size_t calculateOptimalThreads(const size_t objectCount,
                                      const size_t minObjectsPerThread = 100) {
  const size_t maxThreads = calculateSafeWorkerThreads();
  const size_t optimalThreads =
      std::max(static_cast<size_t>(1), objectCount / minObjectsPerThread);
  return std::min(maxThreads, optimalThreads);
}

}  // namespace ThreadUtils
