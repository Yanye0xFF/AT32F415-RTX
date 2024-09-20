/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-03-30     Bernard      the first verion for finsh
 * 2014-01-03     Bernard      msh can execute module.
 * 2017-07-19     Aubr.Cool    limit argc to RT_FINSH_ARG_MAX
 */
#include <rtthread.h>
#include <string.h>
#include <errno.h>

#ifdef RT_USING_FINSH

#ifndef FINSH_ARG_MAX
#define FINSH_ARG_MAX    8
#endif /* FINSH_ARG_MAX */

#include "msh.h"
#include "shell.h"
#ifdef DFS_USING_POSIX
#include <dfs_file.h>
#include <unistd.h>
#include <fcntl.h>
#endif /* DFS_USING_POSIX */
#ifdef RT_USING_MODULE
#include <dlmodule.h>
#endif /* RT_USING_MODULE */

typedef int (*cmd_function_t)(int argc, char **argv);

static int msh_help(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    rt_kprintf("RTX shell commands:\n");
    {
        struct finsh_syscall *index;

        for (index = _syscall_table_begin;
                index < _syscall_table_end;
                FINSH_NEXT_SYSCALL(index))
        {
#if defined(FINSH_USING_DESCRIPTION) && defined(FINSH_USING_SYMTAB)
            rt_kprintf("%-16s - %s\n", index->name, index->desc);
#else
            rt_kprintf("%s ", index->name);
#endif
        }
    }
    rt_kprintf("\n");

    return 0;
}
MSH_CMD_EXPORT_ALIAS(msh_help, help, RT-Thread shell help);

#ifdef MSH_USING_BUILT_IN_COMMANDS
static int cmd_ps(int argc, char **argv)
{
    extern long list_thread(void);
    extern int list_module(void);

#ifdef RT_USING_MODULE
    if ((argc == 2) && (strcmp(argv[1], "-m") == 0))
        list_module();
    else
#endif
    list_thread();
    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_ps, ps, List threads in the system);

#ifdef RT_USING_HEAP
static int cmd_free(int argc, char **argv)
{
#ifdef RT_USING_MEMHEAP_AS_HEAP
    extern void list_memheap(void);
    list_memheap();
#else
    rt_size_t total = 0, used = 0, max_used = 0;

    rt_memory_info(&total, &used, &max_used);
    rt_kprintf("total    : %d\n", total);
    rt_kprintf("used     : %d\n", used);
    rt_kprintf("maximum  : %d\n", max_used);
    rt_kprintf("available: %d\n", total - used);
#endif
    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_free, free, Show the memory usage in the system);
#endif /* RT_USING_HEAP */
#endif /* MSH_USING_BUILT_IN_COMMANDS */

static int msh_split(char *cmd, rt_size_t length, char *argv[FINSH_ARG_MAX])
{
    char *ptr;
    rt_size_t position;
    rt_size_t argc;
    rt_size_t i;

    ptr = cmd;
    position = 0;
    argc = 0;

    while (position < length)
    {
        /* strip bank and tab */
        while ((*ptr == ' ' || *ptr == '\t') && position < length)
        {
            *ptr = '\0';
            ptr ++;
            position ++;
        }

        if (argc >= FINSH_ARG_MAX)
        {
            rt_kprintf("Too many args ! We only Use:\n");
            for (i = 0; i < argc; i++)
            {
                rt_kprintf("%s ", argv[i]);
            }
            rt_kprintf("\n");
            break;
        }

        if (position >= length) break;

        /* handle string */
        if (*ptr == '"')
        {
            ptr ++;
            position ++;
            argv[argc] = ptr;
            argc ++;

            /* skip this string */
            while (*ptr != '"' && position < length)
            {
                if (*ptr == '\\')
                {
                    if (*(ptr + 1) == '"')
                    {
                        ptr ++;
                        position ++;
                    }
                }
                ptr ++;
                position ++;
            }
            if (position >= length) break;

            /* skip '"' */
            *ptr = '\0';
            ptr ++;
            position ++;
        }
        else
        {
            argv[argc] = ptr;
            argc ++;
            while ((*ptr != ' ' && *ptr != '\t') && position < length)
            {
                ptr ++;
                position ++;
            }
            if (position >= length) break;
        }
    }

    return argc;
}

static cmd_function_t msh_get_cmd(char *cmd, int size)
{
    struct finsh_syscall *index;
    cmd_function_t cmd_func = RT_NULL;

    for (index = _syscall_table_begin;
            index < _syscall_table_end;
            FINSH_NEXT_SYSCALL(index))
    {
        if (strncmp(index->name, cmd, size) == 0 &&
                index->name[size] == '\0')
        {
            cmd_func = (cmd_function_t)index->func;
            break;
        }
    }

    return cmd_func;
}

static int _msh_exec_cmd(char *cmd, rt_size_t length, int *retp)
{
    int argc;
    rt_size_t cmd0_size = 0;
    cmd_function_t cmd_func;
    char *argv[FINSH_ARG_MAX];

    RT_ASSERT(cmd);
    RT_ASSERT(retp);

    /* find the size of first command */
    while (cmd0_size < length && (cmd[cmd0_size] != ' ' && cmd[cmd0_size] != '\t'))
        cmd0_size ++;
    if (cmd0_size == 0)
        return -RT_ERROR;

    cmd_func = msh_get_cmd(cmd, cmd0_size);
    if (cmd_func == RT_NULL)
        return -RT_ERROR;

    /* split arguments */
    rt_memset(argv, 0x00, sizeof(argv));
    argc = msh_split(cmd, length, argv);
    if (argc == 0)
        return -RT_ERROR;

    /* exec this command */
    *retp = cmd_func(argc, argv);
    return 0;
}

int msh_exec(char *cmd, rt_size_t length)
{
    int cmd_ret = 0;

    /* strim the beginning of command */
    while ((length > 0) && (*cmd  == ' ' || *cmd == '\t'))
    {
        cmd++;
        length--;
    }

    if (length == 0)
        return 0;

    /* Exec sequence:
     * 1. built-in command
     * 2. module(if enabled)
     */
    if (_msh_exec_cmd(cmd, length, &cmd_ret) == 0)
    {
        return cmd_ret;
    }

    /* truncate the cmd at the first space. */
    {
        char *tcmd;
        tcmd = cmd;
        while (*tcmd != ' ' && *tcmd != '\0')
        {
            tcmd++;
        }
        *tcmd = '\0';
    }

    rt_kprintf("%s: command not found.\n", cmd);

    return -1;
}

static int str_common(const char *str1, const char *str2)
{
    const char *str = str1;

    while ((*str != 0) && (*str2 != 0) && (*str == *str2))
    {
        str ++;
        str2 ++;
    }

    return (str - str1);
}

void msh_auto_complete(char *prefix)
{
    int length, min_length;
    const char *name_ptr, *cmd_name;
    struct finsh_syscall *index;

    min_length = 0;
    name_ptr = RT_NULL;

    if (*prefix == '\0')
    {
        msh_help(0, RT_NULL);
        return;
    }

    /* checks in internal command */
    {
        for (index = _syscall_table_begin; index < _syscall_table_end; FINSH_NEXT_SYSCALL(index))
        {
            /* skip finsh shell function */
            cmd_name = (const char *) index->name;
            if (strncmp(prefix, cmd_name, strlen(prefix)) == 0)
            {
                if (min_length == 0)
                {
                    /* set name_ptr */
                    name_ptr = cmd_name;
                    /* set initial length */
                    min_length = strlen(name_ptr);
                }

                length = str_common(name_ptr, cmd_name);
                if (length < min_length)
                    min_length = length;

                rt_kprintf("%s\n", cmd_name);
            }
        }
    }

    /* auto complete string */
    if (name_ptr != NULL)
    {
        rt_strncpy(prefix, name_ptr, min_length);
    }

    return ;
}
#endif /* RT_USING_FINSH */
