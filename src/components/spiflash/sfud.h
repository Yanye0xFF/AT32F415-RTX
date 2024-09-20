/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: It is an head file for this library. You can see all of the functions which can be called by user.
 * Created on: 2016-04-23
 */

#ifndef _SFUD_H_
#define _SFUD_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SFUD support manufacturer JEDEC ID */
// Do not edit!
#define SFUD_MF_ID_CYPRESS                             0x01
#define SFUD_MF_ID_FUJITSU                             0x04
#define SFUD_MF_ID_EON                                 0x1C
#define SFUD_MF_ID_ATMEL                               0x1F
#define SFUD_MF_ID_MICRON                              0x20
#define SFUD_MF_ID_AMIC                                0x37
#define SFUD_MF_ID_SANYO                               0x62
#define SFUD_MF_ID_INTEL                               0x89
#define SFUD_MF_ID_ESMT                                0x8C
#define SFUD_MF_ID_FUDAN                               0xA1
#define SFUD_MF_ID_HYUNDAI                             0xAD
#define SFUD_MF_ID_SST                                 0xBF
#define SFUD_MF_ID_MACRONIX                            0xC2
#define SFUD_MF_ID_GIGADEVICE                          0xC8
#define SFUD_MF_ID_ISSI                                0xD5
#define SFUD_MF_ID_WINBOND                             0xEF

/* 500ms timeout */
#define SFUD_OPERATION_TIMEOUT        500
#define SFUD_FLASH_CAPACITY           1048576
#define SFUD_FLASH_PAGE_SIZE          4096

/**
 * status register bits
 */
enum {
    SFUD_STATUS_REGISTER_BUSY = (1 << 0),                  /**< busing */
    SFUD_STATUS_REGISTER_WEL = (1 << 1),                   /**< write enable latch */
    SFUD_STATUS_REGISTER_SRP = (1 << 7),                   /**< status register protect */
};

/**
 * error code
 */
typedef enum {
    SFUD_SUCCESS = 0,                                      /**< success */
    SFUD_ERR_NOT_FOUND = 1,                                /**< not found or not supported */
    SFUD_ERR_WRITE = 2,                                    /**< write error */
    SFUD_ERR_READ = 3,                                     /**< read error */
    SFUD_ERR_TIMEOUT = 4,                                  /**< timeout error */
    SFUD_ERR_ADDR_OUT_OF_BOUND = 5,                        /**< address is out of flash bound */
} sfud_err;

/**
 * @brief Read Identification
 * @note MSB ++++++++++++++++++++++++++++++++++++++++++ LSB
 *       | empty | MID7-MID0 | JDID15-JDID8 | JDID7-JDID0 |
 *       for example: gd25q32csig: 0x00C84016
 *                    ZD25WD20BTIGR: 0x00ba6012
 * @param read data pointer
 * @return result
 * */
sfud_err sfud_get_id(uint32_t *id);

/**
 * @brief accesses a factory-set read-only 128bit number that is unique to each device.
 * @param[in] read data pointer
 * @param[in] length read length fixed at 16 bytes
 * @return result
 * */
sfud_err sfud_get_unique_id(uint8_t *id, uint32_t length);

/**
 * read flash data
 *
 * @param addr start address
 * @param size read size
 * @param data read data pointer
 *
 * @return result
 */
sfud_err sfud_read(uint32_t addr, uint32_t size, uint8_t *data);

/**
 * erase flash data
 *
 * @note It will erase align by erase granularity.
 *
 * @param addr start address, align down to 4K
 * @param size erase size, align up to 4K
 *
 * @return result
 */
sfud_err sfud_erase(uint32_t addr, uint32_t size);

/**
 * write flash data (no erase operate)
 *
 * @param addr start address
 * @param data write data
 * @param size write size
 *
 * @return result
 */
sfud_err sfud_write(uint32_t addr, uint32_t size, const uint8_t *data);


#ifdef __cplusplus
}
#endif

#endif
