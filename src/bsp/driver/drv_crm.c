/*
 * drv_crm.c
 * @brief
 * Created on: Jan 24, 2024
 * Author: Yanye
 */
#include "rtthread.h"
#include "at32f415_crm.h"

int rt_hw_crm_init(void) {
    CRM_REG(CRM_DMA1_PERIPH_CLOCK) |= CRM_REG_BIT(CRM_DMA1_PERIPH_CLOCK);
    DMA1->src_sel0 = 0x0; DMA1->src_sel1 = 0x0;
    DMA1->src_sel1_bit.dma_flex_en = 1u;
    DMA1->src_sel0_bit.ch1_src = DMA_FLEXIBLE_ADC1;
    DMA1->src_sel0_bit.ch2_src = DMA_FLEXIBLE_TMR1_CH1;
    DMA1->src_sel0_bit.ch3_src = DMA_FLEXIBLE_SPI1_TX;
    DMA1->src_sel0_bit.ch4_src = DMA_FLEXIBLE_UART1_TX;
    DMA1->src_sel1_bit.ch5_src = DMA_FLEXIBLE_UART1_RX;
    DMA1->src_sel1_bit.ch6_src = DMA_FLEXIBLE_TMR3_CH1;

    CRM_REG(CRM_DMA2_PERIPH_CLOCK) |= CRM_REG_BIT(CRM_DMA2_PERIPH_CLOCK);
    DMA2->src_sel0 = 0x0; DMA2->src_sel1 = 0x0;
    DMA2->src_sel1_bit.dma_flex_en = 1u;
    DMA2->src_sel0_bit.ch1_src = DMA_FLEXIBLE_SPI1_RX;

    return 0;
}
INIT_BOARD_EXPORT(rt_hw_crm_init);


