/*
 * drv_pin.c
 * @brief
 * Created on: Mar 27, 2022
 * Author: Yanye
 */

#include "drv_pin.h"
#include "rtthread.h"
#include "musl_libc.h"

#include "at32f415.h"
#include "at32f415_crm.h"
#include "at32f415_gpio.h"

static const uint32_t PORT_LUT[] = {GPIOA_BASE, GPIOB_BASE, GPIOC_BASE, GPIOD_BASE, 0, GPIOF_BASE};
static const uint8_t GPIO_SPEED[] = {GPIO_DRIVE_STRENGTH_MODERATE, GPIO_DRIVE_STRENGTH_STRONGER, GPIO_DRIVE_STRENGTH_MAXIMUM};

/**
 * @param name gpio's name, for example: "PA.0"，"PB.15"...
 * @return enum GPIO_PIN_NUM, if error occurs return value < 0
 * */
#define GPIO_NAME_MIN    4
#define GPIO_NAME_MAX    5
int rt_pin_get(const char *name) {
    char buffer[8];
    uint32_t len = rt_strlen(name);
    int pin_num, pin_id;

    if((len < GPIO_NAME_MIN) || (len > GPIO_NAME_MAX)) {
        return -RT_ERROR;
    }
    for(uint32_t i = 0; i < len; i++) {
        buffer[i] = (name[i] | 0x20);
    }
    if((buffer[1] < 'a') || (buffer[1] > 'd')) {
        if(buffer[1] != 'f') {
            return -RT_ERROR;
        }
    }
    pin_id = musl_atoi(name + 3);
    pin_num = ((buffer[1] - 'a') * 16 + pin_id);
    return pin_num;
}

bool rt_pin_mode(int pin, uint32_t mode, uint32_t strength) {
    uint32_t port = PORT_LUT[(pin >> 4)];
    gpio_init_type gpio_param;

    gpio_param.gpio_pins = ((uint32_t)1 << (pin & 0xF));
    gpio_param.gpio_drive_strength = GPIO_SPEED[strength];;
    gpio_param.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;

    if(mode == PIN_MODE_OUTPUT) {
        gpio_param.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_param.gpio_pull = GPIO_PULL_NONE;
    }else if(mode == PIN_MODE_INPUT) {
        // in input mode gpio_out_type is meaningless
        gpio_param.gpio_mode = GPIO_MODE_INPUT;
        gpio_param.gpio_pull = GPIO_PULL_NONE;
    }else if(mode == PIN_MODE_INPUT_PULLUP) {
        gpio_param.gpio_mode = GPIO_MODE_INPUT;
        gpio_param.gpio_pull = GPIO_PULL_UP;
    }else if(mode == PIN_MODE_INPUT_PULLDOWN) {
        gpio_param.gpio_mode = GPIO_MODE_INPUT;
        gpio_param.gpio_pull = GPIO_PULL_DOWN;
    }else if(mode == PIN_MODE_OUTPUT_OD) {
        gpio_param.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_param.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
        gpio_param.gpio_pull = GPIO_PULL_NONE;
    }else {
        return false;
    }
    gpio_init((gpio_type *)port, &gpio_param);
    return true;
}

void rt_pin_write(int pin, uint32_t value) {
    uint32_t port = PORT_LUT[(pin >> 4)];
    const uint8_t REG_OFS[] = {0x14u, 0x10u};
    // GPIOx_CLR, GPIOx_SCR
    REG32(port + REG_OFS[value & 0x1u]) = (uint32_t)0x1 << (pin & 0xF);
}

void rt_pin_toggle(int pin) {
    uint32_t port = PORT_LUT[(pin >> 4)];
    uint32_t p = (pin & 0xF);
    uint32_t odt = REG32(port + 0xC) >> p;
    const uint8_t REG_OFS[] = {0x10u, 0x14u};
    // GPIOx_SCR, GPIOx_CLR, reserve status
    REG32(port + REG_OFS[odt & 0x1u]) = (uint32_t)0x1 << p;
}

int rt_pin_read(int pin) {
    uint32_t port = PORT_LUT[(pin >> 4)];
    uint32_t in = (REG32(port + 0x8) >> (pin & 0xF)) & 0x1;
    return in;
}

int rt_hw_gpio_init(void) {
    /* enable the gpio clock */
    CRM_REG(CRM_IOMUX_PERIPH_CLOCK) |= CRM_REG_BIT(CRM_IOMUX_PERIPH_CLOCK);

    CRM_REG(CRM_GPIOA_PERIPH_CLOCK) |= CRM_REG_BIT(CRM_GPIOA_PERIPH_CLOCK);
    CRM_REG(CRM_GPIOB_PERIPH_CLOCK) |= CRM_REG_BIT(CRM_GPIOB_PERIPH_CLOCK);
    CRM_REG(CRM_GPIOC_PERIPH_CLOCK) |= CRM_REG_BIT(CRM_GPIOC_PERIPH_CLOCK);
    CRM_REG(CRM_GPIOF_PERIPH_CLOCK) |= CRM_REG_BIT(CRM_GPIOF_PERIPH_CLOCK);
    return 0;
}
INIT_BOARD_EXPORT(rt_hw_gpio_init);
