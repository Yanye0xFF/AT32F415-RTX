/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-03-30     Bernard      the first verion for FinSH
 */

#ifndef __M_SHELL__
#define __M_SHELL__

#include <rtthread.h>

#ifdef __cplusplus
extern "C" {
#endif

int msh_exec(char *cmd, rt_size_t length);
void msh_auto_complete(char *prefix);

#ifdef __cplusplus
}
#endif

#endif
