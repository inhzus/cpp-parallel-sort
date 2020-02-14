//
// Created by suun on 12/27/19.
//

#ifndef CPP_CONCURRENCY_SORT_H
#define CPP_CONCURRENCY_SORT_H

#define BUCKET 1
#define SOURCE_LENGTH 30000
#define LENGTH SOURCE_LENGTH * BUCKET

#include <array>
#include <memory>
#include <vector>
#include <cassert>
#include <cstdio>
class Sort {
 protected:
  std::vector<int> arr;
  static void print(const std::vector<int> &v) {
    for (int value : v) {
      printf("%d ", value);
    }
    printf("\n");
  }
  template<class C>
  static void print(const C &ptr, int n) {
    for (int i = 0; i < n; ++i) {
      printf("%d ", ptr[i]);
    }
    printf("\n");
  }
  static void print(const std::vector<int> &v, int m, int n) {
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
        printf("%d ", v[j + i * n]);
      }
      printf("\n");
    }
  }
  template<typename C>
  static void print(const C &v,
                    const std::unique_ptr<int[]> &d, int n) {
    for (int j = 1; j < n; ++j) {
      for (int i = d[j - 1]; i < d[j]; ++i) {
        printf("%d ", v[i]);
      }
      printf("\n");
    }
  }
  template<typename T>
  static void print(const std::vector<std::vector<T>> &v) {
    for (auto &line : v) {
      for (T value: line) {
        printf("%d ", value);
      }
      printf("\n");
    }
  }
 public:
  Sort() : arr(LENGTH) { // NOLINT
    FILE *file = fopen("../src/random.txt", "r");
    for (int i = 0; i < SOURCE_LENGTH; ++i) {
      int value;
      fscanf(file, "%d", &value); // NOLINT(cert-err34-c)
      for (int j = 0; j < BUCKET; ++j) {
        arr[i + SOURCE_LENGTH * j] = value;
      }
    }
    fclose(file);
  }

  virtual void run() = 0;
  virtual void par_run() = 0;

  void check() const {
    for (size_t i = 0; i < arr.size() - 1; i++) {
      if (arr[i] > arr[i + 1]) {
        printf("check failed\n");
        exit(-1);
      }
    }
  }

  void print() const {
    print(arr);
  }

  void write_file(size_t idx) const {
    char filename[16];
    sprintf(filename, "order%zu.txt", idx);
    FILE *file = fopen(filename, "w");
    for (auto v : arr) {
      fprintf(file, "%d ", v);
    }
    fclose(file);
  }
};

#endif //CPP_CONCURRENCY_SORT_H
