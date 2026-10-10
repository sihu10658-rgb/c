#include <iostream>

#include <cmath>

#include <cstdint>



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



    double scaled = x * INV_HALF_PI;

    int64_t q = static_cast<int64_t>(scaled > 0.0 ? scaled + 0.5 : scaled - 0.5);

    double y = std::fma(-static_cast<double>(q), HALF_PI, x);

    

    double y2 = y * y;

    double y4 = y2 * y2;



    constexpr double C0 = 1.0;

    constexpr double C1 = 0.33333333333333333;

    constexpr double C2 = 0.13333333333333333;

    constexpr double C3 = 0.0539682539682540;



    double p0 = std::fma(C3, y2, C2);

    double p1 = std::fma(C1, y2, C0);

    double p2 = std::fma(p0, y4, p1);

    

    return y * p2;

}



int main() {

    double test_values[] = {0.0, 0.2, 0.5, 0.78539816339, 1.0, -0.5, -1.0};



    for (double x : test_values) {

        double res = ultra_tan(x);

        double std_res = std::tan(x);

        

        std::cout << "x = " << x << "\n"

                  << "  -> ultra_tan : " << res << "\n"

                  << "  -> std::tan  : " << std_res << "\n\n";

    }



    return 0;
