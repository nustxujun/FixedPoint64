#define FIXED_64_ENABLE_TRIG_LUT 1
#define FIXED_64_ENABLE_SATURATING 1

#include "fixed64.h"
#include <math.h>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <random>

#define TEST_COUNT 1

#define FRACTION_BITS 32

using namespace f64;
using fixed = fixed64<FRACTION_BITS>;


const auto max_error = std::numeric_limits<fixed>::epsilon() * 2 ;

#define TEST_CMP_OPT(A,B, OP) {assert( (A OP B) == (fixed(A) OP fixed(B)) );}

#define TEST_MATH_OPT(a,b, OP,ERR) { double r1 = (a OP b); auto r2 = (fixed(a) OP fixed(b));printf("---\n%s\nr1: %.16lf\nr2: %.16lf\ndiff: %.16lf\n", #a#OP#b,r1,(double)r2,(double)abs(r1 - r2)); assert( abs(r1 - r2) <= ERR );}

#define TEST_MATH_FUNC_1(A, FUNC, ERR) {double r1 = FUNC(A); auto r2 = FUNC(fixed(A));printf("---\n%s(a)\nr1: %.16lf\nr2: %.16lf\ndiff: %.16lf\n", #FUNC,r1,(double)r2,(double)abs(r1 - r2)); assert( abs(r1 - r2) <= ERR );}
#define TEST_MATH_FUNC_2(a,b, FUNC, ERR) {double r1 = FUNC(a,b); auto r2 = FUNC(fixed(a), fixed(b));printf("---\n%s(a,b)\nr1: %.16lf\nr2: %.16lf\ndiff: %.16lf\n", #FUNC,r1,(double)r2,(double)abs(r1 - r2)); assert( abs(r1 - r2) <= ERR );}

#define TEST_CONSTEXPR(FUNC, ...) {constexpr auto r = FUNC(__VA_ARGS__ );}

auto test_func = []{

	std::default_random_engine e;
	e.seed((unsigned int)& e);

	constexpr auto max = sqrt(std::numeric_limits<fixed>::max());
	std::uniform_real_distribution<double> u(-double(max), double(max));
	double a = u(e);
	double a_f = a - (int64_t)a;
	double b = u(e);


	TEST_CMP_OPT(a, b, < );
	TEST_CMP_OPT(a, b, <= );
	TEST_CMP_OPT(a, b, == );
	TEST_CMP_OPT(a, b, != );
	TEST_CMP_OPT(a, b, >= );
	TEST_CMP_OPT(a, b, > );

	TEST_MATH_OPT(a, b, +, max_error);
	TEST_MATH_OPT(a, b, -, max_error);
	TEST_MATH_OPT(a, b, *, 1e-4);
	TEST_MATH_OPT(a, b, / , 1e-4);

	TEST_MATH_FUNC_1(a, ceil, max_error);
	TEST_MATH_FUNC_1(a, floor, max_error);
	TEST_MATH_FUNC_1(a, round, max_error);
	TEST_MATH_FUNC_1(a, abs, max_error);
	TEST_MATH_FUNC_1(a_f, exp, 1e-4);
	TEST_MATH_FUNC_1(a_f, exp2, 1e-2);
	TEST_MATH_FUNC_1(abs(a), log2, 1e-4);
	TEST_MATH_FUNC_1(abs(a), sqrt, 1e-4);

	TEST_MATH_FUNC_1(a, sin, 1e-3);
	TEST_MATH_FUNC_1(a, cos, 1e-3);
	TEST_MATH_FUNC_1(a, tan, 1e-1);

	TEST_MATH_FUNC_1(a_f, asin, 1e-3);
	TEST_MATH_FUNC_1(a_f, acos, 1e-2);
	TEST_MATH_FUNC_1(a_f, atan, 1e-2);


	TEST_MATH_FUNC_2(a, b, fmod, 1e-4);
	TEST_MATH_FUNC_2(abs(a), 2, pow, 1e-4);


	constexpr fixed c_a = 1;
	constexpr fixed c_b = 0.02;

	TEST_CONSTEXPR(ceil,c_a);
	TEST_CONSTEXPR(floor, c_a);
	TEST_CONSTEXPR(round, c_a);
	TEST_CONSTEXPR(abs, c_a);
	TEST_CONSTEXPR(exp, c_a);
	TEST_CONSTEXPR(exp2, c_a);
	TEST_CONSTEXPR(sqrt, c_a);

	TEST_CONSTEXPR(fmod, c_a, c_b);

#if __cplusplus >= 202002L || FIXED_64_FORCE_EVALUATE_IN_COMPILE_TIME // MSVC requires /Zc:__cplusplus 
	TEST_CONSTEXPR(pow, c_a, c_b);
	TEST_CONSTEXPR(log2, c_a);

	TEST_CONSTEXPR(sin, c_a);
	TEST_CONSTEXPR(cos, c_a);
	TEST_CONSTEXPR(tan, c_a);

	TEST_CONSTEXPR(asin, c_a);
	TEST_CONSTEXPR(acos, c_a);
	TEST_CONSTEXPR(atan, c_a);

#endif


	const double pi = 3.14159265358979323846;
	const int count = 10000;
	double err = 0;
	double merr = 0;
	for (int i = 0; i < count; ++i)
	{
		auto rad = pi * ((double(i) / count) - 0.5) * a;

		auto r1 = sin(rad);
		auto r2 = f64::sin(fixed(rad));

		auto diff = abs((double)r1 - (double)r2);
		merr = std::max(merr, diff);
		err += diff;

	}

	std::cout << "test sin\navg err:" << err / count << ", max err: " << merr << std::endl;


};


auto test = []{
	for (int i = 0; i < TEST_COUNT; ++i)
	{
		test_func();
	}
	return 0;
}();
