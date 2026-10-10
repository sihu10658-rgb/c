#include <iostream>
#include <bit>
#include <cstdint>
#include <vector>
#include <chrono>

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

ULTRA_FORCE_INLINE double ultra_sqrt(double x) {
    if (x <= 0.0) return 0.0;

    // IEEE 754 비트 조작을 통한 초기 추정값(Initial Guess) 계산
    uint64_t i = std::bit_cast<uint64_t>(x);
    i = 0x5FE6EB501F7B3D11ULL - (i >> 1); // rsqrt용 매직 넘버 기반 근사
    double y = std::bit_cast<double>(i);

    // Newton-Raphson 반복법 (FMA 극대화)
    // y = y * (1.5 - 0.5 * x * y * y) 형태의 rsqrt를 구한 뒤 x를 곱해 sqrt를 완성
    y = std::fma(-0.5 * x * y, y, 1.5) * y;
    y = std::fma(-0.5 * x * y, y, 1.5) * y; // 2차 정밀도 향상

    return x * y;
}

int main() {
    constexpr int iterations = 100000000;
    constexpr int cache_size = 4096;
    constexpr int cache_mask = cache_size - 1;
    std::vector<double> test_values(cache_size);
    
    for (int i = 0; i < cache_size; ++i) {
        test_values[i] = static_cast<double>(i + 1) * 0.1;
    }

    double sum0 = 0.0, sum1 = 0.0, sum2 = 0.0, sum3 = 0.0;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; i += 4) {
        ULTRA_ASSUME(iterations > 0);
        sum0 += ultra_sqrt(test_values[(i + 0) & cache_mask]);
        sum1 += ultra_sqrt(test_values[(i + 1) & cache_mask]);
        sum2 += ultra_sqrt(test_values[(i + 2) & cache_mask]);
        sum3 += ultra_sqrt(test_values[(i + 3) & cache_mask]);
    }
    
    volatile double dummy = sum0 + sum1 + sum2 + sum3;
    auto end = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    std::cout << "Ultra Sqrt : " << static_cast<double>(ns) / iterations << " ns per call\n";
    (void)dummy;
    
    return 0;
}
