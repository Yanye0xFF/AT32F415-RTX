/*
 * drv_pin.h
 * @brief gpio driver for at32f415rct7-LQFP64
 * Created on: Mar 27, 2022
 * Author: Yanye
 */
#ifndef BSP_DRIVER_DRV_PIN_H_
#define BSP_DRIVER_DRV_PIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define GPIO_PIN_LOW            0x00
#define GPIO_PIN_HIGH           0x01

#define PIN_MODE_OUTPUT         0x00
#define PIN_MODE_INPUT          0x01
#define PIN_MODE_INPUT_PULLUP   0x02
#define PIN_MODE_INPUT_PULLDOWN 0x03
#define PIN_MODE_OUTPUT_OD      0x04

#define PIN_SLEWRATE_LOW        0x00
#define PIN_SLEWRATE_MEDIUM     0x01
#define PIN_SLEWRATE_HIGH       0x02

// AT32F415RCT7 available pins
// GPIO_PA0 ~ GPIO_PA15
// GPIO_PB0 ~ GPIO_PB15
// GPIO_PC0 ~ GPIO_PC15
// [GPIO_PD0, GPIO_PD1 (HSE)], GPIO_PD2
// GPIO_PF6, GPIO_PF7

enum GPIO_PIN_NUM {
    GPIO_PA0 = 0, GPIO_PA1, GPIO_PA2, GPIO_PA3, GPIO_PA4, GPIO_PA5, GPIO_PA6, GPIO_PA7,
    GPIO_PA8, GPIO_PA9, GPIO_PA10, GPIO_PA11, GPIO_PA12, GPIO_PA13, GPIO_PA14, GPIO_PA15,

	GPIO_PB0, GPIO_PB1, GPIO_PB2, GPIO_PB3, GPIO_PB4, GPIO_PB5, GPIO_PB6, GPIO_PB7,
	GPIO_PB8, GPIO_PB9, GPIO_PB10, GPIO_PB11, GPIO_PB12, GPIO_PB13, GPIO_PB14, GPIO_PB15,

	GPIO_PC0, GPIO_PC1, GPIO_PC2, GPIO_PC3, GPIO_PC4, GPIO_PC5, GPIO_PC6, GPIO_PC7,
	GPIO_PC8, GPIO_PC9, GPIO_PC10, GPIO_PC11, GPIO_PC12, GPIO_PC13, GPIO_PC14, GPIO_PC15,

	GPIO_PD0, GPIO_PD1, GPIO_PD2,

	GPIO_PF4 = 84, GPIO_PF5, GPIO_PF6, GPIO_PF7,

	GPIO_MAX = 0x7fffffff
};

int rt_pin_get(const char *name);
bool rt_pin_mode(int pin, uint32_t mode, uint32_t strength);
int rt_pin_read(int pin);
void rt_pin_write(int pin, uint32_t value);
void rt_pin_toggle(int pin);

#ifdef __cplusplus
}
#endif

#endif /* BSP_DRIVER_DRV_PIN_H_ */
