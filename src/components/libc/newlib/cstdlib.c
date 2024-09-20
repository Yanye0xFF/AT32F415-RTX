/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-15     Meco Man     first version
 */

#include <rtthread.h>
#include "cstdlib.h"

#define DBG_TAG    "stdlib"
#define DBG_LVL    DBG_INFO
#include <rtdbg.h>
#include "cmsis_os2.h"
#include "rtx_os.h"
/**
 * @brief   This function is called when a thread exits. It can detach the thread and perform cleanup.
 *
 * @param   status is the exit status of the thread.
 */
void __rt_libc_exit(int status)
{
    osThreadId_t tid = osThreadGetId();
    osRtxThread_t *self = (osRtxThread_t *)tid;

    if (tid != NULL)
    {
        LOG_E("thread:%s exit:%d!", self->name, status);
        osThreadExit();
    }
}

#ifdef RT_USING_MSH
/**
 * @brief   Execute a command using the Micro-Shell (MSH) subsystem.
 *
 * @param   command is the command string to execute.
 *
 * @return  Returns 0 after executing the command.
 */
int system(const char *command)
{
    extern int msh_exec(char *cmd, rt_size_t length);

    if (command)
    {
        msh_exec((char *)command, rt_strlen(command));
    }

    return 0;
}
#endif /* RT_USING_MSH */

/**
 * @brief   Convert a long integer to a string representation with a specified radix.
 *
 * @param   value is the long integer to convert.
 * @param   string is the destination string where the result will be stored.
 * @param   radix is the base of the number system to be used for conversion.
 *
 * @return  Returns a pointer to the destination string.
 */
char *ltoa(long value, char *string, int radix)
{
    char tmp[33];
    char *tp = tmp;
    long i;
    unsigned long v;
    int sign;
    char *sp;

    if (string == NULL)
    {
        return 0;
    }

    if (radix > 36 || radix <= 1)
    {
        return 0;
    }

    sign = (radix == 10 && value < 0);
    if (sign)
    {
        v = -value;
    }
    else
    {
        v = (unsigned long)value;
    }

    while (v || tp == tmp)
    {
        i = v % radix;
        v = v / radix;
        if (i < 10)
            *tp++ = (char)(i + '0');
        else
            *tp++ = (char)(i + 'a' - 10);
    }

    sp = string;

    if (sign)
        *sp++ = '-';
    while (tp > tmp)
        *sp++ = *--tp;
    *sp = 0;

    return string;
}

/**
 * @brief   Convert an integer to a string representation with a specified radix.
 *
 * @param   value is the integer to convert.
 * @param   string is the destination string where the result will be stored.
 * @param   radix is the base of the number system to be used for conversion.
 *
 * @return  Returns a pointer to the destination string.
 */
char *itoa(int value, char *string, int radix)
{
    return ltoa(value, string, radix);
}

/**
 * @brief   Convert an unsigned long integer to a string representation with a specified radix.
 *
 * @param   value is the unsigned long integer to convert.
 * @param   string is the destination string where the result will be stored.
 * @param   radix is the base of the number system to be used for conversion.
 *
 * @return  Returns a pointer to the destination string.
 */
char *ultoa(unsigned long value, char *string, int radix)
{
    char tmp[33];
    char *tp = tmp;
    long i;
    unsigned long v = value;
    char *sp;

    if (string == NULL)
    {
        return 0;
    }

    if (radix > 36 || radix <= 1)
    {
        return 0;
    }

    while (v || tp == tmp)
    {
        i = v % radix;
        v = v / radix;
        if (i < 10)
            *tp++ = (char)(i + '0');
        else
            *tp++ = (char)(i + 'a' - 10);
    }

    sp = string;

    while (tp > tmp)
        *sp++ = *--tp;
    *sp = 0;

    return string;
}

/**
 * @brief   Convert an unsigned integer to a string representation with a specified radix.
 *
 * @param   value is the unsigned integer to convert.
 * @param   string is the destination string where the result will be stored.
 * @param   radix is the base of the number system to be used for conversion.
 *
 * @return  Returns a pointer to the destination string.
 */
char *utoa(unsigned value, char *string, int radix)
{
    return ultoa(value, string, radix);
}
