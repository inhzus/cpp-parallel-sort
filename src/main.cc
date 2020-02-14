//
// Created by suun on 12/27/19.
//

#include "enum_sort.h"
#include "merge_sort.h"
#include "quick_sort.h"
#include "trivial_sort.h"
#include <chrono>
#include <functional>
#include <memory>

#define REPEAT 20

enum Policy {
  par, ser
};

static size_t idx = 0;

template<typename DerivedSort>
void benchmark(Policy policy, const char *method, std::unique_ptr<Sort> &sort) {
  std::function<void()> f;
  if (policy == par) {
    f = [&sort]() { sort->par_run(); };
  } else if (policy == ser) {
    f = [&sort]() { sort->run(); };
  }
  unsigned long long sum = 0;
  using namespace std::chrono;
  for (int i = 0; i < REPEAT; ++i) {
    sort = std::make_unique<DerivedSort>();
    auto start = high_resolution_clock::now();
    f();
    auto stop = high_resolution_clock::now();
    sum += duration_cast<nanoseconds>(stop - start).count();
    sort->check();
    sort->write_file(idx);
  }
  printf("%s costs: %11llu ns\n", method, sum / REPEAT);
  idx++;
}

int main() {
  std::unique_ptr<Sort> sort;

//  sort = std::make_unique<TrivialSort>();
//  timeit(std::bind(&Sort::run, sort.get()));
//  sort->check();
  benchmark<TrivialSort>(ser, "std  algorithm sort", sort);
  benchmark<QuickSort>(ser, "trivial  quick sort", sort);
  benchmark<QuickSort>(par, "parallel quick sort", sort);
  benchmark<MergeSort>(ser, "trivial  merge sort", sort);
  benchmark<MergeSort>(par, "parallel merge sort", sort);
#if BUCKET == 1
  benchmark<EnumSort>(ser, "trivial  enum  sort", sort);
  benchmark<EnumSort>(par, "parallel enum  sort", sort);
#endif

//  sort = std::make_unique<QuickSort>();
//  timeit(std::bind(&Sort::run, sort.get()));
//  sort->check();
//
//  sort = std::make_unique<QuickSort>();
//  timeit(std::bind(&Sort::par_run, sort.get()));
//  sort->check();
//
//#if BUCKET == 1
//  sort = std::make_unique<EnumSort>();
//  timeit(std::bind(&Sort::run, sort.get()));
//  sort->check();
//
//  sort = std::make_unique<EnumSort>();
//  timeit(std::bind(&Sort::par_run, sort.get()));
//  sort->check();
//#endif
//
//  sort = std::make_unique<MergeSort>();
//  timeit(std::bind(&Sort::run, sort.get()));
//  sort->check();
//
//  sort = std::make_unique<MergeSort>();
//  timeit(std::bind(&Sort::par_run, sort.get()));
//  sort->check();
  return 0;
}
