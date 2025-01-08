//
// Copyright (c) 2024 Mackenzie High. All rights reserved.
//
#ifndef SKIP_UNIT_TESTS

#include "unit_test.h"

/**
 * These are the test cases that can be executed when running tests.
 *
 * I am using a globally allocated array, as the number of test cases
 * is usually well below the set max and doing so avoids the need to
 * have the user of this library explicitly free the array in order
 * to avoid valgrind errors. A unit-test library should be seen and
 * not heard, while also requiring the absolute minimum code on the
 * side of the user.
 */
static unit_test_case_t unit_test_cases[UNIT_TEST_MAX_COUNT];

/**
 * When tests are being executed, this is the index,
 * of the test case that is currently being executed,
 * in the global array of unit test cases.
 */
static int32_t unit_test_current;

static jmp_buf unit_test_jump_point;

int32_t unit_test_get_current ()
{
    return unit_test_current;
}

unit_test_case_t* unit_test_get_cases ()
{
    return (unit_test_case_t*) &unit_test_cases;
}

/**
 * A private function used to implement varadic error messages in assertions.
 */
void unit_test_fail (const char* file, int32_t line, bool has_message, ...)
{
    printf("Failed Assertion:\n");
    printf("    File: %s\n", file);
    printf("    Line: %d\n", line);

    if (has_message)
    {
        printf("    Message: ");
        va_list va;
        va_start(va, has_message);
        char* fmt = va_arg(va, char*);
        vprintf(fmt, va);
        va_end(va);
        printf("\n");
    }

    longjmp(unit_test_jump_point, 1);
}

/**
 * A private function used to implement the declaration of test cases.
 */
void unit_test_declare (const char* suite,
                        const char* name,
                        unit_test_function_t function)
{
    for (int32_t i = 0; i < UNIT_TEST_MAX_COUNT; i++)
    {
        if (unit_test_cases[i].allocated == false)
        {
            unit_test_cases[i].allocated = true;
            unit_test_cases[i].suite = suite;
            unit_test_cases[i].name = name;
            unit_test_cases[i].function = function;
            return;
        }
    }
}

/**
 * A private utility function used to obtain the monotonic current time.
 */
static int64_t unit_test_monotonic ()
{
    struct timespec ts;
    assert (clock_gettime(CLOCK_MONOTONIC, &ts) == 0);
    return (int64_t) ts.tv_sec * (int64_t) 1000000000LL + (int64_t) ts.tv_nsec;
}

/**
 * Invoke this function in order to run the unit tests.
 */
int unit_test_run (int argc, const char** argv)
{
    bool run_tests = false;

    // Only execute unit tests, if the --test flag is given,
    // because the caller may have compiled their program
    // with unit-tests always present and also merely passed
    // us their command-lines arguments; therefore, we want
    // to make their life easier by only executing tests
    // when it is clear that is the desired behavior.
    for (int n = 0; n < argc; n++)
    {
        if (0 == strcmp("--test", argv[n]))
        {
            run_tests = true;
        }
    }

    if (run_tests == false)
    {
        return EXIT_FAILURE;
    }

    // Print help.
    for (int n = 0; n < argc; n++)
    {
        if ((0 == strcmp("--help", argv[n])) || (0 == strcmp("-h", argv[n])))
        {
            printf("Unit Test Help:\n");
            printf("    --test:                        Activate the unit test runner.\n");
            printf("    --all, -a:                     Enable all unit test suites and test cases.\n");
            printf("    --enable-suite  <suite>:       Enable the named unit test suite.\n");
            printf("    --disable-suite <suite>:       Disable the named unit test suite.\n");
            printf("    --enable-case  <suite> <name>: Enable the named unit test case.\n");
            printf("    --disable-case <suite> <name>: Disable the named unit test case.\n");
            printf("    --help, -h:                    Print this help message.\n");
            return EXIT_SUCCESS;
        }
    }

    // By default, disable all test cases.
    for (int i = 0; i < UNIT_TEST_MAX_COUNT && unit_test_cases[i].allocated; i++)
    {
        unit_test_cases[i].enabled = false;
    }

    // Optionally, enable all test cases.
    for (int n = 0; n < argc; n++)
    {
        if ((0 == strcmp("--all", argv[n])) || (0 == strcmp("-a", argv[n])))
        {
            for (int i = 0; i < UNIT_TEST_MAX_COUNT && unit_test_cases[i].allocated; i++)
            {
                unit_test_cases[i].enabled = true;
            }
        }
    }

    // If tests are only being selectively enabled, enabled the name test suite.
    for (int n = 0; n < argc - 1; n++)
    {
        if (0 == strcmp("--enable-suite", argv[n]))
        {
            for (int i = 0; i < UNIT_TEST_MAX_COUNT && unit_test_cases[i].allocated; i++)
            {
                bool suite_match = 0 == strcmp(unit_test_cases[i].suite, argv[n + 1]);
                if (suite_match)
                {
                    unit_test_cases[i].enabled = true;
                }
            }
        }
    }


    // If tests are only being selectively enabled, enable the named test case.
    for (int n = 0; n < argc - 2; n++)
    {
        if (0 == strcmp("--enable-case", argv[n]))
        {
            for (int i = 0; i < UNIT_TEST_MAX_COUNT && unit_test_cases[i].allocated; i++)
            {
                bool suite_match = 0 == strcmp(unit_test_cases[i].suite, argv[n + 1]);
                bool name_match = 0 == strcmp(unit_test_cases[i].name, argv[n + 2]);
                bool match = suite_match && name_match;
                if (match)
                {
                    unit_test_cases[i].enabled = true;
                }
            }
        }
    }

    // If tests are only being selectively enabled, disable the name test suite.
    for (int n = 0; n < argc - 1; n++)
    {
        if (0 == strcmp("--disable-suite", argv[n]))
        {
            for (int i = 0; i < UNIT_TEST_MAX_COUNT && unit_test_cases[i].allocated; i++)
            {
                bool suite_match = 0 == strcmp(unit_test_cases[i].suite, argv[n + 1]);
                if (suite_match)
                {
                    unit_test_cases[i].enabled = false;
                }
            }
        }
    }


    // If tests are only being selectively enabled, disable the named test case.
    for (int n = 0; n < argc - 2; n++)
    {
        if (0 == strcmp("--disable-case", argv[n]))
        {
            for (int i = 0; i < UNIT_TEST_MAX_COUNT && unit_test_cases[i].allocated; i++)
            {
                bool suite_match = 0 == strcmp(unit_test_cases[i].suite, argv[n + 1]);
                bool name_match = 0 == strcmp(unit_test_cases[i].name, argv[n + 2]);
                bool match = suite_match && name_match;
                if (match)
                {
                    unit_test_cases[i].enabled = false;
                }
            }
        }
    }

    int passed = 0;
    int failed = 0;
    int64_t total_elapsed_time_mono_ns = 0;

    for (int i = 0; i < UNIT_TEST_MAX_COUNT && unit_test_cases[i].allocated; i++)
    {
        if (unit_test_cases[i].enabled == false)
        {
            continue; // skip disabled test suites and/or test cases
        }

        unit_test_current = i;

        printf("----------------------------- TEST CASE -----------------------------\n");
        printf("Suite: %s\n", unit_test_cases[i].suite);
        printf("Name: %s\n", unit_test_cases[i].name);

        unit_test_cases[i].result.passed = true;
        unit_test_cases[i].result.end_time_mono_ns = 0;
        unit_test_cases[i].result.elapsed_time_mono_ns = 0;
        unit_test_cases[i].result.start_time_mono_ns = unit_test_monotonic();

        if (setjmp(unit_test_jump_point))
        {
            // Pass. Return point of an assertion jump.
            unit_test_get_cases()[unit_test_get_current()].result.passed = false;
        }
        else
        {
            unit_test_cases[i].function();
        }

        unit_test_cases[i].result.end_time_mono_ns = unit_test_monotonic();
        unit_test_cases[i].result.elapsed_time_mono_ns = unit_test_cases[i].result.end_time_mono_ns - unit_test_cases[i].result.start_time_mono_ns;

        total_elapsed_time_mono_ns += unit_test_cases[i].result.elapsed_time_mono_ns;

        bool ok = unit_test_cases[i].result.passed;

        printf("Elapsed Time: %.4f seconds\n", ((double) unit_test_cases[i].result.elapsed_time_mono_ns) / 1e9);

        if (ok)
        {
            ++passed;
            printf("Result: PASS\n");
        }
        else
        {
            ++failed;
            printf("Result: FAIL\n");
        }

        printf("--------------------------- END TEST CASE ---------------------------\n");
        printf("\n");
        printf("\n");
    }

    int total = passed + failed;

    printf("-------------------------- RESULTS SUMMARY --------------------------\n");
    printf("Passed Tests: %d\n", passed);
    printf("Failed Tests: %d\n", failed);
    printf("Total  Tests: %d\n", total);
    printf("Elapsed Time: %.4f seconds\n", ((double) total_elapsed_time_mono_ns) / 1e9);

    if (passed == total)
    {
        printf("------------------------------ PASSED -------------------------------\n");
        return EXIT_SUCCESS;
    }
    else
    {
        printf("------------------------------ FAILED -------------------------------\n");
        return EXIT_FAILURE;
    }
}

#endif // SKIP_UNIT_TESTS
//
// Copyright (c) 2024 Mackenzie High. All rights reserved.
//
