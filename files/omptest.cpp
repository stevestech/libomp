// omptest.cpp - sanity test for an OpenMP runtime
#include <cstdio>
#include <vector>
#include <omp.h>

int main() {
    int failures = 0;

    // 1. The runtime reports at least one thread.
    const int max_threads = omp_get_max_threads();
    std::printf("max threads: %d\n", max_threads);
    if (max_threads < 1) {
        std::printf("FAIL: omp_get_max_threads() < 1\n");
        return 1;
    }

    // 2. A parallel region starts the requested number of threads, and
    //    every thread id from 0 to n-1 appears exactly once.
    const int requested = 4;
    omp_set_num_threads(requested);
    std::vector<int> seen(requested, 0);
    int team_size = 0;

    #pragma omp parallel
    {
        const int id = omp_get_thread_num();
        #pragma omp single
        team_size = omp_get_num_threads();
        #pragma omp critical
        {
            if (id >= 0 && id < requested) seen[id]++;
        }
    }

    std::printf("team size: %d\n", team_size);
    if (team_size != requested) {
        std::printf("FAIL: expected %d threads, got %d\n", requested, team_size);
        ++failures;
    }
    for (int i = 0; i < requested; ++i) {
        if (seen[i] != 1) {
            std::printf("FAIL: thread id %d seen %d times\n", i, seen[i]);
            ++failures;
        }
    }

    // 3. A parallel reduction gives the right answer.
    //    sum of 1..n = n(n+1)/2
    const long n = 10000000L;
    long sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (long i = 1; i <= n; ++i) sum += i;

    const long expected = n * (n + 1) / 2;
    if (sum != expected) {
        std::printf("FAIL: reduction gave %ld, expected %ld\n", sum, expected);
        ++failures;
    }

    // 4. omp_get_wtime() advances.
    const double t0 = omp_get_wtime();
    #pragma omp parallel for
    for (int i = 0; i < 1000000; ++i) {
        volatile double x = i * 0.5;
        (void)x;
    }
    if (omp_get_wtime() < t0) {
        std::printf("FAIL: omp_get_wtime() went backwards\n");
        ++failures;
    }

    if (failures == 0) std::printf("PASS\n");
    return failures == 0 ? 0 : 1;
}
