/* RT-Thread config file */

#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

/* RT-Thread Kernel */
#define RT_NAME_MAX     12
#define RT_ALIGN_SIZE   4

/* kservice optimization */
#define RT_DEBUG
#define DBG_ENABLE
#define RT_DEBUG_INIT   0
//#define RT_DEBUG_COLOR

/* Memory Management */
#define RT_USING_HEAP
#define RT_USING_SMALL_MEM

/* Kernel Device Object */
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE          128

/* RT-Thread Components */
#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE     2048
#define RT_MAIN_THREAD_PRIORITY       24

#define RT_USING_MSH
#define RT_USING_FINSH

#if defined(RT_USING_FINSH)
    #define FINSH_USING_MSH
    #define FINSH_USING_MSH_ONLY
    #define FINSH_THREAD_NAME "shell"
    //#define FINSH_ECHO_DISABLE_DEFAULT

    #define FINSH_THREAD_PRIORITY       16
    #define FINSH_THREAD_STACK_SIZE     2048

    //#define FINSH_USING_HISTORY
    //#define FINSH_HISTORY_LINES         1
    #define FINSH_CMD_SIZE 80
    #define FINSH_USING_SYMTAB
    #define MSH_USING_BUILT_IN_COMMANDS
    #define FINSH_USING_DESCRIPTION
    #define FINSH_ARG_MAX 8
#endif

/* Device Drivers */
#define RT_USING_RTC
#define RT_LIBC_DEFAULT_TIMEZONE   8
#define USE_MAIN_STACK_WATERMARK   1
#define DEBUG_FAULT_RESET          1
#endif
