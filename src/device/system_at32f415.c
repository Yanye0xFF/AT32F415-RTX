/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup AT32F415_system
  * @{
  */

#include "at32f415.h"
#include "at32f415_flash.h"
#include "at32f415_crm.h"

/** @addtogroup AT32F415_system_private_defines
  * @{
  */
#define VECT_TAB_OFFSET    0x0 /*!< vector table base offset field. this value must be a multiple of 0x200. */
/**
  * @}
  */

/**
  * @brief  setup the microcontroller system
  *         initialize the flash interface.
  * @note   this function should be used only after reset.
  * @param  none
  * @retval none
  */
void SystemInit (void) {
    /* enable low power mode */
    CRM->apb1en_bit.pwcen = 1;
    *(volatile uint8_t *)(0x40007050) |= (uint8_t)(0x1 << 2);
    CRM->apb1en_bit.pwcen = 0;

    /* reset the crm clock configuration to the default reset state(for debug purpose) */
    /* set hicken bit */
    CRM->ctrl_bit.hicken = TRUE;

    /* wait hick stable */
    while(CRM->ctrl_bit.hickstbl != SET);

    /* hick used as system clock */
    CRM->cfg_bit.sclksel = CRM_SCLK_HICK;

    /* wait sclk switch status */
    while(CRM->cfg_bit.sclksts != CRM_SCLK_HICK);

    /* reset hexten, hextbyps, cfden and pllen bits */
    CRM->ctrl &= ~(0x010D0000U);

    /* reset cfg register, include sclk switch, ahbdiv, apb1div, apb2div, adcdiv,
    clkout pllrcs, pllhextdiv, pllmult, usbdiv and pllrange bits */
    CRM->cfg = 0;

    /* reset pllfr, pllms, pllns and pllfref bits */
    CRM->pll = (0x00001F10U);

    /* reset clkout[3], usbbufs, hickdiv, clkoutdiv */
    CRM->misc1 = 0x00100000;

    /* disable all interrupts enable and clear pending bits  */
    CRM->clkint = 0x009F0000;

    #ifdef VECT_TAB_SRAM
        SCB->VTOR = SRAM_BASE  | VECT_TAB_OFFSET;  /* vector table relocation in internal sram. */
    #else
        SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;  /* vector table relocation in internal flash. */
    #endif

    // the system clock is configured as follow:
    // system clock (sclk) = hext / 2 * pll_mult = 8 / 2 * 36 = 144M
    // system clock source = HEXT_VALUE
    // hext = 8000000, sclk = 144000000, pll_mult = 36
    // ahbdiv = 1, ahbclk = 144000000
    // apb1div = 2, apb1clk = 72000000
    // apb2div = 2, apb2clk = 72000000
    // flash_wtcyc = 4 cycle

    /* config flash psr register */
    flash_psr_set(FLASH_WAIT_CYCLE_4);

    /* enable hext */
    CRM->ctrl_bit.hexten = TRUE;

    /* wait till hext is ready */
    while((CRM_REG(CRM_HEXT_STABLE_FLAG) & CRM_REG_BIT(CRM_HEXT_STABLE_FLAG)) != CRM_REG_BIT(CRM_HEXT_STABLE_FLAG));

    /* config pll clock source */
    CRM->cfg_bit.pllrcs = TRUE;
    CRM->cfg_bit.pllhextdiv = TRUE;

    /* config pll multiplication factor */
    CRM->cfg_bit.pllmult_l = (CRM_PLL_MULT_36 & 0x0F);
    CRM->cfg_bit.pllmult_h = ((CRM_PLL_MULT_36 & 0x30) >> 4);

    /* config pll fref */
    CRM->pll_bit.pllfref = CRM_PLL_FREF_4M;

    /* enable pll */
    CRM->ctrl_bit.pllen = TRUE;

    /* wait till pll is ready */
    while((CRM_REG(CRM_PLL_STABLE_FLAG) & CRM_REG_BIT(CRM_PLL_STABLE_FLAG)) != CRM_REG_BIT(CRM_PLL_STABLE_FLAG));

    /* config ahbclk */
    CRM->cfg_bit.ahbdiv = CRM_AHB_DIV_1;
    /* config apb2clk */
    CRM->cfg_bit.apb2div = CRM_APB2_DIV_2;
    /* config apb1clk */
    CRM->cfg_bit.apb1div = CRM_APB1_DIV_2;
    /* enable auto step mode */
    CRM->misc2_bit.auto_step_en = CRM_AUTO_STEP_MODE_ENABLE;

    /* select pll as system clock source */
    CRM->cfg_bit.sclksel = CRM_SCLK_PLL;
    DUMMY_NOP();

    /* wait till pll is used as system clock source */
    while((CRM->cfg_bit.sclksts) != CRM_SCLK_PLL);

    /* disable auto step mode */
    CRM->misc2_bit.auto_step_en = CRM_AUTO_STEP_MODE_DISABLE;
}
