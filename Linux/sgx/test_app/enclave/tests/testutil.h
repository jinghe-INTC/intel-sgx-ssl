/*
 * Copyright 1995-2021 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef OSSL_TESTUTIL_H
# define OSSL_TESTUTIL_H
#define DEFINE_COMPARISON(type, name, opname, op, fmt)                  \
    int test_ ## name ## _ ## opname(const char *file, int line,        \
                                     const char *s1, const char *s2,    \
                                     const type t1, const type t2)      \
    {                                                                   \
        if (t1 op t2)                                                   \
            return 1;                                                   \
        printf("FAILED: %s\n", __FUNCTION__);                           \
        return 0;                                                       \
    }

#define DEFINE_COMPARISONS(type, name, fmt)                             \
    DEFINE_COMPARISON(type, name, eq, ==, fmt)                          \
    DEFINE_COMPARISON(type, name, ne, !=, fmt)                          \
    DEFINE_COMPARISON(type, name, lt, <, fmt)                           \
    DEFINE_COMPARISON(type, name, le, <=, fmt)                          \
    DEFINE_COMPARISON(type, name, gt, >, fmt)                           \
    DEFINE_COMPARISON(type, name, ge, >=, fmt)

DEFINE_COMPARISONS(int, int, "%d")
DEFINE_COMPARISONS(unsigned int, uint, "%u")
DEFINE_COMPARISONS(char, char, "%c")
DEFINE_COMPARISONS(unsigned char, uchar, "%u")
DEFINE_COMPARISONS(long, long, "%ld")
DEFINE_COMPARISONS(unsigned long, ulong, "%lu")
DEFINE_COMPARISONS(size_t, size_t, "%zu")
DEFINE_COMPARISONS(double, double, "%g")

DEFINE_COMPARISON(void *, ptr, eq, ==, "%p")
DEFINE_COMPARISON(void *, ptr, ne, !=, "%p")

# define TEST_true(a)         test_true(__FILE__, __LINE__, #a, (a) != 0)
# define TEST_false(a)        test_false(__FILE__, __LINE__, #a, (a) != 0)
# define TEST_ptr(a)          test_ptr(__FILE__, __LINE__, #a, a)
# define TEST_mem_eq(a, m, b, n) test_mem_eq(__FILE__, __LINE__, #a, #b, a, m, b, n)
# define TEST_ptr_eq(a, b)    test_ptr_eq(__FILE__, __LINE__, #a, #b, a, b)

# define TEST_int_eq(a, b)    test_int_eq(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_ne(a, b)    test_int_ne(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_lt(a, b)    test_int_lt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_le(a, b)    test_int_le(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_gt(a, b)    test_int_gt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_ge(a, b)    test_int_ge(__FILE__, __LINE__, #a, #b, a, b)


# define TEST_size_t_eq(a, b) test_size_t_eq(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_size_t_ne(a, b) test_size_t_ne(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_size_t_lt(a, b) test_size_t_lt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_size_t_le(a, b) test_size_t_le(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_size_t_gt(a, b) test_size_t_gt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_size_t_ge(a, b) test_size_t_ge(__FILE__, __LINE__, #a, #b, a, b)

#  define TEST_error(...)    test_error(__FILE__, __LINE__, __VA_ARGS__)

#endif
