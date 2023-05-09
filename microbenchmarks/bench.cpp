#include "bench.h"

static void BenchZone(benchmark::State &state) {
  for (auto _ : state) {
    zone_parser_t parser = { 0 };
    zone_name_block_t name;
    zone_rdata_block_t rdata;
    zone_cache_t cache = { 1, &name, &rdata };
    zone_options_t options = { 0 };
    zone_return_t result = zone_parse_string(&parser, &options, &cache, input, input_size, NULL);
  }
  if (collector.has_events()) {
    event_aggregate aggregate{};
    for (size_t i = 0; i < N; i++) {
      std::atomic_thread_fence(std::memory_order_acquire);
      collector.start();
      // Do something here
      std::atomic_thread_fence(std::memory_order_release);
      event_count allocate_count = collector.end();
      aggregate << allocate_count;
    }
    state.counters["instructions/cycle"] =
        aggregate.best.instructions() / aggregate.best.cycles();
    state.counters["instructions/byte"] =
        aggregate.best.instructions() / input_size;
    state.counters["instructions/ns"] =
        aggregate.best.instructions() / aggregate.best.elapsed_ns();
    state.counters["GHz"] =
        aggregate.best.cycles() / aggregate.best.elapsed_ns();
    state.counters["ns/byte"] = aggregate.best.elapsed_ns() / input_size;
    state.counters["cycle/byte"] = aggregate.best.cycles() / input_size;
  }
  state.counters["time/byte"] = benchmark::Counter(
      input_size, benchmark::Counter::kIsIterationInvariantRate |
                      benchmark::Counter::kInvert);
  state.counters["speed"] = benchmark::Counter(
      input_size, benchmark::Counter::kIsIterationInvariantRate);
}
BENCHMARK(BenchZone);

int main(int argc, char **argv) {
  
  if ((argc == 1) || (argc > 1 && argv[1][0] == '-')) {
    printf("You should provide a zone file as input. E.g. "
                                "'dig @zonedata.iis.se se AXFR > se.zone.txt'.\n");
    return EXIT_FAILURE;
  } else {
    default_file_name = argv[1];
    input_size = get_file_size(default_file_name);
    if (input_size == 0) {
      printf("The input file is empty.\n");
      return EXIT_FAILURE;
    }
    bool loaded = load_from_disk(default_file_name);
    if (!loaded) {
      printf("The input file could not be loaded.\n");
      return EXIT_FAILURE;
    }
  }
  benchmark::AddCustomContext("input size (B)", std::to_string(input_size));
#if (__APPLE__ && __aarch64__) || defined(__linux__)
  if (!collector.has_events()) {
    benchmark::AddCustomContext("performance counters",
                                "No privileged access (sudo may help).");
  }
#else
  if (!collector.has_events()) {
    benchmark::AddCustomContext("performance counters", "Unsupported system.");
  }
#endif

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  if (input) {
    std::free((void *)input);
  }
}