/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-09-20     Bernard      Change the name to components.c
 *                             And all components related header files.
 * 2012-12-23     Bernard      fix the pthread initialization issue.
 * 2013-06-23     Bernard      Add the init_call for components initialization.
 * 2013-07-05     Bernard      Remove initialization feature for MS VC++ compiler
 * 2015-02-06     Bernard      Remove the MS VC++ support and move to the kernel
 * 2015-05-04     Bernard      Rename it to components.c because compiling issue
 *                             in some IDEs.
 * 2015-07-29     Arda.Fu      Add support to use RT_USING_USER_MAIN with IAR
 * 2018-11-22     Jesven       Add secondary cpu boot up
 * 2023-09-15     xqyjlj       perf rt_hw_interrupt_disable/enable
 */

#include <rthw.h>
#include <rtthread.h>
#include "at32f415_misc.h"
#include "cmsis_os2.h"
#include "rtx_os.h"

#ifdef RT_USING_COMPONENTS_INIT
/*
 * Components Initialization will initialize some driver and components as following
 * order:
 * rti_start         --> 0
 * BOARD_EXPORT      --> 1
 * rti_board_end     --> 1.end
 *
 * DEVICE_EXPORT     --> 2
 * COMPONENT_EXPORT  --> 3
 * FS_EXPORT         --> 4
 * ENV_EXPORT        --> 5
 * APP_EXPORT        --> 6
 *
 * rti_end           --> 6.end
 *
 * These automatically initialization, the driver or component initial function must
 * be defined with:
 * INIT_BOARD_EXPORT(fn);
 * INIT_DEVICE_EXPORT(fn);
 * ...
 * INIT_APP_EXPORT(fn);
 * etc.
 */
static int rti_start(void)
{
    return 0;
}
INIT_EXPORT(rti_start, "0");

static int rti_board_start(void)
{
    return 0;
}
INIT_EXPORT(rti_board_start, "0.end");

static int rti_board_end(void)
{
    return 0;
}
INIT_EXPORT(rti_board_end, "1.end");

static int rti_end(void)
{
    return 0;
}
INIT_EXPORT(rti_end, "6.end");

/**
 * @brief  Onboard components initialization. In this function, the board-level
 *         initialization function will be called to complete the initialization
 *         of the on-board peripherals.
 */
void rt_components_board_init(void)
{
#if RT_DEBUG_INIT
    int result;
    const struct rt_init_desc *desc;
    for (desc = &__rt_init_desc_rti_board_start; desc < &__rt_init_desc_rti_board_end; desc ++)
    {
        rt_kprintf("initialize %s", desc->fn_name);
        result = desc->fn();
        rt_kprintf(":%d done\n", result);
    }
#else
    volatile const init_fn_t *fn_ptr;

    for (fn_ptr = &__rt_init_rti_board_start; fn_ptr < &__rt_init_rti_board_end; fn_ptr++)
    {
        (*fn_ptr)();
    }
#endif /* RT_DEBUG_INIT */
}

/**
 * @brief  RT-Thread Components Initialization.
 */
void rt_components_init(void)
{
#if RT_DEBUG_INIT
    int result;
    const struct rt_init_desc *desc;

    rt_kprintf("do components initialization.\n");
    for (desc = &__rt_init_desc_rti_board_end; desc < &__rt_init_desc_rti_end; desc ++)
    {
        rt_kprintf("initialize %s", desc->fn_name);
        result = desc->fn();
        rt_kprintf(":%d done\n", result);
    }
#else
    volatile const init_fn_t *fn_ptr;

    for (fn_ptr = &__rt_init_rti_board_end; fn_ptr < &__rt_init_rti_end; fn_ptr ++)
    {
        (*fn_ptr)();
    }
#endif /* RT_DEBUG_INIT */
}
#endif /* RT_USING_COMPONENTS_INIT */

#if defined(RT_USING_HEAP)
// at32f415rc_flash.ld
// extern unsigned int __bss_start;
extern unsigned int __bss_end;
// size in kb
#define CHIP_SRAM_SIZE      32
#define RT_HW_HEAP_BEGIN    (void *)&__bss_end
#define RT_HW_HEAP_END      (void *)(0x20000000 + CHIP_SRAM_SIZE * 1024)
#endif

#ifdef RT_USING_USER_MAIN

/* if there is not enable heap, we should use static thread and stack. */
static osRtxThread_t main_thread;
rt_align(8) static uint8_t main_thread_stack[RT_MAIN_THREAD_STACK_SIZE];

void main_thread_entry(void *parameter);
extern void user_main(osThreadId_t tid);

/**
 * @brief  This function will create and start the main thread, but this thread
 *         will not run until the scheduler starts.
 */
void rt_application_init(void) {
    const osThreadAttr_t main_attr = {
        .name = "main",
        .attr_bits = osThreadDetached,
        .cb_mem = &main_thread,
        .cb_size = sizeof(osRtxThread_t),
        .stack_mem = main_thread_stack,
        .stack_size = RT_MAIN_THREAD_STACK_SIZE,
        .priority = RT_MAIN_THREAD_PRIORITY, // osPriorityNormal
        .tz_module = 0u,
        .affinity_mask = osThreadProcessor(0)
    };
    // Create application main thread
    osThreadNew(main_thread_entry, NULL, &main_attr);
}

/* Add -eentry to arm-none-eabi-gcc argument */
void entry(void) {
    /* board level initialization */
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    /* System Tick Configuration */
    // @see ../RTOS2/Source/os_systick.c

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(RT_HW_HEAP_BEGIN, RT_HW_HEAP_END);
#endif
    /* show RT-Thread version */
    rt_show_version();

    // Initialize CMSIS-RTOS
    osKernelInitialize();

    /* create init_thread */
    rt_application_init();

    // Start thread execution
    osKernelStart();

    for (;;) {}
}

/**
 * @brief  The system main thread. In this thread will call the rt_components_init()
 *         for initialization of RT-Thread Components and call the user's programming
 *         entry main().
 */
void main_thread_entry(void *parameter) {
    (void)parameter;
#ifdef RT_USING_COMPONENTS_INIT
    /* RT-Thread components initialization */
    rt_components_init();
#endif /* RT_USING_COMPONENTS_INIT */

    osThreadId_t tid = osThreadGetId();
    /* invoke user main function */
    user_main(tid);
}


#endif /* RT_USING_USER_MAIN */
