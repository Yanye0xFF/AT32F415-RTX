/*
 * drv_spi.h
 * @brief
 * Created on: Feb 1, 2024
 * Author: Yanye
 */

#ifndef BSP_DRIVER_DRV_SPI_H_
#define BSP_DRIVER_DRV_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define USE_ROM_SPI_FUNC    0

/**
 * SPI message structure
 */
struct rt_spi_message {
    const void *send_buf;
    void *recv_buf;
    uint16_t length;
    uint8_t cs_take : 1;
    uint8_t cs_release : 1;
};

#if (USE_ROM_SPI_FUNC == 1)
extern uint16_t rt_spi_xfer(struct rt_spi_message *message);
#else
uint16_t rt_spi_xfer(struct rt_spi_message *message);
#endif

#ifdef __cplusplus
}
#endif

#endif /* BSP_DRIVER_DRV_SPI_H_ */
