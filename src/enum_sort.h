//
// Created by suun on 12/27/19.
//

#ifndef CPP_CONCURRENCY_ENUM_SORT_H
#define CPP_CONCURRENCY_ENUM_SORT_H

#include "sort.h"
#include <algorithm>
#include <future>
#include <thread>

class EnumSort : public Sort {
 private:
  decltype(arr) src;
  size_t num;
  static int rank(const decltype(arr) &src, int v) {
    int count = 0;
    for (int value : src) {
      if (v > value) {
        ++count;
      }
    }
    return count;
  }
 public:
  EnumSort() : Sort(), src(arr.size()) {
    src.swap(arr);
    num = std::thread::hardware_concurrency();
  }
  void run() override {
    for (int value : src) {
      int k = rank(src, value);
      arr[k] = value;
    }
  }
  void par_run() override {
    std::unique_ptr<std::future<void>[]> futures(new std::future<void>[num]);
    size_t bucket_num = arr.size() / num + (arr.size() % num > 0);
    for (int p = 0; p < num; ++p) {
      futures[p] = std::async(
          std::launch::async,
          [&arr = this->arr, &src = this->src,
              bucket_num, num = this->num, p]() {
            for (size_t i = p * bucket_num;
                 i < arr.size() && i < (p + 1) * bucket_num;
                 ++i) {
              arr[rank(src, src[i])] = src[i];
            }
          });
    }
    for (int k = 0; k < num; k++) {
      futures[k].get();
    }
  }
};


#endif //CPP_CONCURRENCY_ENUM_SORT_H
