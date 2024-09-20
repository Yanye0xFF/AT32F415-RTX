/*
 * spi_flash.c
 * @brief 
 * Created on: Sep 5, 2022
 * Author: Yanye
 */

#include <stdbool.h>
#include "sfud.h"
#include "drv_spi.h"
#include "rtthread.h"
#include "cmsis_os2.h"

/* software version number */
#define SFUD_SW_VERSION                             "1.1.0"
/*
 * all defined supported command
 */
#define SFUD_CMD_WRITE_ENABLE                          0x06
#define SFUD_CMD_WRITE_DISABLE                         0x04
#define SFUD_CMD_READ_STATUS_REGISTER                  0x05
#define SFUD_VOLATILE_SR_WRITE_ENABLE                  0x50
#define SFUD_CMD_WRITE_STATUS_REGISTER                 0x01
#define SFUD_CMD_PAGE_PROGRAM                          0x02
#define SFUD_CMD_AAI_WORD_PROGRAM                      0xAD
#define SFUD_CMD_ERASE_CHIP                            0xC7
#define SFUD_CMD_READ_DATA                             0x03
#define SFUD_CMD_DUAL_OUTPUT_READ_DATA                 0x3B
#define SFUD_CMD_DUAL_IO_READ_DATA                     0xBB
#define SFUD_CMD_QUAD_IO_READ_DATA                     0xEB
#define SFUD_CMD_QUAD_OUTPUT_READ_DATA                 0x6B
#define SFUD_CMD_MANUFACTURER_DEVICE_ID                0x90
#define SFUD_CMD_JEDEC_ID                              0x9F
#define SFUD_CMD_READ_UNIQUE_ID                        0x4B
#define SFUD_CMD_READ_SFDP_REGISTER                    0x5A
#define SFUD_CMD_ENABLE_RESET                          0x66
#define SFUD_CMD_RESET                                 0x99
#define SFUD_SECTOR_ERASE                              0x20
#define SFUD_CMD_ENTER_4B_ADDRESS_MODE                 0xB7
#define SFUD_CMD_EXIT_4B_ADDRESS_MODE                  0xE9
#define SFUD_WRITE_MAX_PAGE_SIZE                        256
/* send dummy data for read data */
#define SFUD_DUMMY_DATA                                0xFF
/* maximum number of erase type support on JESD216 (V1.0) */
#define SFUD_SFDP_ERASE_TYPE_MAX_NUM                      4

static osMutexId_t sfud_lock;

int sfud_init(void) {
    const osMutexAttr_t attr = {
        .name = "sfud",
        .attr_bits = (osMutexRecursive|osMutexPrioInherit|osMutexRobust),
        .cb_mem = NULL,
        .cb_size = 0
    };
    sfud_lock = osMutexNew(&attr);
    return 0;
}
INIT_DEVICE_EXPORT(sfud_init);

sfud_err sfud_get_id(uint32_t *id) {
    const uint8_t cmd = SFUD_CMD_JEDEC_ID;
    struct rt_spi_message message;
    uint8_t buffer[4];
    uint16_t xfer_len;
    osStatus_t sts;
    sfud_err err = SFUD_SUCCESS;

    sts = osMutexAcquire(sfud_lock, SFUD_OPERATION_TIMEOUT);
    if(sts != osOK) {
        return SFUD_ERR_TIMEOUT;
    }

    message.send_buf = (const void *)&cmd;
    message.recv_buf = RT_NULL;
    message.length = 1;
    message.cs_take = true;
    message.cs_release = false;
    xfer_len = rt_spi_xfer(&message);
    if(xfer_len != 1) {
        err = SFUD_ERR_WRITE;
        goto sfud_get_id_end;
    }

    message.send_buf = RT_NULL;
    message.recv_buf = (void *)buffer;
    message.length = 3;
    message.cs_take = false;
    message.cs_release = true;
    xfer_len = rt_spi_xfer(&message);
    if(xfer_len == 3) {
        *id = (buffer[2] | (buffer[1] << 8) | (buffer[0]<< 16));
    }else {
        err = SFUD_ERR_READ;
    }

sfud_get_id_end:
    osMutexRelease(sfud_lock);
    return err;
}

sfud_err sfud_get_unique_id(uint8_t *id, uint32_t length) {
    struct rt_spi_message message;
    uint8_t cmd[8];
    uint16_t xfer_len;
    osStatus_t sts;
    sfud_err err = SFUD_SUCCESS;

    sts = osMutexAcquire(sfud_lock, SFUD_OPERATION_TIMEOUT);
    if(sts != osOK) {
        return SFUD_ERR_TIMEOUT;
    }

    cmd[0] = SFUD_CMD_READ_UNIQUE_ID;
    cmd[1] = 0xFFu; cmd[2] = 0xFFu;
    cmd[3] = 0xFFu; cmd[4] = 0xFFu;

    message.send_buf = (const void *)&cmd;
    message.recv_buf = RT_NULL;
    message.length = 5;
    message.cs_take = true;
    message.cs_release = false;
    xfer_len = rt_spi_xfer(&message);
    if(xfer_len != 5) {
        err = SFUD_ERR_WRITE;
        goto sfud_get_unique_end;
    }

    message.send_buf = RT_NULL;
    message.recv_buf = (void *)id;
    message.length = length;
    message.cs_take = false;
    message.cs_release = true;
    xfer_len = rt_spi_xfer(&message);
    if(xfer_len != length) {
        err = SFUD_ERR_READ;
    }

sfud_get_unique_end:
    osMutexRelease(sfud_lock);
    return err;
}

/**
 * read flash register status
 *
 * @param status register status
 *
 * @return none
 */
static sfud_err sfud_read_status(uint8_t *status) {
    const uint8_t cmd = SFUD_CMD_READ_STATUS_REGISTER;
    struct rt_spi_message message;
    uint16_t xfer_len;

    message.send_buf = (const void *)&cmd;
    message.recv_buf = RT_NULL;
    message.length = 1;
    message.cs_take = 1;
    message.cs_release = 0;
    xfer_len = rt_spi_xfer(&message);
    if(xfer_len != 1) {
        return SFUD_ERR_TIMEOUT;
    }

    message.send_buf = RT_NULL;
    message.recv_buf = (void *)status;
    message.length = 1;
    message.cs_take = 0;
    message.cs_release = 1;
    xfer_len = rt_spi_xfer(&message);
    if(xfer_len != 1) {
        return SFUD_ERR_TIMEOUT;
    }

    return SFUD_SUCCESS;
}

static sfud_err wait_busy(void) {
    sfud_err result;
    uint8_t status;
    int32_t retry_times = SFUD_OPERATION_TIMEOUT;

    while (true) {
        result = sfud_read_status(&status);
        if (result == SFUD_SUCCESS && ((status & SFUD_STATUS_REGISTER_BUSY) == 0)) {
            break;
        }
        /* retry counts */
        if (retry_times == 0) {
            result = SFUD_ERR_TIMEOUT;
            break;
        }else {
            osDelay(1);
            retry_times--;
        }
    }

    return result;
}

/**
 * set the flash write enable or write disable
 *
 * @param enabled true: enable  false: disable
 *
 * @return result
 */
static sfud_err set_write_enabled(bool enabled) {
    sfud_err result;
    uint8_t cmd, register_status;
    struct rt_spi_message message;
    uint16_t xfer_len;

    if (enabled) {
        cmd = SFUD_CMD_WRITE_ENABLE;
    } else {
        cmd = SFUD_CMD_WRITE_DISABLE;
    }

    message.send_buf = (const void *)&cmd;
    message.recv_buf = RT_NULL;
    message.length = 1;
    message.cs_take = 1;
    message.cs_release = 1;
    xfer_len = rt_spi_xfer(&message);
    if(xfer_len != 1) {
        return SFUD_ERR_TIMEOUT;
    }

    result = sfud_read_status(&register_status);

    if (result == SFUD_SUCCESS) {
        if (enabled && (register_status & SFUD_STATUS_REGISTER_WEL) == 0) {
            return SFUD_ERR_WRITE;
        } else if (!enabled && (register_status & SFUD_STATUS_REGISTER_WEL) != 0) {
            return SFUD_ERR_WRITE;
        }
    }

    return result;
}

static void make_adress_byte_array(uint32_t addr, uint8_t *array) {
    uint32_t i;
    for (i = 0; i < 3; i++) {
        array[i] = (addr >> ((3 - (i + 1)) * 8)) & 0xFF;
    }
}

/**
 * read flash data
 *
 * @param addr start address
 * @param size read size
 * @param data read data pointer
 *
 * @return result
 */
sfud_err sfud_read(uint32_t addr, uint32_t size, uint8_t *data) {
    uint16_t xfer_len;
    uint8_t cmd_data[4];
    struct rt_spi_message message;
    osStatus_t sts;
    sfud_err err;

    /* check the flash address bound */
    if ((addr + size) > SFUD_FLASH_CAPACITY) {
        return SFUD_ERR_ADDR_OUT_OF_BOUND;
    }

    sts = osMutexAcquire(sfud_lock, SFUD_OPERATION_TIMEOUT);
    if(sts != osOK) {
        return SFUD_ERR_TIMEOUT;
    }

    err = wait_busy();

    if (err == SFUD_SUCCESS) {
        cmd_data[0] = SFUD_CMD_READ_DATA;
        make_adress_byte_array(addr, &cmd_data[1]);

        message.send_buf = (const void *)cmd_data;
        message.recv_buf = RT_NULL;
        message.length = 4;
        message.cs_take = 1;
        message.cs_release = 0;
        xfer_len = rt_spi_xfer(&message);
        if(xfer_len != 4) {
            err = SFUD_ERR_WRITE;
            goto sfud_read_end;
        }

        message.send_buf = RT_NULL;
        message.recv_buf = (void *)data;
        message.length = size;
        message.cs_take = 0;
        message.cs_release = 1;
        xfer_len = rt_spi_xfer(&message);
        if(xfer_len != size) {
            err = SFUD_ERR_READ;
        }
    }

sfud_read_end:
    osMutexRelease(sfud_lock);

    return err;
}

/**
 * erase flash data
 *
 * @note It will erase align by erase granularity.
 *
 * @param addr start address
 * @param size erase size
 *
 * @return result
 */
sfud_err sfud_erase(uint32_t addr, uint32_t size) {
    uint8_t cmd_data[4];
    uint16_t xfer_len;
    struct rt_spi_message message;
    const uint32_t cur_erase_size = SFUD_FLASH_PAGE_SIZE;
    osStatus_t sts;
    sfud_err err;

    /* check the flash address bound */
    if ((addr + size) > SFUD_FLASH_CAPACITY) {
        return SFUD_ERR_ADDR_OUT_OF_BOUND;
    }

    sts = osMutexAcquire(sfud_lock, SFUD_OPERATION_TIMEOUT);
    if(sts != osOK) {
        return SFUD_ERR_TIMEOUT;
    }

    /* loop erase operate. erase unit is erase granularity */
    while (size) {
        /* set the flash write enable */
        err = set_write_enabled(true);
        if (err != SFUD_SUCCESS) {
            break;
        }

        cmd_data[0] = SFUD_SECTOR_ERASE;
        make_adress_byte_array(addr, &cmd_data[1]);

        message.send_buf = (const void *)cmd_data;
        message.recv_buf = RT_NULL;
        message.length = 4;
        message.cs_take = 1;
        message.cs_release = 1;
        xfer_len = rt_spi_xfer(&message);
        if(xfer_len != 4) {
            err = SFUD_ERR_WRITE;
            break;
        }

        err = wait_busy();
        if (err != SFUD_SUCCESS) {
            break;
        }

        /* make erase align and calculate next erase address */
        if (addr % cur_erase_size != 0) {
            if (size > cur_erase_size - (addr % cur_erase_size)) {
                size -= cur_erase_size - (addr % cur_erase_size);
                addr += cur_erase_size - (addr % cur_erase_size);
            } else {
                break;
            }
        } else {
            if (size > cur_erase_size) {
                size -= cur_erase_size;
                addr += cur_erase_size;
            } else {
                break;
            }
        }
    }

    /* set the flash write disable */
    err = set_write_enabled(false);
    osMutexRelease(sfud_lock);

    return err;
}

/**
 * write flash data (no erase operate)
 *
 * @param addr start address
 * @param size write size
 * @param data write data
 *
 * @return result
 */
sfud_err sfud_write(uint32_t addr, uint32_t size, const uint8_t *data) {
    uint8_t cmd_data[4 + SFUD_WRITE_MAX_PAGE_SIZE];
    const uint32_t write_gran = SFUD_WRITE_MAX_PAGE_SIZE;
    uint32_t data_size;
    uint16_t xfer_len;
    struct rt_spi_message message;
    osStatus_t sts;
    sfud_err err;

    /* check the flash address bound */
    if ((addr + size) > SFUD_FLASH_CAPACITY) {
        return SFUD_ERR_ADDR_OUT_OF_BOUND;
    }
    sts = osMutexAcquire(sfud_lock, SFUD_OPERATION_TIMEOUT);
    if(sts != osOK) {
        return SFUD_ERR_TIMEOUT;
    }
    /* loop write operate. write unit is write granularity */
    while (size) {
        /* set the flash write enable */
        err = set_write_enabled(true);
        if (err != SFUD_SUCCESS) {
            break;
        }

        cmd_data[0] = SFUD_CMD_PAGE_PROGRAM;
        make_adress_byte_array(addr, &cmd_data[1]);

        /* make write align and calculate next write address */
        if (addr % write_gran != 0) {
            if (size > write_gran - (addr % write_gran)) {
                data_size = write_gran - (addr % write_gran);
            } else {
                data_size = size;
            }
        } else {
            if (size > write_gran) {
                data_size = write_gran;
            } else {
                data_size = size;
            }
        }
        size -= data_size;
        addr += data_size;

        rt_memcpy((cmd_data + 4), data, data_size);

        message.send_buf = (const void *)cmd_data;
        message.recv_buf = RT_NULL;
        message.length = (4 + data_size);
        message.cs_take = 1;
        message.cs_release = 1;
        xfer_len = rt_spi_xfer(&message);
        if(xfer_len != message.length) {
            err = SFUD_ERR_WRITE;
            break;
        }

        err = wait_busy();
        if (err != SFUD_SUCCESS) {
            break;
        }
        data += data_size;
    }

    /* set the flash write disable */
    err = set_write_enabled(false);
    osMutexRelease(sfud_lock);

    return err;
}
