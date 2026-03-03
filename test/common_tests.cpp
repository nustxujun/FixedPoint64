#define FIXED_64_ENABLE_TRIG_LUT 1
#define FIXED_64_ENABLE_SATURATING 1

#include "fixed64.hpp"
#include <math.h>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>

#define TEST_COUNT 1

#define FRACTION_BITS 32

using namespace f64;
using fixed = fixed64<FRACTION_BITS>;


const auto max_error = 0.0001f;


void report(bool expr)
{
	//if (!expr)
	//{
	//	throw 1;
	//}
}

static std::default_random_engine e;

struct Operand
{
	double a;
	double b;
	fixed fa;
	fixed fb;

	inline Operand(float Min, float Max)
	{
		std::uniform_real_distribution<double> u(Min, Max);
		a = u(e);
		b = u(e);
		fa = a;
		fb = b;
	}
};


#define TEST_CMP_OPT( OP) {Operand operand(-100, 100); report( (operand.a OP operand.b) == (operand.fa OP operand.fb) );}

#define TEST_MATH_OPT(Min,Max, OP, COUNT,ERR) {\
	double aerr = 0; double merr = 0;\
	for (int i = 0; i < COUNT; ++i){\
		Operand operand(Min, Max);\
		double r1;\
		fixed r2;\
		{auto a = operand.a; auto b = operand.b; r1 = OP;}\
		{auto a = operand.fa; auto b = operand.fb; r2 = OP; }\
		auto diff = abs(r1 - (double)r2);\
		aerr += diff; merr = std::max(diff, merr);\
	}\
	aerr /= COUNT;\
	printf("---\n%s\navg err: %.16lf %c\nmax err: %.16lf %c\n", #OP,aerr, aerr < ERR ? ' ':'!', merr, merr < ERR ? ' ':'!');\
	report(merr < ERR);}

#define TEST_MATH_FUNC_1(A, FUNC, ERR) {\
	double r1 = FUNC(A); auto r2 = FUNC(fixed(A));printf("---\n%s(a)\nr1: %.16lf\nr2: %.16lf\ndiff: %.16lf\n", #FUNC,r1,(double)r2,(double)abs(r1 - r2)); report( abs(r1 - r2) <= ERR );}
#define TEST_MATH_FUNC_2(a,b, FUNC, ERR) {double r1 = FUNC(a,b); auto r2 = FUNC(fixed(a), fixed(b));printf("---\n%s(a,b)\nr1: %.16lf\nr2: %.16lf\ndiff: %.16lf\n", #FUNC,r1,(double)r2,(double)abs(r1 - r2)); report( abs(r1 - r2) <= ERR );}

#ifdef FIXED_64_ENABLE_INT128_ACCELERATION
#define TEST_CONSTEXPR(FUNC, ...)
#else
#define TEST_CONSTEXPR(FUNC, ...) {constexpr auto r = FUNC(__VA_ARGS__ );}
#endif
auto test_func = [] {

	e.seed(std::chrono::steady_clock::now().time_since_epoch().count());

	const double pi = 3.1415926;
	


	TEST_CMP_OPT( < );
	TEST_CMP_OPT( <= );
	TEST_CMP_OPT( == );
	TEST_CMP_OPT( != );
	TEST_CMP_OPT( >= );
	TEST_CMP_OPT( > );

	const int count = 0xffff;

	TEST_MATH_OPT(-100, 100, a+b, count, max_error);
	TEST_MATH_OPT(-100, 100, a-b, count, max_error);
	TEST_MATH_OPT(-100, 100, a*b, count, max_error);
	TEST_MATH_OPT(-100, 100, a/b , count, max_error);

	TEST_MATH_OPT(-100, 100, ceil(a), count, max_error);
	TEST_MATH_OPT(-100, 100, floor(a), count, max_error);
	TEST_MATH_OPT(-100, 100, round(a), count, max_error);
	TEST_MATH_OPT(-100, 100, abs(a), count, max_error);
	TEST_MATH_OPT(0, 1, exp(a), count, max_error);
	TEST_MATH_OPT(0, 1, exp2(a), count, max_error);
	TEST_MATH_OPT(0, 100, log2(a), count, max_error);
	TEST_MATH_OPT(0, 10000, sqrt(a), count, max_error);

	TEST_MATH_OPT(-100, 100, sin(a), count, max_error);
	TEST_MATH_OPT(-100, 100, cos(a), count, max_error);
	TEST_MATH_OPT(-pi / 4, pi / 4, tan(a), count, max_error);

	TEST_MATH_OPT(-1, 1, asin(a), count, max_error);
	TEST_MATH_OPT(-1, 1, acos(a), count, max_error);
	TEST_MATH_OPT(-100, 100, atan(a), count, max_error);

	TEST_MATH_OPT(-100, 100, fmod(a,b), count, max_error);
	TEST_MATH_OPT(0, 1, pow(a,b), count, max_error);



	constexpr fixed c_a = 1;
	constexpr fixed c_b = 0.02;

	TEST_CONSTEXPR(ceil, c_a);
	TEST_CONSTEXPR(floor, c_a);
	TEST_CONSTEXPR(round, c_a);
	TEST_CONSTEXPR(abs, c_a);
	TEST_CONSTEXPR(exp, c_a);
	TEST_CONSTEXPR(exp2, c_a);

	TEST_CONSTEXPR(fmod, c_a, c_b);

#if __cplusplus >= 202002L || FIXED_64_FORCE_EVALUATE_IN_COMPILE_TIME // MSVC requires /Zc:__cplusplus 
	TEST_CONSTEXPR(sqrt, c_a);
	TEST_CONSTEXPR(pow, c_a, c_b);
	TEST_CONSTEXPR(log2, c_a);

	TEST_CONSTEXPR(sin, c_a);
	TEST_CONSTEXPR(cos, c_a);
	TEST_CONSTEXPR(tan, c_a);

	TEST_CONSTEXPR(asin, c_a);
	TEST_CONSTEXPR(acos, c_a);
	TEST_CONSTEXPR(atan, c_a);

#endif





};


void test_from_fixed() {
	printf("==== test_from_fixed ====\n");

	using f16 = fixed64<16>;
	using f24 = fixed64<24>;
	using f32 = fixed64<32>;
	using f48 = fixed64<48>;

	int pass = 0, fail = 0;

	auto check = [&](const char* name, double got, double expect, double eps = 1e-4) {
		bool ok = std::abs(got - expect) <= eps;
		printf("  %s  %-45s  got=%-16.6f expect=%.6f\n", ok ? "PASS" : "FAIL", name, got, expect);
		ok ? pass++ : fail++;
	};

	printf("\n[BUG1] return type is fixed64<F> not fixed64<Dst>, direct use gives wrong value\n");
	check("f32::from_fixed(f16(3.5))  as f16",   (double)f32::from_fixed(f16(3.5)),   3.5);
	check("f48::from_fixed(f16(10))   as f16",   (double)f48::from_fixed(f16(10.0)),  10.0);
	check("f16::from_fixed(f48(5.75)) as f48",   (double)f16::from_fixed(f48(5.75)),  5.75);

	printf("\n[BUG3] high->low negative truncation: div truncates toward zero, not -inf\n");
	{
		auto pos = f16(f32::from_raw(1));
		auto neg = f16(f32::from_raw(-1));
		check("f32(raw=-1)->f16 symmetric with +1",
			(double)std::abs(neg.raw_value()), (double)std::abs(pos.raw_value()), 0);
	}
	{
		int64_t r = -((int64_t(1) << 32) - 1);
		auto pos = f16(f48::from_raw(-r));
		auto neg = f16(f48::from_raw(r));
		check("f48(raw=-(2^32-1))->f16 symmetric",
			(double)std::abs(neg.raw_value()), (double)std::abs(pos.raw_value()), 0);
	}
	{
		auto p = f16(f32(0.3));
		auto n = f16(f32(-0.3));
		check("f32(+/-0.3)->f16 symmetric", (double)n.raw_value(), (double)-p.raw_value(), 0);
	}

	printf("\n==== test_from_fixed: %d passed, %d failed ====\n", pass, fail);
}

auto test = [] {
	printf("==== test begin ====\n");

	test_from_fixed();

	for (int i = 0; i < TEST_COUNT; ++i)
	{
		test_func();
	}
	return 0;
}();
