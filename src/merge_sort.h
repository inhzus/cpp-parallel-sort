//
// Created by suun on 12/27/19.
//

#ifndef CPP_CONCURRENCY_MERGE_SORT_H
#define CPP_CONCURRENCY_MERGE_SORT_H

#include "sort.h"
#include <cstring>

class MergeSort : public Sort {
 private:
  size_t num;

  static void merge(const int *v, size_t ll, size_t rl, size_t rh, int *dst) {
//    if (ll + 1 == rh) return;
    size_t lh = rl, i = 0;
    while (ll < lh && rl < rh) {
      dst[i++] = v[ll] < v[rl] ? v[ll++] : v[rl++];
    }
    while (ll < lh) {
      dst[i++] = v[ll++];
    }
    while (rl < rh) {
      dst[i++] = v[rl++];
    }
  }

  static void sort(int *v, const int *gap, size_t length) {
    int i;
    std::unique_ptr<int[]> src(new int[gap[length - 1]]);
    enum Status { left, mid };
    std::unique_ptr<bool[]> flag(new bool[length]);
    std::fill(flag.get(), flag.get() + length, true);
    while (true) {
      int ll = 0, rl = 0, rh, count = 0;
      Status status = left;
      memcpy(src.get(), v, gap[length - 1] * sizeof(int));
      for (int k = 0; k < length; ++k) {
        if (!flag[k]) continue;
        if (status == left) {
          flag[k] = false;
          status = mid;
          rl = gap[k];
        } else {
          rh = gap[k];
          merge(src.get(), ll, rl, rh, v + ll);
          ++count;
          ll = rh;
          status = left;
        }
      }
      if (count == 0) break;
    }
  }

  static void sort(int *v, size_t length) {
    std::unique_ptr<int[]> dst(new int[length]);
    for (size_t seg = 1; seg < length; seg <<= 1u) {
      for (size_t p = 0; p < length; p += 2 * seg) {
        size_t ll = p, lh = std::min(length, p + seg),
            rl = lh, rh = std::min(length, rl + seg);
        merge(v, ll, lh, rh, dst.get() + ll);
//        size_t i = ll;
//        while (ll < lh && rl < rh) {
//          dst[i++] = v[ll] < v[rl] ? v[ll++] : v[rl++];
//        }
//        while (ll < lh) {
//          dst[i++] = v[ll++];
//        }
//        while (rl < rh) {
//          dst[i++] = v[rl++];
//        }
      }
      memcpy(v, dst.get(), length * sizeof(int));
    }
  }

 public:
  MergeSort() : Sort() {
    num = std::thread::hardware_concurrency();
  }
  void run() override {
    sort(arr.data(), arr.size());
  }
  void par_run() override {
    std::unique_ptr<std::future<void>[]> futures(new std::future<void>[num]);
    // 0. divide evenly
    size_t bucket_num = arr.size() / num + (arr.size() % num > 0);
    std::unique_ptr<int[]> sample(new int[num * num]);
    for (int i = 0; i < num; ++i) {
      futures[i] = std::async(
          std::launch::async,
          [&arr = this->arr, &sample, this, i, bucket_num]() {
            size_t front = bucket_num * i;
            size_t actual_bkt_num = std::min(bucket_num, arr.size() - front);
            // 1. partial sort
            MergeSort::sort(&arr[front], actual_bkt_num);

            size_t split_num =
                actual_bkt_num / num + (actual_bkt_num % num > 0);
            // 2. regular sampling
            for (int j = 0; j < num; j++) {
              sample[num * i + j] = arr[front + std::min(
                  actual_bkt_num, j * split_num)];
            }
          });
    }
    for (int i = 0; i < num; ++i) futures[i].get();
    // 3. sort sample numbers
//    print(arr, num, bucket_num);
    sort(sample.get(), num * num);
    std::unique_ptr<int[]> pivots(new int[num - 1]);
    // 4. select pivots
    for (int i = 1; i < num; ++i) {
      pivots[i - 1] = sample[i * num];
    }
//    printf("pivots:\n");
//    print(pivots, 7);
    // record step 5 result, num * (num + 1)
//    std::unique_ptr<std::unique_ptr<int[]>[]> marker(
//        new std::unique_ptr<int[]>[num]);
//    std::vector<std::vector<int>> marker(num, std::vector<int>(num + 1, 0));
    std::unique_ptr<std::unique_ptr<int[]>[]> marker(
        new std::unique_ptr<int[]>[num]);
    for (int p = 0; p < num; ++p) {
      futures[p] = std::async(
          std::launch::async,
          [&arr = this->arr, &pivots, &marker,
              num = this->num, p, bucket_num]() {
            marker[p] = std::make_unique<int[]>(num + 1);
            size_t front = bucket_num * p;
            size_t actual_bkt_num = std::min(
                bucket_num, arr.size() - bucket_num * p);
            // 5. divide by pivots
            // i: element index in array, j: pivot range index
            int j = 0;
            marker[p][j++] = front;
            for (; arr[front] > pivots[j - 1]; ++j) {
              marker[p][j] = front;
            }
            for (int i = front; i < front + actual_bkt_num && j < num; ++i) {
              while (arr[i] > pivots[j - 1] && j < num) {
                marker[p][j++] = i;
              }
            }
            for (; j <= num; ++j)
              marker[p][j] = front + actual_bkt_num;
          }
      );
    }
    for (int p = 0; p < num; ++p) futures[p].get();
//    for (auto &v : marker) {
//      print(v);
//    }
    // record marker borders
    std::unique_ptr<int[]> border(new int[num + 1]);
    border[0] = 0;
    for (int j = 1, count = 0; j < num; ++j) {
      for (int i = 0; i < num; ++i) {
        count += marker[i][j] - marker[i][j - 1];
      }
      border[j] = count;
    }
    border[num] = arr.size();
//    printf("border:\n");
//    print(border, num + 1);
    std::vector<int> src(arr.size());
    src.swap(arr);
//    std::unique_ptr<int[]> src(new int[arr.size()]);
    for (int p = 0; p < num; ++p) {
      futures[p] = std::async(
          std::launch::async,
          [&arr = this->arr, &border, &src, &marker,
              num = this->num, p]
              () {
            std::unique_ptr<int[]> gap(new int[num]);
            int k = border[p];
            for (int i = 0; i < num; ++i) {
              int length = marker[i][p + 1] - marker[i][p];
//              printf("ptr: %ld, tmp: %ld, length: %d, "
//                     "%d: %d, %d: %d\n", ptr, tmp, length, p,
//                     marker[i][p], p + 1, marker[i][p + 1]);
              for (int j = marker[i][p]; j < marker[i][p + 1]; ++j) {
//                src[k++] = arr[j];
                arr[k++] = src[j];
              }
              gap[i] = k - border[p];
            }
//            sort(&arr[border[p]], border[p + 1] - border[p]);
//            if (p == 0) {
//              print(&arr[border[p]], border[p + 1] - border[p]);
//            }
            sort(&arr[border[p]], gap.get(), num);
//            if (p == 0) {
//              print(gap.get(), num);
//              print(&arr[border[p]], border[p + 1] - border[p]);
//            }
          }
      );
    }
//    print(flag);
//    printf("count: %d\n", count.load());
    for (int p = 0; p < num; ++p) futures[p].get();
//    printf("src:\n");
//    print(arr, border, num + 1);
  }
};

#endif //CPP_CONCURRENCY_MERGE_SORT_H
