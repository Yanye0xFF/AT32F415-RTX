/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-03-18     Bernard      the first version
 * 2006-04-25     Bernard      add rt_hw_context_switch_interrupt declaration
 * 2006-09-24     Bernard      add rt_hw_context_switch_to declaration
 * 2012-12-29     Bernard      add rt_hw_exception_install declaration
 * 2017-10-17     Hichard      add some macros
 * 2018-11-17     Jesven       add rt_hw_spinlock_t
 *                             add smp support
 * 2019-05-18     Bernard      add empty definition for not enable cache case
 * 2023-09-15     xqyjlj       perf rt_hw_interrupt_disable/enable
 * 2023-10-16     Shell        Support a new backtrace framework
 */

#ifndef __RT_HW_H__
#define __RT_HW_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Some macros define
 */
#ifndef HWREG64
#define HWREG64(x)          (*((volatile uint64_t *)(x)))
#endif
#ifndef HWREG32
#define HWREG32(x)          (*((volatile uint32_t *)(x)))
#endif
#ifndef HWREG16
#define HWREG16(x)          (*((volatile uint16_t *)(x)))
#endif
#ifndef HWREG8
#define HWREG8(x)           (*((volatile uint8_t *)(x)))
#endif

#define BIT(n)              ((uint32_t)0x1u << (n))

void rt_hw_cpu_reset(void);
void rt_hw_cpu_shutdown(void);
void rt_hw_console_output(const char *str);

#ifdef __cplusplus
}
#endif

#endif
