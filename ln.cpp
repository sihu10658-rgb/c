#include <iostream>
#include <bit>
#include <cstdint>
#include <cmath>

#if defined(_MSC_VER)
    #define ULTRA_FORCE_INLINE __forceinline
    #define ULTRA_ASSUME(cond) __assume(cond)
#elif defined(__GNUC__) || defined(__CLANG__)
    #define ULTRA_FORCE_INLINE __attribute__((always_inline)) inline
    #define ULTRA_ASSUME(cond) __builtin_assume(cond)
#else
    #define ULTRA_FORCE_INLINE inline
    #define ULTRA_ASSUME(cond) ((void)0)
#endif

ULTRA_FORCE_INLINE double ultra_ln(double x) {
    uint64_t bits = std::bit_cast<uint64_t>(x);
    int64_t exp = static_cast<int64_t>((bits >> 52) & 0x7FF) - 1023;
    
    uint64_t m_bits = (bits & 0x000FFFFFFFFFFFFFULL) | (1023ULL << 52);
    double m = std::bit_cast<double>(m_bits);

    double t = std::fma(2.0, m, -3.0);

    constexpr double C[20] = {
         0.37645281291919543163075440704323148,
         0.34314575050761980479324510316120769,
        -0.029437251522859414379735309483623057,
         0.0033670892555643892545262035474230001,
        -0.00043327588861004445550026122159197179,
         0.000059470711989579833685531735187664528,
        -0.0000085029675412028647608178615436747826,
         0.0000012504673622005661373977628048527904,
        -0.00000018772799565082365072485856772106289,
         0.000000028630250648396919223207957404609527,
        -0.0000000044209569806844432254361158080411317,
         0.0000000006895602732267564106632119855438325,
        -0.00000000010845068551012423745090274712346903,
         1.7175873171894198397666611685200931e-11,
        -2.7364200918754732576544818545746192e-12,
         4.3819576552767829822070159177703514e-13,
        -7.04836007021513270437748309930562e-14,
         1.1381716734785466052462346981025633e-14,
        -1.8443053174275724805978942320096787e-15,
         2.9977841004341605330066728311171738e-16
    };

    double t2  = t * t;
    double t4  = t2 * t2;
    double t8  = t4 * t4;
    double t16 = t8 * t8;

    double p0  = std::fma(std::fma(C[3], t, C[2]), t2, std::fma(C[1], t, C[0]));
    double p4  = std::fma(std::fma(C[7], t, C[6]), t2, std::fma(C[5], t, C[4]));
    double p8  = std::fma(std::fma(C[11], t, C[10]), t2, std::fma(C[9], t, C[8]));
    double p12 = std::fma(std::fma(C[15], t, C[14]), t2, std::fma(C[13], t, C[12]));
    double p16 = std::fma(std::fma(C[19], t, C[18]), t2, std::fma(C[17], t, C[16]));

    double term1 = std::fma(p4,  t4,  p0);
    double term2 = std::fma(p12, t4,  p8);
    double term3 = std::fma(term2, t8, term1);
    double poly  = std::fma(p16, t16, term3);

    constexpr double LN2 = 0.6931471805599453;
    return std::fma(static_cast<double>(exp), LN2, poly);
}

int main() {
    double test_values[] = {0.5, 1.0, 2.0, 5.0, 10.0, 100.0};

    for (double x : test_values) {
        double res = ultra_ln(x);
        double std_res = std::log(x);
        
        std::cout << "x = " << x << "\n"
                  << "  -> ultra_ln : " << res << "\n"
                  << "  -> std::log  : " << std_res << "\n\n";
    }

    return 0;
}
