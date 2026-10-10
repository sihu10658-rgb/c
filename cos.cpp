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

ULTRA_FORCE_INLINE double ultra_cos(double x) {
    constexpr double PI = 3.14159265358979323846;
    constexpr double HALF_PI = PI * 0.5;
    constexpr double INV_PI = 1.0 / PI;

    // 코사인 위상 환산 (x + PI/2)
    double scaled = (x + HALF_PI) * INV_PI;
    int64_t q = static_cast<int64_t>(scaled > 0.0 ? scaled + 0.5 : scaled - 0.5);
    double y = std::fma(-static_cast<double>(q), PI, x + HALF_PI);
    
    int sign_flip = (q & 1);

    double y2 = y * y;   // z = y^2
    double y4 = y2 * y2; // z^2 = y^4
    double y8 = y4 * y4; // z^4 = y^8

    // 코사인 전용 미니맥스 체비쇼브 최적화 계수 (짝수 항)
    constexpr double C0  =  1.0;
    constexpr double C2  = -0.49999999999999991;
    constexpr double C4  =  0.0416666666666659;
    constexpr double C6  = -0.0013888888888825;
    constexpr double C8  =  0.000024801587285;
    constexpr double C10 = -0.00000027557313;
    constexpr double C12 =  0.00000000208757;
    constexpr double C14 = -0.00000000001135;

    // Estrin 스킴 (짝수 차항 병렬 평가)
    double p0 = std::fma(C2,  y2, C0);
    double p1 = std::fma(C6,  y2, C4);
    double A  = std::fma(p1,  y4, p0);

    double p2 = std::fma(C10, y2, C8);
    double p3 = std::fma(C14, y2, C12);
    double B  = std::fma(p3,  y4, p2);

    double poly = std::fma(B, y8, A);

    return sign_flip ? -poly : poly;
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
        sum0 += ultra_cos(test_values[(i + 0) & cache_mask]);
        sum1 += ultra_cos(test_values[(i + 1) & cache_mask]);
        sum2 += ultra_cos(test_values[(i + 2) & cache_mask]);
        sum3 += ultra_cos(test_values[(i + 3) & cache_mask]);
    }
    
    volatile double dummy = sum0 + sum1 + sum2 + sum3;
    auto end = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    std::cout << "Ultra Pro Cos : " << static_cast<double>(ns) / iterations << " ns per call\n";
    (void)dummy;
    
    return 0;
}
