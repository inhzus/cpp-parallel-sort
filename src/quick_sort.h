//
// Created by suun on 12/27/19.
//

#ifndef CPP_CONCURRENCY_QUICK_SORT_H
#define CPP_CONCURRENCY_QUICK_SORT_H

#include "sort.h"
#include <functional>
#include <future>
//#include <unistd.h>
//#include <sys/syscall.h>

class QuickSort : public Sort {
 private:

  size_t num;
  size_t p;
  static constexpr unsigned threshold = 4000;

  int partition(int low, int high) {

    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j < high; ++j) {
      if (arr[j] < pivot) {
        ++i;
        std::swap(arr[i], arr[j]);
      }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
  }

  void sort(int low, int high) {
    if (low >= high) return;
    int k = partition(low, high);
    sort(low, k - 1);
    sort(k + 1, high);
  }

  void par_sort(int low, int high, size_t depth = 1) {
//    printf("thread: %ld\n", syscall(SYS_gettid));
    if (low >= high) return;
    int k = partition(low, high);
    auto left_task = std::bind(
        &QuickSort::par_sort, this, low, k - 1, depth << 1u);
    std::future<void> left, right;
    if (depth <= num) {
      right = std::async(
          std::launch::async,
          std::bind(
              &QuickSort::par_sort, this, k + 1, high, depth << 1u));
      par_sort(low, k - 1, depth << 1u);
    } else {
      sort(low, k - 1);
      sort(k + 1, high);
    }
    if (right.valid()) {
      right.get();
    }
//    printf("thread: %ld end\n", syscall(SYS_gettid));
  }

 public:
  QuickSort() : Sort(), p() {
    num = std::thread::hardware_concurrency();
  }

  void run() override {
    sort(0, static_cast<int>(arr.size() - 1));
  }
  void par_run() override {
    par_sort(0, static_cast<int>(arr.size() - 1));
  }
};

#endif //CPP_CONCURRENCY_QUICK_SORT_H
