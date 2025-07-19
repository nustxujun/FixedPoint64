#define FIXED_64_ENABLE_OVERFLOW 0
#define FIXED_64_ENABLE_INT128_ACCELERATION 0
#define FIXED_64_ENABLE_TRIG_LUT 1
#define FIXED_64_ENABLE_FORCEINLINE 1
#include "fixed64.hpp"
#include <math.h>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <functional>

using fp = float;

struct Counter
{
	using time_t = std::chrono::high_resolution_clock;
	long long& total;
	time_t::time_point time;
	inline Counter(long long& t) :
		total(t)
	{
		time = time_t::now();
	}

	inline ~Counter()
	{
		total += (time_t::now() - time).count();
	}
};

static std::default_random_engine e;

using fixed = f64::fixed64<32>;
struct Operand
{
	fp a;
	fp b;
	fixed fa;
	fixed fb;

	inline void operator()(fp Min, fp Max)
	{
		std::uniform_real_distribution<double> u(Min, Max);
		a = u(e) ;
		b = u(e);
		fa = a;
		fb = b;
		//printf("a: %lf, b: %lf\n", a, b); 
	}
};
static Operand operand;
long long totals[2] = {0,0};
fp prevent_optimized_float = 0;
fixed prevent_optimized_fixed = 0;

FIXED_64_FORCEINLINE void PreventOptimizedAway(fp val)
{
	prevent_optimized_float += val;
}

FIXED_64_FORCEINLINE void PreventOptimizedAway(fixed val)
{
	prevent_optimized_fixed += val;
}

#define TEST_LOOP(EXPR1, EXPR2, COUNT) \
	for (uint64_t i = 0; i < COUNT; i += 0xf){\
		EXPR1;\
		EXPR2;\
		EXPR1;\
		EXPR2;\
		EXPR1;\
		EXPR2;\
		EXPR1;\
		EXPR2;\
		EXPR1;\
		EXPR2;\
		EXPR1;\
		EXPR2;\
		EXPR1;\
		EXPR2;\
		EXPR1;\
	}

// prevent statment reordering
template<class T>
#ifdef _MSC_VER
__declspec(noinline)
#else
__attribute__((noinline))
#endif
void run_test(T& a, T& b, std::function<void(T&, T&)>&& f)
{
	f(a,b);
}



#define RUN_TEST(EXPR1, EXPR2, COUNT, Min, Max) \
{\
	operand(Min, Max);\
	{\
		Counter c(totals[0]);\
		run_test<fp>(operand.a, operand.b, [COUNT](auto& a, auto& b){ TEST_LOOP(EXPR1, EXPR2, COUNT) });\
	}\
	{\
		Counter c(totals[1]);\
		run_test<fixed>(operand.fa, operand.fb,  [COUNT](auto& a, auto& b){ TEST_LOOP(EXPR1, EXPR2, COUNT) });\
	}\
}



struct TestGroup
{
	std::string name;
	uint64_t num_batch;
	uint64_t count;
	fp min;
	fp max;
	TestGroup(std::string n, uint64_t num, uint64_t c, fp min, fp max)
	{
		this->min = min;
		this->max = max;
		name = n;
		num_batch = num;
		count = c;
		totals[0] = 0;
		totals[1] = 0;
	}

	~TestGroup()
	{
		printf("%16s|[%6.1f, %6.1f]| %3.4lf ns | %3.4lf ns |\n",
			name.c_str(),(float)min, (float)max,
			double(totals[1]) /count / num_batch
			,double(totals[0]) / count / num_batch
			);
	}
};

#define RUN_BASIC_TEST_GROUP(NAME, OP1, NUM,COUNT, Min, Max) \
{\
	TestGroup g(NAME, NUM, COUNT, Min, Max);\
	for (uint64_t i = 0; i < NUM; ++i)\
	{\
		RUN_TEST(a OP1##= b, b OP1##= a,COUNT,Min,Max)\
	}\
}

#define RUN_METHOD_TEST_GROUP(NAME, METHOD, NUM,COUNT, Min, Max) \
{\
	TestGroup g(NAME, NUM, COUNT, Min, Max);\
	for (uint64_t i = 0; i < NUM; ++i)\
	{\
		RUN_TEST(PreventOptimizedAway(METHOD), PreventOptimizedAway(METHOD),COUNT,Min,Max)\
	}\
}



auto benchmark = [](){
	printf("==== benchmark begin ====\n");

	printf("enable overflow: %d\n", FIXED_64_ENABLE_OVERFLOW);
	printf("enable int128: %d\n", FIXED_64_ENABLE_INT128_ACCELERATION);
	printf("enable forceinline: %d\n", FIXED_64_ENABLE_FORCEINLINE);

	printf("\n\n");

	e.seed(std::chrono::steady_clock::now().time_since_epoch().count());

	const uint64_t count1 = 0xffff'ff;
	const uint64_t count2 = 0xffff'f;

	printf("           arithmetic|[ min, max]|fixed point| hard float|\n");

	RUN_BASIC_TEST_GROUP("add", +, 0xff, count1, -100, 100);
	RUN_BASIC_TEST_GROUP("sub", -, 0xff, count1, -100, 100);

	RUN_BASIC_TEST_GROUP("mul", *, 0xff, count2, -100,100);
    RUN_BASIC_TEST_GROUP("div", /, 0xff, count2, -100, 100);

	const uint64_t count3 = 0xffff'f;
	using namespace f64;
	RUN_METHOD_TEST_GROUP("ceil", ceil(a), 0xff, count3, -2, 2);
	RUN_METHOD_TEST_GROUP("floor", floor(a), 0xff, count3, -2, 2);
	RUN_METHOD_TEST_GROUP("round", round(a), 0xff, count3, -2, 2);
	RUN_METHOD_TEST_GROUP("abs", abs(a), 0xff, count3, -2, 2);
	RUN_METHOD_TEST_GROUP("exp", exp(a), 0xf, count3, 0, 1);
	RUN_METHOD_TEST_GROUP("exp2", exp2(a), 0xf, count3, 0, 1);
	RUN_METHOD_TEST_GROUP("sqrt", sqrt(a), 0xf, count3, 0, 100);

	RUN_METHOD_TEST_GROUP("sin", sin(a), 0xf, count3, -10, 10);
	RUN_METHOD_TEST_GROUP("cos", cos(a), 0xf, count3, -10, 10);
	RUN_METHOD_TEST_GROUP("tan", tan(a), 0xf, count3, -10, 10);
	RUN_METHOD_TEST_GROUP("asin", asin(a), 0xf, count3, -1, 1);
	RUN_METHOD_TEST_GROUP("acos", acos(a), 0xf, count3, -1, 1);
	RUN_METHOD_TEST_GROUP("atan", atan(a), 0xf, count3, 1, 100);


	return 0;
}();
