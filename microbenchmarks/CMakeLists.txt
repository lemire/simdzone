function(set_off NAME)
  set("${NAME}" OFF CACHE INTERNAL "")
endfunction()
set_off(BENCHMARK_ENABLE_TESTING)
set_off(BENCHMARK_ENABLE_INSTALL)
set_off(BENCHMARK_ENABLE_WERROR)
find_package(google_benchmark REQUIRED)

add_executable(bench bench.cpp)
set_target_properties(bench
    PROPERTIES
    CXX_STANDARD 11
    CXX_EXTENSIONS off
)
target_link_libraries(bench PRIVATE zone)
target_link_libraries(bench PRIVATE benchmark::benchmark)