#ifndef __AT32F415_H
#define __AT32F415_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined (__CC_ARM)
 #pragma anon_unions
#endif

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup AT32F415
  * @{
  */

/** @addtogroup Library_configuration_section
  * @{
  */

/**
  * tip: to avoid modifying this file each time you need to switch between these
  *      devices, you can define the device in your toolchain compiler preprocessor.
  */

#if !defined (AT32F415RCT7)   && !defined (AT32F415RCT7_7) && !defined (AT32F415CCT7)   && \
    !defined (AT32F415CCU7)   && !defined (AT32F415KCU7_4) && !defined (AT32F415RBT7)   && \
    !defined (AT32F415RBT7_7) && !defined (AT32F415CBT7)   && !defined (AT32F415CBU7)   && \
    !defined (AT32F415KBU7_4) && !defined (AT32F415R8T7)   && !defined (AT32F415R8T7_7) && \
    !defined (AT32F415C8T7)   && !defined (AT32F415K8U7_4)

    #error "Please select first the target device used in your application (in at32f415.h file)"
#endif

#if defined (AT32F415RCT7)   || defined (AT32F415RCT7_7) || defined (AT32F415CCT7)   || \
    defined (AT32F415CCU7)   || defined (AT32F415KCU7_4) || defined (AT32F415RBT7)   || \
    defined (AT32F415RBT7_7) || defined (AT32F415CBT7)   || defined (AT32F415CBU7)   || \
    defined (AT32F415KBU7_4) || defined (AT32F415R8T7)   || defined (AT32F415R8T7_7) || \
    defined (AT32F415C8T7)   || defined (AT32F415K8U7_4)

    #define AT32F415xx
#endif

/**
  * define with package
  */
#if defined (AT32F415RCT7)   || defined (AT32F415RCT7_7) || defined (AT32F415RBT7)   || \
    defined (AT32F415RBT7_7) || defined (AT32F415R8T7)   || defined (AT32F415R8T7_7)

    #define AT32F415Rx
#endif

#if defined (AT32F415CCT7)   || defined (AT32F415CCU7)   || defined (AT32F415CBT7)   || \
    defined (AT32F415CBU7)   || defined (AT32F415C8T7)

    #define AT32F415Cx
#endif

#if defined (AT32F415KCU7_4) || defined (AT32F415KBU7_4) || defined (AT32F415K8U7_4)

    #define AT32F415Kx
#endif

/**
  * define with memory density
  */
#if defined (AT32F415R8T7)   || defined (AT32F415R8T7_7) || defined (AT32F415C8T7)   || \
    defined (AT32F415K8U7_4)

    #define AT32F415x8
#endif

#if defined (AT32F415RBT7)   || defined (AT32F415RBT7_7) || defined (AT32F415CBT7)   || \
    defined (AT32F415CBU7)   || defined (AT32F415KBU7_4)

    #define AT32F415xB
#endif

#if defined (AT32F415RCT7)   || defined (AT32F415RCT7_7) || defined (AT32F415CCT7)   || \
    defined (AT32F415CCU7)   || defined (AT32F415KCU7_4)

    #define AT32F415xC
#endif

#ifndef USE_STDPERIPH_DRIVER
/**
  * @brief comment the line below if you will not use the peripherals drivers.
  * in this case, these drivers will not be included and the application code will
  * be based on direct access to peripherals registers
  */
  #ifdef _RTE_
    #include "RTE_Components.h"
    #ifdef RTE_DEVICE_STDPERIPH_FRAMEWORK
      #define USE_STDPERIPH_DRIVER
    #endif
  #endif
#endif

/**
  * @brief at32f415 standard peripheral library version number
  */
#define __AT32F415_LIBRARY_VERSION_MAJOR    (0x02) /*!< [31:24] major version */
#define __AT32F415_LIBRARY_VERSION_MIDDLE   (0x01) /*!< [23:16] middle version */
#define __AT32F415_LIBRARY_VERSION_MINOR    (0x01) /*!< [15:8]  minor version */
#define __AT32F415_LIBRARY_VERSION_RC       (0x00) /*!< [7:0]  release candidate */
#define __AT32F415_LIBRARY_VERSION          ((__AT32F415_LIBRARY_VERSION_MAJOR << 24)  | \
                                             (__AT32F415_LIBRARY_VERSION_MIDDLE << 16) | \
                                             (__AT32F415_LIBRARY_VERSION_MINOR << 8)   | \
                                             (__AT32F415_LIBRARY_VERSION_RC))

/**
  * @}
  */

/** @addtogroup Configuration_section_for_CMSIS
  * @{
  */

#include "irq_type.h"
#include "core_cm4.h"
#include "system_at32f415.h"
#include <stdint.h>

/** @addtogroup Exported_types
  * @{
  */

typedef int32_t  INT32;
typedef int16_t  INT16;
typedef int8_t   INT8;
typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t  UINT8;

typedef int32_t  s32;
typedef int16_t  s16;
typedef int8_t   s8;

typedef const int32_t sc32;   /*!< read only */
typedef const int16_t sc16;   /*!< read only */
typedef const int8_t  sc8;    /*!< read only */

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;    /*!< read only */
typedef __I int16_t vsc16;    /*!< read only */
typedef __I int8_t  vsc8;     /*!< read only */

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< read only */
typedef const uint16_t uc16;  /*!< read only */
typedef const uint8_t  uc8;   /*!< read only */

typedef __IO uint32_t vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;   /*!< read only */
typedef __I uint16_t vuc16;   /*!< read only */
typedef __I uint8_t  vuc8;    /*!< read only */

/**
  * @brief flag status
  */
typedef enum {RESET = 0, SET = !RESET} flag_status;

/**
  * @brief confirm state
  */
typedef enum {FALSE = 0, TRUE = !FALSE} confirm_state;

/**
  * @brief error status
  */
typedef enum {ERROR = 0, SUCCESS = !ERROR} error_status;

/**
  * @}
  */

/** @addtogroup Exported_macro
  * @{
  */

#define REG8(addr)                       *(volatile uint8_t *)(addr)
#define REG16(addr)                      *(volatile uint16_t *)(addr)
#define REG32(addr)                      *(volatile uint32_t *)(addr)

#define MAKE_VALUE(reg_offset, bit_num)  (uint32_t)(((reg_offset) << 16) | (bit_num & 0x1F))

#define PERIPH_REG(periph_base, value)   REG32((periph_base + (value >> 16)))
#define PERIPH_REG_BIT(value)            (0x1U << (value & 0x1F))

/**
  * @}
  */

/** @addtogroup Peripheral_memory_map
  * @{
  */

#define FLASH_BASE                       ((uint32_t)0x08000000)
#define SPIM_FLASH_BASE                  ((uint32_t)0x08400000)
#define USD_BASE                         ((uint32_t)0x1FFFF800)
#define SRAM_BASE                        ((uint32_t)0x20000000)
#define PERIPH_BASE                      ((uint32_t)0x40000000)
#define DEBUG_BASE                       ((uint32_t)0xE0042000)

#define APB1PERIPH_BASE                  (PERIPH_BASE + 0x00000)
#define APB2PERIPH_BASE                  (PERIPH_BASE + 0x10000)
#define AHBPERIPH_BASE                   (PERIPH_BASE + 0x20000)

/* apb1 bus base address */
#define TMR2_BASE                        (APB1PERIPH_BASE + 0x0000)
#define TMR3_BASE                        (APB1PERIPH_BASE + 0x0400)
#define TMR4_BASE                        (APB1PERIPH_BASE + 0x0800)
#define TMR5_BASE                        (APB1PERIPH_BASE + 0x0C00)
#define CMP_BASE                         (APB1PERIPH_BASE + 0x2400)
#define ERTC_BASE                        (APB1PERIPH_BASE + 0x2800)
#define WWDT_BASE                        (APB1PERIPH_BASE + 0x2C00)
#define WDT_BASE                         (APB1PERIPH_BASE + 0x3000)
#define SPI2_BASE                        (APB1PERIPH_BASE + 0x3800)
#define USART2_BASE                      (APB1PERIPH_BASE + 0x4400)
#define USART3_BASE                      (APB1PERIPH_BASE + 0x4800)
#define UART4_BASE                       (APB1PERIPH_BASE + 0x4C00)
#define UART5_BASE                       (APB1PERIPH_BASE + 0x5000)
#define I2C1_BASE                        (APB1PERIPH_BASE + 0x5400)
#define I2C2_BASE                        (APB1PERIPH_BASE + 0x5800)
#define CAN1_BASE                        (APB1PERIPH_BASE + 0x6400)
#define PWC_BASE                         (APB1PERIPH_BASE + 0x7000)
/* apb2 bus base address */
#define IOMUX_BASE                       (APB2PERIPH_BASE + 0x0000)
#define EXINT_BASE                       (APB2PERIPH_BASE + 0x0400)
#define GPIOA_BASE                       (APB2PERIPH_BASE + 0x0800)
#define GPIOB_BASE                       (APB2PERIPH_BASE + 0x0C00)
#define GPIOC_BASE                       (APB2PERIPH_BASE + 0x1000)
#define GPIOD_BASE                       (APB2PERIPH_BASE + 0x1400)
#define GPIOF_BASE                       (APB2PERIPH_BASE + 0x1C00)
#define ADC1_BASE                        (APB2PERIPH_BASE + 0x2400)
#define TMR1_BASE                        (APB2PERIPH_BASE + 0x2C00)
#define SPI1_BASE                        (APB2PERIPH_BASE + 0x3000)
#define USART1_BASE                      (APB2PERIPH_BASE + 0x3800)
#define TMR9_BASE                        (APB2PERIPH_BASE + 0x4C00)
#define TMR10_BASE                       (APB2PERIPH_BASE + 0x5000)
#define TMR11_BASE                       (APB2PERIPH_BASE + 0x5400)
#define SDIO1_BASE                       (APB2PERIPH_BASE + 0x8000)
/* ahb bus base address */
#define DMA1_BASE                        (AHBPERIPH_BASE + 0x0000)
#define DMA1_CHANNEL1_BASE               (AHBPERIPH_BASE + 0x0008)
#define DMA1_CHANNEL2_BASE               (AHBPERIPH_BASE + 0x001C)
#define DMA1_CHANNEL3_BASE               (AHBPERIPH_BASE + 0x0030)
#define DMA1_CHANNEL4_BASE               (AHBPERIPH_BASE + 0x0044)
#define DMA1_CHANNEL5_BASE               (AHBPERIPH_BASE + 0x0058)
#define DMA1_CHANNEL6_BASE               (AHBPERIPH_BASE + 0x006C)
#define DMA1_CHANNEL7_BASE               (AHBPERIPH_BASE + 0x0080)
#define DMA2_BASE                        (AHBPERIPH_BASE + 0x0400)
#define DMA2_CHANNEL1_BASE               (AHBPERIPH_BASE + 0x0408)
#define DMA2_CHANNEL2_BASE               (AHBPERIPH_BASE + 0x041C)
#define DMA2_CHANNEL3_BASE               (AHBPERIPH_BASE + 0x0430)
#define DMA2_CHANNEL4_BASE               (AHBPERIPH_BASE + 0x0444)
#define DMA2_CHANNEL5_BASE               (AHBPERIPH_BASE + 0x0458)
#define DMA2_CHANNEL6_BASE               (AHBPERIPH_BASE + 0x046C)
#define DMA2_CHANNEL7_BASE               (AHBPERIPH_BASE + 0x0480)
#define CRM_BASE                         (AHBPERIPH_BASE + 0x1000)
#define FLASH_REG_BASE                   (AHBPERIPH_BASE + 0x2000)
#define CRC_BASE                         (AHBPERIPH_BASE + 0x3000)
#define OTGFS1_BASE                      (PERIPH_BASE + 0x10000000)

/**
  * @}
  */

#include "at32f415_conf.h"
#include "at32f415_def.h"

#ifdef __cplusplus
}
#endif

#endif
