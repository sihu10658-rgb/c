#include <iostream>
#include <bit>
#include <cstdint>
#include <vector>
#include <chrono>
#include <cmath>

#if defined(_MSC_VER)
    #define ULTRA_FORCE_INLINE __forceinline
    #define ULTRA_ASSUME(cond) __assume(cond)
#elif defined(__GNUC__) || defined(__CLANG__)
    #define ULTRA_FORCE_INLINE inline __attribute__((always_inline))
    #define ULTRA_ASSUME(cond) __builtin_assume(cond)
#else
    #define ULTRA_FORCE_INLINE inline
    #define ULTRA_ASSUME(cond) ((void)0)
#endif

ULTRA_FORCE_INLINE double ultra_sin(double x) {
    constexpr double PI = 3.14159265358979323846;
    constexpr double INV_PI = 1.0 / PI;

    double scaled = x * INV_PI;
    int64_t q = static_cast<int64_t>(scaled > 0.0 ? scaled + 0.5 : scaled - 0.5);
    double y = std::fma(-static_cast<double>(q), PI, x);
    
    int sign_flip = (q & 1);

    double y2 = y * y;   // z
    double y4 = y2 * y2; // z^2
    double y8 = y4 * y4; // z^4

    // 제공해주신 15차 체비쇼브/미니맥스 최적화 계수
    constexpr double C1  =  0.880101171489867;
    constexpr double C3  = -0.039102643615935;
    constexpr double C5  =  0.000490647102693;
    constexpr double C7  = -0.000003620032549;
    constexpr double C9  =  0.000000019013233;
    constexpr double C11 = -0.000000000078106;
    constexpr double C13 =  0.000000000000247;
    constexpr double C15 = -0.0000000000000006422;

    // Estrin 스킴 (15차 홀수 항 병렬 평가)
    double p0 = std::fma(C3,  y2, C1);
    double p1 = std::fma(C7,  y2, C5);
    double p2 = std::fma(C11, y2, C9);
    double p3 = std::fma(C15, y2, C13);

    double t0 = std::fma(p1, y4, p0);
    double t1 = std::fma(p3, y4, p2);
    
    double poly = std::fma(t1, y8, t0);
    double result = y * poly;

    return sign_flip ? -result : result;
}

int main() {
    constexpr int iterations = 100000000;
    constexpr int cache_size = 4096;
    constexpr int cache_mask = cache_size - 1;
    std::vector<double> test_values(cache_size);
    
    for (int i = 0; i < cache_size; ++i) {
        test_values[i] = static_cast<double>(i) * (3.14159265358979323846 / cache_size);
    }

    double sum0 = 0.0, sum1 = 0.0, sum2 = 0.0, sum3 = 0.0;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; i += 4) {
        ULTRA_ASSUME(iterations > 0);
        sum0 += ultra_sin(test_values[(i + 0) & cache_mask]);
        sum1 += ultra_sin(test_values[(i + 1) & cache_mask]);
        sum2 += ultra_sin(test_values[(i + 2) & cache_mask]);
        sum3 += ultra_sin(test_values[(i + 3) & cache_mask]);
    }
    
    volatile double dummy = sum0 + sum1 + sum2 + sum3;
    auto end = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    std::cout << "Ultra Pro Sin : " << static_cast<double>(ns) / iterations << " ns per call\n";
    (void)dummy;
    
    return 0;
}
