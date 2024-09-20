/*
 * cstdlib.h
 * @brief
 * Created on: Sep 20, 2024
 * Author: Yanye
 */

#ifndef COMPONENTS_LIBC_NEWLIB_CSTDLIB_H_
#define COMPONENTS_LIBC_NEWLIB_CSTDLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

void __rt_libc_exit(int status);

char *itoa(int n, char *buffer, int radix);
char *lltoa(int64_t ll, char *buffer, int radix);
char *ltoa(long l, char *buffer, int radix);
char *ulltoa(uint64_t ll, char *buffer, int radix);
char *ultoa(unsigned long l, char *buffer, int radix);
char *utoa(unsigned int n, char *buffer, int radix);

#ifdef __cplusplus
}
#endif



#endif /* COMPONENTS_LIBC_NEWLIB_CSTDLIB_H_ */
