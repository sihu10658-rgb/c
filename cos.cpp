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

ULTRA_FORCE_INLINE double ultra_cos(double x) {
    constexpr double PI = 3.14159265358979323846;
    constexpr double HALF_PI = PI * 0.5;
    constexpr double INV_PI = 1.0 / PI;

    double scaled = (x + HALF_PI) * INV_PI;
    int64_t q = static_cast<int64_t>(scaled > 0.0 ? scaled + 0.5 : scaled - 0.5);
    double y = std::fma(-static_cast<double>(q), PI, x + HALF_PI);
    
    int sign_flip = (q & 1);

    double y2 = y * y;   
    double y4 = y2 * y2; 
    double y8 = y4 * y4; 

    constexpr double C0  =  1.0;
    constexpr double C2  = -0.49999999999999991;
    constexpr double C4  =  0.0416666666666659;
    constexpr double C6  = -0.0013888888888825;
    constexpr double C8  =  0.000024801587285;
    constexpr double C10 = -0.00000027557313;
    constexpr double C12 =  0.00000000208757;
    constexpr double C14 = -0.00000000001135;

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
    double test_values[] = {0.0, 0.5, 1.0, 1.57079632679, 3.14159265359, -0.5, -1.57079632679};

    for (double x : test_values) {
        double res = ultra_cos(x);
        double std_res = std::cos(x);
        
        std::cout << "x = " << x << "\n"
                  << "  -> ultra_cos : " << res << "\n"
                  << "  -> std::cos  : " << std_res << "\n\n";
    }

    return 0;
}
