#ifndef CROARING_MICROBENCHMARKS_BENCH_H
#define CROARING_MICROBENCHMARKS_BENCH_H
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <climits>
#include <benchmark/benchmark.h>
extern "C" {
#include "zone.h"
}

#include "performancecounters/event_counter.h"

const size_t N = 10;
const char *default_file_name = NULL;
char *input = NULL;
size_t input_size = 0;

static size_t get_file_size(const char *filename) {
  if (filename == NULL) {
    return false;
  }
  // Open the file
  std::FILE *fp = std::fopen(filename, "rb");

  if (fp == nullptr) {
    return false;
  }

  // Get the file size
  int ret;
#if _WIN64
  ret = _fseeki64(fp, 0, SEEK_END);
#else
  ret = std::fseek(fp, 0, SEEK_END);
#endif // _WIN64
  if (ret < 0) {
    std::fclose(fp);
    return false;
  }
#if _WIN64
  __int64 llen = _ftelli64(fp);
  if (llen == -1L) {
    std::fclose(fp);
    return false;
  }
#else
  long llen = std::ftell(fp);
  if ((llen < 0) || (llen == LONG_MAX)) {
    std::fclose(fp);
    return false;
  }
#endif

  size_t len = (size_t)llen;
  return len;
}

event_collector collector;

static bool load_from_disk(const char *filename) {
  if (filename == NULL) {
    return false;
  }
  // Open the file
  std::FILE *fp = std::fopen(filename, "rb");

  if (fp == nullptr) {
    return false;
  }

  // Get the file size
  int ret;
#if _WIN64
  ret = _fseeki64(fp, 0, SEEK_END);
#else
  ret = std::fseek(fp, 0, SEEK_END);
#endif // _WIN64
  if (ret < 0) {
    std::fclose(fp);
    return false;
  }
#if _WIN64
  __int64 llen = _ftelli64(fp);
  if (llen == -1L) {
    std::fclose(fp);
    return false;
  }
#else
  long llen = std::ftell(fp);
  if ((llen < 0) || (llen == LONG_MAX)) {
    std::fclose(fp);
    return false;
  }
#endif

  size_t len = (size_t)llen;
  input = (char *)malloc(len + ZONE_BLOCK_SIZE);
  if (input == nullptr) {
    std::fclose(fp);
    return false;
  }

  std::rewind(fp);
  size_t bytes_read = std::fread(input, 1, len, fp);
  if (std::fclose(fp) != 0 || bytes_read != len) {
    free(input);
    return false;
  }
  input_size = len;

  return true;
}


#endif
