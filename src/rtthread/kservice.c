/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-03-16     Bernard      the first version
 * 2006-05-25     Bernard      rewrite vsprintf
 * 2006-08-10     Bernard      add rt_show_version
 * 2010-03-17     Bernard      remove rt_strlcpy function
 *                             fix gcc compiling issue.
 * 2010-04-15     Bernard      remove weak definition on ICCM16C compiler
 * 2012-07-18     Arda         add the alignment display for signed integer
 * 2012-11-23     Bernard      fix IAR compiler error.
 * 2012-12-22     Bernard      fix rt_kprintf issue, which found by Grissiom.
 * 2013-06-24     Bernard      remove rt_kprintf if RT_USING_CONSOLE is not defined.
 * 2013-09-24     aozima       make sure the device is in STREAM mode when used by rt_kprintf.
 * 2015-07-06     Bernard      Add rt_assert_handler routine.
 * 2021-02-28     Meco Man     add RT_KSERVICE_USING_STDLIB
 * 2021-12-20     Meco Man     implement rt_strcpy()
 * 2022-01-07     Gabriel      add __on_rt_assert_hook
 * 2022-06-04     Meco Man     remove strnlen
 * 2022-08-24     Yunjie       make rt_memset word-independent to adapt to ti c28x (16bit word)
 * 2022-08-30     Yunjie       make rt_vsnprintf adapt to ti c28x (16bit int)
 * 2023-02-02     Bernard      add Smart ID for logo version show
 * 2023-10-16     Shell        Add hook point for rt_malloc services
 * 2023-12-10     xqyjlj       perf rt_hw_interrupt_disable/enable, fix memheap lock
 * 2024-03-10     Meco Man     move std libc related functions to rtklibc
 */

#include "rtthread.h"
#include "rthw.h"
#include "cmsis_os2.h"
#include "rtx_os.h"

#define DBG_TAG           "kernel.service"
#ifdef RT_DEBUG_DEVICE
#define DBG_LVL           DBG_LOG
#else
#define DBG_LVL           DBG_WARNING
#endif /* defined (RT_DEBUG_DEVICE) */
#include <rtdbg.h>

#include "rtt.h"

/**
 * @addtogroup KernelService
 * @{
 */

/**
 * @brief This function will show the version of rt-thread rtos
 */
void rt_show_version(void)
{
    rt_kputs("CMSIS-RTX-5.9.0\r\n");
}

#ifdef RT_USING_CONSOLE

rt_weak void rt_hw_console_output(const char *str)
{
    rtt_write(0, str, rt_strlen(str));
}

/**
 * @brief This function will put string to the console.
 *
 * @param str is the string output to the console.
 */
static void _kputs(const char *str, long len)
{
    RT_UNUSED(len);
    rt_hw_console_output(str);
}

/**
 * @brief This function will put string to the console.
 *
 * @param str is the string output to the console.
 */
void rt_kputs(const char *str)
{
    if (!str)
    {
        return;
    }
    rt_hw_console_output(str);
}

/**
 * @brief This function will print a formatted string on system console.
 *
 * @param fmt is the format parameters.
 *
 * @return The number of characters actually written to buffer.
 */
rt_weak int rt_kprintf(const char *fmt, ...)
{
    va_list args;
    rt_size_t length = 0;
    static char rt_log_buf[RT_CONSOLEBUF_SIZE];

    va_start(args, fmt);

    /* the return value of vsnprintf is the number of bytes that would be
     * written to buffer had if the size of the buffer been sufficiently
     * large excluding the terminating null byte. If the output string
     * would be larger than the rt_log_buf, we have to adjust the output
     * length. */
    length = rt_vsnprintf(rt_log_buf, sizeof(rt_log_buf) - 1, fmt, args);
    if (length > RT_CONSOLEBUF_SIZE - 1)
    {
        length = RT_CONSOLEBUF_SIZE - 1;
    }

    _kputs(rt_log_buf, length);

    va_end(args);

    return length;
}
#endif /* RT_USING_CONSOLE */


#if defined(RT_USING_HEAP)

static rt_smem_t system_heap;

rt_inline void _smem_info(rt_size_t *total,
    rt_size_t *used, rt_size_t *max_used)
{
    if (total)
        *total = system_heap->total;
    if (used)
        *used = system_heap->used;
    if (max_used)
        *max_used = system_heap->max;
}

#define _MEM_INIT(_name, _start, _size) \
    system_heap = rt_smem_init(_name, _start, _size)

#define _MEM_MALLOC(_size)  \
    rt_smem_alloc(system_heap, _size)

#define _MEM_REALLOC(_ptr, _newsize)\
    rt_smem_realloc(system_heap, _ptr, _newsize)

#define _MEM_FREE(_ptr) \
    rt_smem_free(_ptr)

#define _MEM_INFO(_total, _used, _max)  \
    _smem_info(_total, _used, _max)

static osMutexId_t _lock;
rt_align(4) static osRtxMutex_t _lock_entity;
static const osMutexAttr_t _lock_attr = {
    .name = "heap",
    .attr_bits = (osMutexRecursive|osMutexPrioInherit|osMutexRobust),
    .cb_mem = (void *)&_lock_entity,
    .cb_size = sizeof(osRtxMutex_t)
};

rt_inline void _heap_lock_init(void)
{
    _lock = osMutexNew(&_lock_attr);
}

rt_inline rt_base_t _heap_lock(void)
{
    if (osThreadGetId()) {
        return osMutexAcquire(_lock, osWaitForever);
    }else {
        return RT_EOK;
    }
}

rt_inline void _heap_unlock(rt_base_t level)
{
    (void)level;
    if (osThreadGetId()) {
        osMutexRelease(_lock);
    }
}

/**
 * @brief This function will init system heap.
 *
 * @param begin_addr the beginning address of system page.
 *
 * @param end_addr the end address of system page.
 */
void rt_system_heap_init(void *begin_addr, void *end_addr)
{
    rt_ubase_t begin_align = RT_ALIGN((rt_ubase_t)begin_addr, RT_ALIGN_SIZE);
    rt_ubase_t end_align   = RT_ALIGN_DOWN((rt_ubase_t)end_addr, RT_ALIGN_SIZE);

    /* Initialize system memory heap */
    _MEM_INIT("heap", (void *)begin_align, end_align - begin_align);
    /* Initialize multi thread contention lock */
    _heap_lock_init();
}

/**
 * @brief Allocate a block of memory with a minimum of 'size' bytes.
 *
 * @param size is the minimum size of the requested block in bytes.
 *
 * @return the pointer to allocated memory or NULL if no free memory was found.
 */
rt_weak void *rt_malloc(rt_size_t size)
{
    rt_base_t level;
    void *ptr;

    /* Enter critical zone */
    level = _heap_lock();
    /* allocate memory block from system heap */
    ptr = _MEM_MALLOC(size);
    /* Exit critical zone */
    _heap_unlock(level);
    return ptr;
}

/**
 * @brief This function will change the size of previously allocated memory block.
 *
 * @param ptr is the pointer to memory allocated by rt_malloc.
 *
 * @param newsize is the required new size.
 *
 * @return the changed memory block address.
 */
rt_weak void *rt_realloc(void *ptr, rt_size_t newsize)
{
    rt_base_t level;
    void *nptr;

    /* Enter critical zone */
    level = _heap_lock();
    /* Change the size of previously allocated memory block */
    nptr = _MEM_REALLOC(ptr, newsize);
    /* Exit critical zone */
    _heap_unlock(level);
    return nptr;
}

/**
 * @brief  This function will contiguously allocate enough space for count objects
 *         that are size bytes of memory each and returns a pointer to the allocated
 *         memory.
 *
 * @note   The allocated memory is filled with bytes of value zero.
 *
 * @param  count is the number of objects to allocate.
 *
 * @param  size is the size of one object to allocate.
 *
 * @return pointer to allocated memory / NULL pointer if there is an error.
 */
rt_weak void *rt_calloc(rt_size_t count, rt_size_t size)
{
    void *p;

    /* allocate 'count' objects of size 'size' */
    p = rt_malloc(count * size);
    /* zero the memory */
    if (p)
    {
        rt_memset(p, 0, count * size);
    }
    return p;
}

/**
 * @brief This function will release the previously allocated memory block by
 *        rt_malloc. The released memory block is taken back to system heap.
 *
 * @param ptr the address of memory which will be released.
 */
rt_weak void rt_free(void *ptr)
{
    rt_base_t level;

    /* NULL check */
    if (ptr == RT_NULL) return;
    /* Enter critical zone */
    level = _heap_lock();
    _MEM_FREE(ptr);
    /* Exit critical zone */
    _heap_unlock(level);
}

/**
* @brief This function will caculate the total memory, the used memory, and
*        the max used memory.
*
* @param total is a pointer to get the total size of the memory.
*
* @param used is a pointer to get the size of memory used.
*
* @param max_used is a pointer to get the maximum memory used.
*/
rt_weak void rt_memory_info(rt_size_t *total,
                            rt_size_t *used,
                            rt_size_t *max_used)
{
    rt_base_t level;

    /* Enter critical zone */
    level = _heap_lock();
    _MEM_INFO(total, used, max_used);
    /* Exit critical zone */
    _heap_unlock(level);
}

/**
 * @brief  This function allocates a memory block, which address is aligned to the
 * specified alignment size.
 *
 * @param  size is the allocated memory block size.
 *
 * @param  align is the alignment size.
 *
 * @return The memory block address was returned successfully, otherwise it was
 *         returned empty RT_NULL.
 */
rt_weak void *rt_malloc_align(rt_size_t size, rt_size_t align)
{
    void *ptr = RT_NULL;
    void *align_ptr = RT_NULL;
    int uintptr_size = 0;
    rt_size_t align_size = 0;

    /* sizeof pointer */
    uintptr_size = sizeof(void*);
    uintptr_size -= 1;

    /* align the alignment size to uintptr size byte */
    align = ((align + uintptr_size) & ~uintptr_size);

    /* get total aligned size */
    align_size = ((size + uintptr_size) & ~uintptr_size) + align;
    /* allocate memory block from heap */
    ptr = rt_malloc(align_size);
    if (ptr != RT_NULL)
    {
        /* the allocated memory block is aligned */
        if (((rt_ubase_t)ptr & (align - 1)) == 0)
        {
            align_ptr = (void *)((rt_ubase_t)ptr + align);
        }
        else
        {
            align_ptr = (void *)(((rt_ubase_t)ptr + (align - 1)) & ~(align - 1));
        }

        /* set the pointer before alignment pointer to the real pointer */
        *((rt_ubase_t *)((rt_ubase_t)align_ptr - sizeof(void *))) = (rt_ubase_t)ptr;

        ptr = align_ptr;
    }

    return ptr;
}

/**
 * @brief This function release the memory block, which is allocated by
 * rt_malloc_align function and address is aligned.
 *
 * @param ptr is the memory block pointer.
 */
rt_weak void rt_free_align(void *ptr)
{
    void *real_ptr = RT_NULL;

    /* NULL check */
    if (ptr == RT_NULL) return;
    real_ptr = (void *) * (rt_ubase_t *)((rt_ubase_t)ptr - sizeof(void *));
    rt_free(real_ptr);
}

#endif /* RT_USING_HEAP */

/**@}*/
