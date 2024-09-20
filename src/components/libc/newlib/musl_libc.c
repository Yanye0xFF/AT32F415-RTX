/*
 * musl_libc.c
 * @brief 
 * Created on: Sep 22, 2023
 * Author: Yanye
 */
#include "musl_libc.h"

int musl_atoi(const char *s)
{
    int n=0, neg=0;
    while (musl_isspace(*s)) s++;
    switch (*s) {
    case '-':
        neg=1;
        /* fall through */
    case '+':
        s++;
    }
    /* Compute n as a negative number to avoid overflow on INT_MIN */
    while (musl_isdigit(*s))
        n = 10*n - (*s++ - '0');
    return neg ? n : -n;
}
