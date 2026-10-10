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

ULTRA_FORCE_INLINE double ultra_sin(double x) {
    constexpr double PI = 3.14159265358979323846;
    constexpr double INV_PI = 1.0 / PI;

    double scaled = x * INV_PI;
    int64_t q = static_cast<int64_t>(scaled > 0.0 ? scaled + 0.5 : scaled - 0.5);
    double y = std::fma(-static_cast<double>(q), PI, x);
    
    int sign_flip = (q & 1);

    double y2 = y * y;   
    double y4 = y2 * y2; 
    double y8 = y4 * y4; 

    constexpr double C1  =  0.880101171489867;
    constexpr double C3  = -0.039102643615935;
    constexpr double C5  =  0.000490647102693;
    constexpr double C7  = -0.000003620032549;
    constexpr double C9  =  0.000000019013233;
    constexpr double C11 = -0.000000000078106;
    constexpr double C13 =  0.000000000000247;
    constexpr double C15 = -0.0000000000000006422;

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
    double test_values[] = {0.0, 0.5, 1.0, 1.57079632679, 3.14159265359, -0.5, -1.57079632679};

    for (double x : test_values) {
        double res = ultra_sin(x);
        double std_res = std::sin(x);
        
        std::cout << "x = " << x << "\n"
                  << "  -> ultra_sin : " << res << "\n"
                  << "  -> std::sin  : " << std_res << "\n\n";
    }

    return 0;
}
