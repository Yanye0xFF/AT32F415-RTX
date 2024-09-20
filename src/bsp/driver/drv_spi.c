/*
 * drv_spi.c
 * @brief
 * Created on: Feb 1, 2024
 * Author: Yanye
 */
#include "drv_spi.h"

#if (USE_ROM_SPI_FUNC == 0)

#include "rtthread.h"
#include "irq_prio.h"
#include "at32f415_gpio.h"
#include "at32f415_spi.h"
#include "at32f415_dma.h"
#include "cmsis_os2.h"
#include "rtx_os.h"

#define SPI_DMA_TIMEOUT  3000

rt_align(4) const uint32_t DUMMY_BYTE = 0xFFFFFFFFu;

static osSemaphoreId_t sem_spi;
rt_align(4) static osRtxSemaphore_t sem_mem;
static const osSemaphoreAttr_t sem_spi_attr = {
    .name = "sem_spi",
    .attr_bits = 0,
    .cb_mem = (void *)&sem_mem,
    .cb_size = sizeof(osRtxSemaphore_t),
};

void DMA2_Channel1_IRQHandler(void) {
    uint32_t temp = DMA2->sts;
    if(temp & DMA1_FDT1_FLAG) {
        DMA2->clr = (uint32_t)(DMA2_FDT1_FLAG & 0x0FFFFFFF);
    }
}

void DMA1_Channel3_IRQHandler(void) {
    uint32_t temp = DMA1->sts;
    if(temp & DMA1_FDT3_FLAG) {
        DMA1->clr = DMA1_FDT3_FLAG;
        osSemaphoreRelease(sem_spi);
    }
}

static int rt_spi_init(void) {
    dma_init_type dma_param;
    spi_init_type spi_param;
    uint32_t reg;

    // crm init
    CRM_REG(CRM_SPI1_PERIPH_CLOCK) |= CRM_REG_BIT(CRM_SPI1_PERIPH_CLOCK);

    // gpio init
    reg = GPIOA->cfglr;
    reg &= 0x0000FFFF;
    // PA5 spi1 sck & PA7 mosi out AF
    // PA6 miso INPUT pd
    // PA4 cs out PP
    reg |= 0xB8B10000;
    GPIOA->clr = GPIO_PINS_6;
    GPIOA->scr = GPIO_PINS_4;
    GPIOA->cfglr = reg;

    SPI1->ctrl1_bit.spien = 0u;
    spi_i2s_reset(SPI1);

    dma_reset(DMA2_CHANNEL1);
    dma_reset(DMA1_CHANNEL3);
    dma_param.buffer_size = 0;
    dma_param.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
    dma_param.memory_base_addr = 0;
    dma_param.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
    dma_param.memory_inc_enable = TRUE;
    dma_param.peripheral_base_addr = (uint32_t)(SPI1_BASE + 0x0C);
    dma_param.peripheral_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
    dma_param.peripheral_inc_enable = FALSE;
    dma_param.priority = DMA_PRIORITY_LOW;
    dma_param.loop_mode_enable = FALSE;
    dma_init(DMA2_CHANNEL1, &dma_param);

    dma_param.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
    dma_init(DMA1_CHANNEL3, &dma_param);

    SPI1->ctrl1_bit.spien = 0u;
    SPI1->ctrl1_bit.ccen = 0;
    spi_param.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
    spi_param.master_slave_mode =SPI_MODE_MASTER;
    spi_param.mclk_freq_division = SPI_MCLK_DIV_2;
    spi_param.first_bit_transmission = SPI_FIRST_BIT_MSB;
    spi_param.frame_bit_num = SPI_FRAME_8BIT;
    spi_param.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    spi_param.clock_phase = SPI_CLOCK_PHASE_1EDGE;
    spi_param.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
    spi_init(SPI1, &spi_param);

    nvic_irq_enable(DMA2_Channel1_IRQn, DMA2_Channel1_PRIO, 0);
    nvic_irq_enable(DMA1_Channel3_IRQn, DMA2_Channel2_PRIO, 0);

    sem_spi = osSemaphoreNew(1u, 0u, &sem_spi_attr);
    return 0;
}
INIT_DEVICE_EXPORT(rt_spi_init);

uint16_t rt_spi_xfer(struct rt_spi_message *message) {
    osStatus_t status;
    uint16_t length = 0;
    uint32_t reg;

    DMA2_CHANNEL1->ctrl_bit.chen = 0u;
    DMA1_CHANNEL3->ctrl_bit.chen = 0u;
    SPI1->ctrl1_bit.spien = 0u;

    osSemaphoreAcquire(sem_spi, 0);

    if(message->cs_take) {
        GPIOA->clr = GPIO_PINS_4;
    }

    if(message->send_buf) {
        DMA1_CHANNEL3->dtcnt = message->length;
        DMA1_CHANNEL3->maddr = (uint32_t)message->send_buf;
        reg = DMA1_CHANNEL3->ctrl;
        reg |= 0x80u;
        DMA1_CHANNEL3->ctrl = reg;
        DMA1->clr = DMA1_FDT3_FLAG;
        DMA1_CHANNEL3->ctrl |= DMA_FDT_INT;

        SPI1->ctrl2_bit.dmaten = 1u;
        SPI1->ctrl1_bit.spien = 1u;
        DMA1_CHANNEL3->ctrl_bit.chen = 1u;

        status = osSemaphoreAcquire(sem_spi, SPI_DMA_TIMEOUT);
        if(status == osOK) {
            while(SPI1->sts_bit.bf) {};
            length = message->length;
        }
        DMA1_CHANNEL3->ctrl &= ~DMA_FDT_INT;
        DMA1_CHANNEL3->ctrl_bit.chen = 0u;

    }else if(message->recv_buf) {
        *(uint8_t *)message->recv_buf = (uint8_t)SPI1->dt;

        DMA1_CHANNEL3->dtcnt = message->length;
        DMA1_CHANNEL3->maddr = (uint32_t)(&DUMMY_BYTE);

        reg = DMA1_CHANNEL3->ctrl;
        reg &= 0xFFFFFF7Fu;
        DMA1_CHANNEL3->ctrl = reg;
        DMA1->clr = DMA1_FDT3_FLAG;
        DMA1_CHANNEL3->ctrl |= DMA_FDT_INT;

        DMA2_CHANNEL1->dtcnt = message->length;
        DMA2_CHANNEL1->maddr = (uint32_t)message->recv_buf;
        DMA2->clr = (uint32_t)(DMA2_FDT1_FLAG & 0x0FFFFFFF);
        DMA2_CHANNEL1->ctrl |= DMA_FDT_INT;

        SPI1->ctrl2 = 0x3u;
        SPI1->ctrl1_bit.spien = 1u;
        DMA2_CHANNEL1->ctrl_bit.chen = 1u;
        DMA1_CHANNEL3->ctrl_bit.chen = 1u;

        status = osSemaphoreAcquire(sem_spi, SPI_DMA_TIMEOUT);
        if(status == osOK) {
            while(SPI1->sts_bit.bf) {};
            length = message->length;
        }
        DMA2_CHANNEL1->ctrl &= ~DMA_FDT_INT;
        DMA2_CHANNEL1->ctrl_bit.chen = 0u;
        DMA1_CHANNEL3->ctrl &= ~DMA_FDT_INT;
        DMA1_CHANNEL3->ctrl_bit.chen = 0u;
    }

    SPI1->ctrl2 = 0x0u;
    SPI1->ctrl1_bit.spien = 0u;

    if(message->cs_release) {
        GPIOA->scr = GPIO_PINS_4;
    }
    return length;
}

#endif
