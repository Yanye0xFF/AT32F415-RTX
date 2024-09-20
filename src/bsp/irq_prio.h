/*
 * irq_prio.h
 * @brief
 * Created on: Jun 11, 2024
 * Author: Yanye
 */

#ifndef BSP_IRQ_PRIO_H_
#define BSP_IRQ_PRIO_H_

// @note SYSTICK always set 0xF (lowest priority)

#define CAN1_SE_PRIO    0x0u

// 11 SPI1_RX & SPI1_TX
#define DMA2_Channel1_PRIO   0x0Bu
#define DMA2_Channel2_PRIO   0x0Bu

// 12
#define CAN1_TX_PRIO    0x0Cu
// 13
#define CAN1_RX0_PRIO   0x0Du

// 14 USART_IDLEF & USART_TDC
#define USART1_PRIO    0x0Eu


#endif /* BSP_IRQ_PRIO_H_ */
