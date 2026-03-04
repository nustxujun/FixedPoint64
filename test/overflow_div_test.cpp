#define FIXED_64_ENABLE_OVERFLOW 1
#define FIXED_64_ENABLE_SATURATING 1
#define FIXED_64_ENABLE_TRIG_LUT 0

#include "fixed64.hpp"
#include <cstdio>
#include <cmath>
#include <cstdlib>

#define FRACTION_BITS 32
using namespace f64;
using fixed = fixed64<FRACTION_BITS>;

static int pass_count = 0;
static int fail_count = 0;

static void check(const char* name, bool ok, const char* detail = nullptr)
{
    printf("  %s  %s", ok ? "PASS" : "FAIL", name);
    if (detail) printf("  (%s)", detail);
    printf("\n");
    ok ? pass_count++ : fail_count++;
}

void test_division_overflow_detection()
{
    printf("==== test_division_overflow_detection ====\n");
    printf("Verifies that operator/= detects overflow and saturates correctly.\n");
    printf("Bug: ~fixed_raw(0) >> bit_pos uses arithmetic shift (always -1),\n");
    printf("     so ~(-1) == 0, making the check `div & 0` always false.\n\n");

    constexpr auto MAX_VAL = fixed::MAXIMUM;
    constexpr auto MIN_VAL = fixed::MINIMUM;

    // Case 1: large_positive / tiny_positive => should overflow to MAXIMUM
    {
        fixed a(100000);
        fixed b(0.00001);
        fixed result = a / b;
        // true answer ~= 10,000,000,000, far exceeds max ~= 2,147,483,647
        bool saturated = (result.raw_value() == MAX_VAL);

        char buf[256];
        snprintf(buf, sizeof(buf),
                 "a=100000, b=0.00001, expect saturate to MAX=%lld, got raw=%lld (as double: %.2f)",
                 (long long)MAX_VAL, (long long)result.raw_value(), (double)result);
        check("large_pos / tiny_pos => MAXIMUM", saturated, buf);
    }

    // Case 2: large_negative / tiny_positive => should overflow to MINIMUM
    {
        fixed a(-100000);
        fixed b(0.00001);
        fixed result = a / b;
        bool saturated = (result.raw_value() == MIN_VAL);

        char buf[256];
        snprintf(buf, sizeof(buf),
                 "a=-100000, b=0.00001, expect saturate to MIN=%lld, got raw=%lld (as double: %.2f)",
                 (long long)MIN_VAL, (long long)result.raw_value(), (double)result);
        check("large_neg / tiny_pos => MINIMUM", saturated, buf);
    }

    // Case 3: large_positive / tiny_negative => should overflow to MINIMUM
    {
        fixed a(100000);
        fixed b(-0.00001);
        fixed result = a / b;
        bool saturated = (result.raw_value() == MIN_VAL);

        char buf[256];
        snprintf(buf, sizeof(buf),
                 "a=100000, b=-0.00001, expect saturate to MIN=%lld, got raw=%lld (as double: %.2f)",
                 (long long)MIN_VAL, (long long)result.raw_value(), (double)result);
        check("large_pos / tiny_neg => MINIMUM", saturated, buf);
    }

    // Case 4: MAXIMUM as raw / a fraction < 1 => should overflow
    {
        fixed a = fixed::from_raw(MAX_VAL);
        fixed b(0.5);
        fixed result = a / b;
        bool saturated = (result.raw_value() == MAX_VAL);

        char buf[256];
        snprintf(buf, sizeof(buf),
                 "a=MAX, b=0.5, expect saturate to MAX=%lld, got raw=%lld",
                 (long long)MAX_VAL, (long long)result.raw_value());
        check("MAX_RAW / 0.5 => MAXIMUM", saturated, buf);
    }

    // Case 5: sanity check — normal division should still work correctly
    {
        fixed a(100);
        fixed b(4);
        fixed result = a / b;
        double expected = 25.0;
        double got = (double)result;
        bool ok = std::abs(got - expected) < 0.001;

        char buf[128];
        snprintf(buf, sizeof(buf), "100/4 expect=25, got=%.6f", got);
        check("normal division still correct", ok, buf);
    }

    // Case 6: sanity check — division that is near max but doesn't overflow
    {
        fixed a(2000000000);  // ~2e9, near max int for 32-bit fraction
        fixed b(1);
        fixed result = a / b;
        double expected = 2000000000.0;
        double got = (double)result;
        bool ok = std::abs(got - expected) < 1.0;

        char buf[128];
        snprintf(buf, sizeof(buf), "2e9/1 expect=2e9, got=%.2f", got);
        check("near-max division no overflow", ok, buf);
    }

    printf("\n==== overflow_div_test: %d passed, %d failed ====\n", pass_count, fail_count);
}
