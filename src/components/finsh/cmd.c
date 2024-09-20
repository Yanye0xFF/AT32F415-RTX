/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-04-30     Bernard      first implementation
 * 2006-05-04     Bernard      add list_thread,
 *                                 list_sem,
 *                                 list_timer
 * 2006-05-20     Bernard      add list_mutex,
 *                                 list_mailbox,
 *                                 list_msgqueue,
 *                                 list_event,
 *                                 list_fevent,
 *                                 list_mempool
 * 2006-06-03     Bernard      display stack information in list_thread
 * 2006-08-10     Bernard      change version to invoke rt_show_version
 * 2008-09-10     Bernard      update the list function for finsh syscall
 *                                 list and sysvar list
 * 2009-05-30     Bernard      add list_device
 * 2010-04-21     yi.qiu       add list_module
 * 2012-04-29     goprife      improve the command line auto-complete feature.
 * 2012-06-02     lgnq         add list_memheap
 * 2012-10-22     Bernard      add MS VC++ patch.
 * 2016-06-02     armink       beautify the list_thread command
 * 2018-11-22     Jesven       list_thread add smp support
 * 2018-12-27     Jesven       Fix the problem that disable interrupt too long in list_thread
 *                             Provide protection for the "first layer of objects" when list_*
 * 2020-04-07     chenhui      add clear
 * 2022-07-02     Stanley Lwin add list command
 * 2023-09-15     xqyjlj       perf rt_hw_interrupt_disable/enable
 * 2024-02-09     Bernard      fix the version command
 */

#include <rthw.h>
#include <rtthread.h>
#include <string.h>

#include "cmsis_os2.h"
#include "rtx_os.h"
#include "core_cm4.h"

#ifdef RT_USING_FINSH
#include <finsh.h>

#define LIST_FIND_OBJ_NR 8
#define LIST_THREAD_NR   16
static const int maxlen = RT_NAME_MAX;

static long version(void)
{
    rt_show_version();

    return 0;
}
MSH_CMD_EXPORT(version, show RTX version information);

rt_inline void object_split(int len)
{
    while (len--) rt_kprintf("-");
}

int list_thread(void) {
    const char *item_title = "thread";

    osRtxThread_t *thread;
    uint8_t thread_state;
    uint32_t stack_size;
    uint32_t *ptr;
    osThreadId_t buffer[LIST_THREAD_NR];
    uint32_t thread_count, enum_count;

    thread_count = osThreadGetCount();
    if (thread_count > LIST_THREAD_NR) {
        thread_count = LIST_THREAD_NR;
        rt_kprintf("thread count larger than buffer size !\n");
    }

    enum_count = osThreadEnumerate(buffer, thread_count);

    rt_kprintf("%-*.s pri  status      sp     stack size stack base max-used left-tick\n",
            maxlen, item_title);
    object_split(maxlen);
    rt_kprintf(" ---  ------- ---------- ---------- ----------  ------  ----------\n");

    for (uint32_t i = 0; i < enum_count; i++) {
        thread = (osRtxThread_t*) buffer[i];
        thread_state = (thread->state & osRtxThreadStateMask);
        rt_kprintf("%-*.*s %3d ", maxlen, RT_NAME_MAX, thread->name, thread->priority);

        if (thread_state == osRtxThreadInactive)
            rt_kprintf(" inactive  ");
        else if (thread_state == osRtxThreadReady)
            rt_kprintf(" ready  ");
        else if (thread_state == osRtxThreadRunning)
            rt_kprintf(" running");
        else if (thread_state == osRtxThreadBlocked)
            rt_kprintf(" suspend");
        else if (thread_state == osRtxThreadTerminated)
            rt_kprintf(" close  ");

        // 1 Magic Word + 16 Regs
        stack_size = (thread->stack_size - (17 << 2));
        ptr = (uint32_t*) thread->stack_mem;
        for (uint32_t n = (thread->stack_size / 4U) - (16U + 1U); n != 0U; n--) {
            // first: move next to skip magic word.
            ptr++;
            // second: check watermark
            if (osRtxStackFillPattern != *ptr) {
                break;
            }
        }

        rt_kprintf(" 0x%08x 0x%08x 0x%08x    %02d%%     %8d\n", thread->sp,
                thread->stack_size, (uint32_t) thread->stack_mem,
                (stack_size - ((uint32_t) ptr - (uint32_t) thread->stack_mem))
                        * 100 / stack_size, thread->delay);
    }

    return 0;
}
MSH_CMD_EXPORT(list_thread, list thread);

int list_timer(void) {
    const char *item_title = "timer";
    osRtxTimer_t *timer = osRtxInfo.timer.list;

    if (timer == NULL) {
        rt_kprintf("no soft timer.\n");
        return 0;
    }

    rt_kprintf("%-*.s  periodic   timeout    activated     mode\n", maxlen, item_title);
    object_split(maxlen);
    rt_kprintf(" ---------- ---------- ----------- ---------\n");

    do {
        rt_kprintf("%-*.*s   %8d   %8d ", maxlen, RT_NAME_MAX, timer->name,
                timer->load, timer->tick);
        if (timer->state == osRtxTimerInactive) {
            rt_kprintf("Inactive    ");
        } else if (timer->state == osRtxTimerStopped) {
            rt_kprintf("Stopped     ");
        } else if (timer->state == osRtxTimerRunning) {
            rt_kprintf("Running     ");
        } else {
            rt_kprintf("Unknown     ");
        }

        if (timer->attr & 0x1u) {
            rt_kprintf("periodic\n");
        } else {
            rt_kprintf("one shot\n");
        }

        timer = timer->next;
    } while (timer != NULL);

    return 0;
}
MSH_CMD_EXPORT(list_timer, list timer in system);

// startup_at32f415.S
extern void Default_Handler(void);
// at32f415rc_flash.ld
extern unsigned int __link_addr;

// 15 arm core exception + 55 periph interrupt
#define PERIPH_INTERRUPT_NUMS    76
// some core exception not defined in arm cortex-m4
#define CORE_EXCEPTION_NUMS      10

int list_isr(void) {
    const char *item_title = "interrupt";
    const uint8_t core_exception_offset[] = { 4, 4, 4, 4, 4, 4, 20, 4, 8, 4 };
    const int8_t core_exception_num[] =
            { 0, 0, 0, -12, -11, -10, -5, -4, -2, -1 };
    const char *const exception_name[] = { "Reset", "NMI", "HardFault",
            "MemManage", "BusFault", "UsageFault", "SVC", "DebugMon", "PendSV",
            "SysTick" };

    uint32_t grouping = NVIC_GetPriorityGrouping();
    uint32_t pre_bit = (7u - grouping);
    uint32_t sub_bit = (grouping - 3u);

    rt_kprintf("%d bits for pre-emption priority %d bits for subpriority.\n",
            pre_bit, sub_bit);

    uint32_t addr = (uint32_t) &__link_addr;
    const uint32_t addr_default = (const uint32_t) Default_Handler;
    uint32_t func_ptr;
    uint32_t priority;

    rt_kprintf("link address:0x%08x\n", addr);
    rt_kprintf("default handler:0x%08x\n", addr_default);

    rt_kprintf("%-*.s  pri   handler\n", maxlen, item_title);
    object_split(maxlen);
    rt_kprintf(" ---- ----------\n");

    for (size_t i = 0; i < CORE_EXCEPTION_NUMS; i++) {
        addr += core_exception_offset[i];
        func_ptr = HWREG32(addr) & 0xFFFFFFFEu;

        if (core_exception_num[i] == 0) {
            rt_kprintf("%-*.*s    - ", maxlen, RT_NAME_MAX, exception_name[i]);
        } else {
            priority = NVIC_GetPriority((IRQn_Type) core_exception_num[i]);
            rt_kprintf("%-*.*s %4d ", maxlen, RT_NAME_MAX, exception_name[i],
                    priority);
        }
        if (func_ptr == addr_default) {
            rt_kprintf("   default\n");
        } else {
            rt_kprintf("0x%08x\n", func_ptr);
        }
    }
    object_split(maxlen);
    rt_kprintf("----------------\n");
    // periph interrupt
    addr += sizeof(size_t);

    for (size_t i = 0; i < PERIPH_INTERRUPT_NUMS; i++) {
        if ((i & 0xF) == 0) {
            osDelay(100);
        }
        func_ptr = HWREG32(addr) & 0xFFFFFFFEu;
        priority = NVIC_GetPriority((IRQn_Type) i);
        rt_kprintf("IRQ:%02d    Prio:%2d ", i, priority);
        if (func_ptr == addr_default) {
            rt_kprintf("   default\n");
        } else {
            rt_kprintf("0x%08x\n", func_ptr);
        }
        addr += sizeof(size_t);
    }
    return 0;
}
MSH_CMD_EXPORT(list_isr, list interrupt handlers);

#if (USE_MAIN_STACK_WATERMARK)
// at32f415rc_flash.ld
extern unsigned int _sstack;
extern unsigned int _estack;
extern unsigned int _system_stack_size;

long mstack(void) {
    uint32_t *msp_bottom = (uint32_t *)&_sstack;
    uint32_t *msp_top = (uint32_t *)&_estack;
    uint32_t stack_size = (uint32_t)&_system_stack_size;
    uint32_t *msp_ptr = msp_bottom;

    while(msp_ptr < msp_top) {
        // watermark set by /libcpu/startup_gd32f30x_hd.S
        if(*msp_ptr != 0xCCCCCCCC) {
            break;
        }
        msp_ptr++;
    }

    uint32_t percent = ((uint32_t)msp_top - (uint32_t)msp_ptr) * 100 / stack_size;

    rt_kprintf("main stack info:\n");
    rt_kprintf("stack size : %d Bytes\n", stack_size);
    rt_kprintf("stack base : 0x%08x\n", (uint32_t)msp_bottom);
    rt_kprintf("stack top  : 0x%08x\n", (uint32_t)msp_top);
    rt_kprintf("watermark  : 0x%08x\n", (uint32_t)msp_ptr);
    rt_kprintf("max used   : %d%%\n", percent);

    return 0;
}
MSH_CMD_EXPORT(mstack, show main stack usage);
#endif

static int cmd_list(int argc, char **argv)
{
    if(argc == 2)
    {
        if(strcmp(argv[1], "thread") == 0)
        {
            list_thread();
        }
        else if(strcmp(argv[1], "timer") == 0)
        {
            list_timer();
        }
        else
        {
            goto _usage;
        }

        return 0;
    }

_usage:
    rt_kprintf("Usage: list [options]\n");
    rt_kprintf("[options]:\n");
    rt_kprintf("    %-12s - list threads\n", "thread");
    rt_kprintf("    %-12s - list timers\n", "timer");

    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_list, list, list objects, optenable);

#endif /* RT_USING_FINSH */
