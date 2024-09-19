#ifndef __SYSTEM_AT32F415_H
#define __SYSTEM_AT32F415_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup AT32F415_system
  * @{
  */

/** @defgroup AT32F415_system_clock_stable_definition
  * @{
  */

#define HEXT_STABLE_DELAY                (5000u)
#define PLL_STABLE_DELAY                 (500u)
#define SystemCoreClock                  144000000
#define DUMMY_NOP()                      {__NOP();__NOP();__NOP();__NOP();__NOP(); \
                                          __NOP();__NOP();__NOP();__NOP();__NOP(); \
                                          __NOP();__NOP();__NOP();__NOP();__NOP(); \
                                          __NOP();__NOP();__NOP();__NOP();__NOP();}

/**
  * @}
  */

/** @defgroup AT32F415_system_exported_functions
  * @{
  */
extern void SystemInit(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
