#include "parallel_radix_sort.hpp"

#include <cstdio>
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <algorithm>

int
main(int argc, char *argv[]) {
  int opt;
  int char_start = 0;
  int char_stop = 255;
  int record_size=8;
  int key_size=8;
  int stack_size=5;
  int cut_off = 4;
  int switch_to_shell = 20;
  int verbosity = 0;
  omp_set_num_threads(1);

  while ((opt = getopt(argc, argv, "vr:k:t:")) != -1) {
    switch (opt) {
    case 'v':
      verbosity += 1;
      break;
    case 'r':
      record_size = atoi(optarg);
      break;
    case 'k':
      key_size = atoi(optarg);
      break;
    case 't':
      omp_set_num_threads(atoi(optarg));
      break;
    default:
      fprintf(stderr, "Invalid parameter: -%c\n", opt);
      goto failure;
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Expected argument after options\n");
    goto failure;
  }

  while(optind < argc) {
    if (verbosity)
        printf("sorting %s\n", argv[optind]);
    parallel_radix_sort::temp_file::set_dir("work");
    parallel_radix_sort::utility::buffer_t buf;
    parallel_radix_sort::utility::OpenDiskBackedBuffer(std::string(argv[optind]), &buf);
    parallel_radix_sort::KeySort<uint8_t> key_sort;
    key_sort.Init(buf.size/sizeof(uint8_t));
    key_sort.Sort((uint8_t*)buf.data, buf.size/sizeof(uint8_t));
    parallel_radix_sort::utility::CloseDiskBackedBuffer(&buf);
    optind++;
  }

  exit(0);
failure:
  fprintf(stderr,
          "Usage: %s [-r ###] [-k ###] [-t ###] file1, file2 ... \n",
          argv[0]);
  fprintf(stderr,
          "Sort binary files with a parallel radix sort\n"
          "\n"
          "Sorting Options:\n"
          "\n"
          "  -k ###   size of compariable section of record, in bytes (default 8)\n"
          "  -r ###   size of overall record, in bytes.  (default 8)\n"
          "  -t ###   number of threads to use.  (default 1)\n"
          "\n"
          "Options:\n"
          "  -v  verbose output logging\n"
         );
  exit(1);
}
