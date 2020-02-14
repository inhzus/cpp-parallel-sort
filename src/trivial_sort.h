//
// Created by suun on 12/27/19.
//

#ifndef CPP_CONCURRENCY_TRIVIAL_SORT_H
#define CPP_CONCURRENCY_TRIVIAL_SORT_H

#include "sort.h"
#include <algorithm>

class TrivialSort : public Sort {
 public:
  void run() override {
    std::sort(arr.begin(), arr.end());
  }
  void par_run() override {
    std::sort(arr.begin(), arr.end());
  }
};

#endif //CPP_CONCURRENCY_TRIVIAL_SORT_H
