/*
 * main.c
 * @brief
 * Created on: Sep 19, 2024
 * Author: Yanye
 */
#include "cmsis_os2.h"
#include "rtthread.h"
#include "at32f415_gpio.h"

// PF7
#define SYSLED_CLEAR()     (GPIOF->clr = 0x80u);
#define SYSLED_SET()   (GPIOF->scr = 0x80u);

void user_main(osThreadId_t tid) {
    (void)tid;

    uint32_t reg;
    CRM_REG(CRM_GPIOF_PERIPH_CLOCK) |= CRM_REG_BIT(CRM_GPIOF_PERIPH_CLOCK);
    // sysled: PF7 outpp
    reg = GPIOF->cfglr;
    reg &= 0x0FFFFFFF;
    reg |= 0x20000000;
    GPIOF->cfglr = reg;

    rt_kprintf("user_main\n");

    while(1) {
        SYSLED_CLEAR();
        osDelay(200);
        SYSLED_SET();
        rt_kprintf("hello RTX6\r\n");
        osDelay(200);
    }


}


