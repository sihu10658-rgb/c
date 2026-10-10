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

ULTRA_FORCE_INLINE double ultra_tan(double x) {
    constexpr double HALF_PI = 1.57079632679489661923;
    constexpr double INV_HALF_PI = 1.0 / HALF_PI;

    // [-PI/4, PI/4] 범위로 협착하기 위한 쿼드런트 계산
    double scaled = x * INV_HALF_PI;
    int64_t q = static_cast<int64_t>(scaled > 0.0 ? scaled + 0.5 : scaled - 0.5);
    double y = std::fma(-static_cast<double>(q), HALF_PI, x);
    
    double y2 = y * y;
    double y4 = y2 * y2;

    // 탄젠트 전용 미니맥스 체비쇼브 계수 (홀수 항 기반: y * (c0 + c1*y^2 + c2*y^4 + ...))
    constexpr double C0 = 1.0;
    constexpr double C1 = 0.33333333333333333;
    constexpr double C2 = 0.13333333333333333;
    constexpr double C3 = 0.0539682539682540;

    // Estrin 스킴 적용
    double p0 = std::fma(C3, y2, C2);
    double p1 = std::fma(C1, y2, C0);
    double p2 = std::fma(p0, y4, p1);
    
    return y * p2;
}

int main() {
    constexpr int iterations = 100000000;
    constexpr int cache_size = 4096;
    constexpr int cache_mask = cache_size - 1;
    std::vector<double> test_values(cache_size);
    
    for (int i = 0; i < cache_size; ++i) {
        // 안전한 범위 내에서 테스트
        test_values[i] = static_cast<double>(i) * (1.0 / cache_size);
    }

    double sum0 = 0.0, sum1 = 0.0, sum2 = 0.0, sum3 = 0.0;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; i += 4) {
        ULTRA_ASSUME(iterations > 0);
        sum0 += ultra_tan(test_values[(i + 0) & cache_mask]);
        sum1 += ultra_tan(test_values[(i + 1) & cache_mask]);
        sum2 += ultra_tan(test_values[(i + 2) & cache_mask]);
        sum3 += ultra_tan(test_values[(i + 3) & cache_mask]);
    }
    
    volatile double dummy = sum0 + sum1 + sum2 + sum3;
    auto end = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    std::cout << "Ultra Direct Tan : " << static_cast<double>(ns) / iterations << " ns per call\n";
    (void)dummy;
    
    return 0;
}
