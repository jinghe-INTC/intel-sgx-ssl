/*
 * Copyright 1995-2021 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_ptr(const char *file, int line, const char *s, const void *p)
{
    if (p != NULL)
        return 1;
    printf("FAILED: %s\n", __FUNCTION__);//    test_fail_message(NULL, file, line, "ptr", s, "NULL", "!=", "%p", p);
    return 0;
}

int test_true(const char *file, int line, const char *s, int b)
{
    if (b)
        return 1;
    printf("FAILED: %s\n", __FUNCTION__);//    test_fail_message(NULL, file, line, "bool", s, "true", "==", "false");
    return 0;
}

int test_false(const char *file, int line, const char *s, int b)
{
    if (!b)
        return 1;
    printf("FAILED: %s\n", __FUNCTION__);//    test_fail_message(NULL, file, line, "bool", s, "false", "==", "true");
    return 0;
}

int test_mem_eq(const char *file, int line, const char *st1, const char *st2,
                const void *s1, size_t n1, const void *s2, size_t n2)
{
    if (s1 == NULL && s2 == NULL)
        return 1;
    if (n1 != n2 || s1 == NULL || s2 == NULL || memcmp(s1, s2, n1) != 0) {
    printf("FAILED: %s\n", __FUNCTION__);//        test_fail_memory_message(NULL, file, line, "memory", st1, st2, "==",
//                                 s1, n1, s2, n2);
        return 0;
    }
    return 1;
}

void test_error(const char *file, int line, const char *desc, ...)
{
	printf("FAILED: %s\n", __FUNCTION__);
}

