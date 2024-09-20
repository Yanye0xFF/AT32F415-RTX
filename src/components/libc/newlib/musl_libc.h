/*
 * musl_libc.h
 * @brief 
 * Created on: Sep 22, 2023
 * Author: Yanye
 */

#pragma once

/* includes */

#ifdef __cplusplus
extern "C" {
#endif

#define musl_isdigit(a) (((unsigned)(a)-'0') < 10)

static __inline int musl_isspace(int _c)
{
    return _c == ' ' || (unsigned)_c-'\t' < 5;
}

int musl_atoi(const char *s);

#ifdef __cplusplus
}
#endif
