function(set_off NAME)
  set("${NAME}" OFF CACHE INTERNAL "")
endfunction()

include(${PROJECT_SOURCE_DIR}/microbenchmarks/import.cmake)

set_off(BENCHMARK_ENABLE_TESTING)
set_off(BENCHMARK_ENABLE_INSTALL)
set_off(BENCHMARK_ENABLE_WERROR)
import_dependency(google_benchmarks google/benchmark f91b6b4)
add_dependency(google_benchmarks)
# In theory this should work with conan, but it appears not to work.
# find_package(google_benchmark REQUIRED)


add_executable(bench bench.cpp)
set_target_properties(bench
    PROPERTIES
    CXX_STANDARD 11
    CXX_EXTENSIONS off
)
target_link_libraries(bench PRIVATE zone)
target_link_libraries(bench PRIVATE benchmark::benchmark)