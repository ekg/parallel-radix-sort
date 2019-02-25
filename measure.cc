#include "parallel_radix_sort.hpp"

#include <cstdio>
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <algorithm>

const int DEFAULT_N = 200000000;
const int TRIAL = 3;

struct __bench__ {
  double start;
  char msg[100];
  __bench__(const char* format, ...)
  __attribute__((format(printf, 2, 3)))
  {
    va_list args;
    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    start = sec();
  }
  ~__bench__() {
    fprintf(stderr, "%s: %.6f sec\n", msg, sec() - start);
  }
  double sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
  }
  operator bool() { return false; }
};

#define benchmark(...) if (__bench__ __b__ = __bench__(__VA_ARGS__)); else

void InitRandom(int *a, int n) {
  for (int i = 0; i < n; ++i) {
    a[i] = rand();
  }
}

int main(int argc, char **argv) {
  if (argc >= 3) {
    fprintf(stderr, "usage: measure [size]\n");
    exit(EXIT_FAILURE);
  }

  uint64_t N = DEFAULT_N;
  if (argc == 2) N = (uint64_t)1 << std::stoul(argv[1]);
  std::cout << "N = " << N << std::endl;

  int *buf;
  //buf = new int[N];
  //std::string path = "input.dat";
  parallel_radix_sort::temp_file::set_dir("work");
  parallel_radix_sort::utility::buffer_t x;
  parallel_radix_sort::utility::CreateDiskBackedBuffer(&x, N*sizeof(int));
  buf = (int*) x.data;
  assert(buf);

  parallel_radix_sort::KeySort<int> key_sort;
  key_sort.Init(N);

  for (int t = 0; t < TRIAL; ++t) {
    InitRandom(buf, N);
    benchmark("parallel_radix_sort::KeySort(%d)", t) {
      key_sort.Sort(buf, N);
    }
  }

  parallel_radix_sort::utility::RemoveDiskBackedBuffer(&x);
  buf = NULL;
  /*
  for (int t = 0; t < TRIAL; ++t) {
    InitRandom(buf, N);
    benchmark("std::sort(%d)", t) {
      std::sort(buf, buf + N);
    }
  }
  */

  exit(EXIT_SUCCESS);
}
