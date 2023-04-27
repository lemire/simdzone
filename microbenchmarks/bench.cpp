#include "bench.h"

static void BenchZone(benchmark::State &state) {
  // volatile to prevent optimizations.
  volatile int r = 0;

  for (auto _ : state) {
    // Do something here.
  }
  if (collector.has_events()) {
    event_aggregate aggregate{};
    for (size_t i = 0; i < N; i++) {
      std::atomic_thread_fence(std::memory_order_acquire);
      collector.start();
      // Do something here.
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
  (void)r;
}
BENCHMARK(BenchZone);

int main(int argc, char **argv) {
  if ((argc == 1) || (argc > 1 && argv[1][0] == '-')) {
    benchmark::AddCustomContext("You should provide a zone file as input. E.g. "
                                "'dig @zonedata.iis.se se AXFR > se.zone.txt'.",
                                "Missing input file.");
    return EXIT_FAILURE;
  } else {
    default_file_name = argv[1];
    input_size = get_file_size(default_file_name);
    if (input_size == 0) {
      benchmark::AddCustomContext("The input file is empty.",
                                  "Empty input file.");
      return EXIT_FAILURE;
    }
  }
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