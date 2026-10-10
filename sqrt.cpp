#include <iostream>
#include <bit>
#include <cstdint>
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

ULTRA_FORCE_INLINE double ultra_sqrt(double x) {
    if (x <= 0.0) return 0.0;

    uint64_t i = std::bit_cast<uint64_t>(x);
    i = 0x5FE6EB501F7B3D11ULL - (i >> 1);
    double y = std::bit_cast<double>(i);

    y = std::fma(-0.5 * x * y, y, 1.5) * y;
    y = std::fma(-0.5 * x * y, y, 1.5) * y;

    return x * y;
}

int main() {
    double test_values[] = {0.0, 0.5, 1.0, 2.0, 4.0, 9.0, 16.0};

    for (double x : test_values) {
        double res = ultra_sqrt(x);
        double std_res = std::sqrt(x);
        
        std::cout << "x = " << x << "\n"
                  << "  -> ultra_sqrt : " << res << "\n"
                  << "  -> std::sqrt  : " << std_res << "\n\n";
    }

    return 0;
}
