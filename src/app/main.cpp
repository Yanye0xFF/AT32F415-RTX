/*
 * main.c
 * @brief
 * Created on: Sep 19, 2024
 * Author: Yanye
 */
#include "cmsis_os2.h"
#include "rtthread.h"
#include "at32f415_gpio.h"
#include "sfud.h"
#include "lfs_port.h"

// PF7
#define SYSLED_CLEAR()     (GPIOF->clr = 0x80u);
#define SYSLED_SET()   (GPIOF->scr = 0x80u);

extern "C" void user_main(osThreadId_t tid) {
    (void)tid;

    uint32_t reg;
    CRM_REG(CRM_GPIOF_PERIPH_CLOCK) |= CRM_REG_BIT(CRM_GPIOF_PERIPH_CLOCK);

    reg = GPIOF->cfglr;
    reg &= 0x0FFFFFFF;
    reg |= 0x20000000;
    GPIOF->cfglr = reg;

    uint8_t *buf = new uint8_t[32];

    sfud_err err = sfud_get_id(&reg);
    rt_kprintf("result:%d, flashId:0x%08x\n", err, reg);

    rt_kprintf("buf ptr:0x%08x\n", (uint32_t)buf);
    err = sfud_get_unique_id(buf, 16);
    for(uint32_t i = 0; i < 16; i++) {
        rt_kprintf("%02x", buf[i]);
    }
    rt_kprintf("\n");

    lfs_port_init();

    while(1) {
        SYSLED_CLEAR();
        osDelay(200);
        SYSLED_SET();
        osDelay(200);
    }
}

int lfsopt(int argc, char **argv) {
    int result;
    lfs_file_t file;
    struct lfs_info finfo;
    int32_t count;
    char buffer[64];

    if(argc < 2) {
        goto lfs_print_help;
    }

    if(rt_strcmp(argv[1], "format") == 0) {
        result = lfs_format(&lfs_obj, &lfs_cfg);
        rt_kprintf("format result:%d\n", result);
        return 0;

    }else if(rt_strcmp(argv[1], "mount") == 0) {
        result = lfs_mount(&lfs_obj, &lfs_cfg);
        rt_kprintf("mount result:%d\n", result);
        return 0;

    }else if(rt_strcmp(argv[1], "write") == 0) {
        if(argc < 4) {
            goto lfs_print_help;
        }
        result = lfs_file_open(&lfs_obj, &file, argv[2], (LFS_O_WRONLY|LFS_O_CREAT));
        rt_kprintf("open result:%d\n", result);

        count = lfs_file_write(&lfs_obj, &file, (const void *)argv[3], rt_strlen(argv[3]));
        rt_kprintf("write size:%d\n", count);

        result = lfs_file_sync(&lfs_obj, &file);
        rt_kprintf("flush result:%d\n", result);

        result = lfs_file_close(&lfs_obj, &file);
        rt_kprintf("close result:%d\n", result);
        return 0;

    }else if(rt_strcmp(argv[1], "read") == 0) {
        if(argc < 3) {
            goto lfs_print_help;
        }
        result = lfs_file_open(&lfs_obj, &file, argv[2], (LFS_O_RDONLY));
        rt_kprintf("open result:%d\n", result);

        result =  lfs_stat(&lfs_obj, argv[2], &finfo);
        rt_kprintf("stat result:%d, type:%d, file size:%d\n", result, finfo.type, finfo.size);

        count = lfs_file_read(&lfs_obj, &file, buffer, 64);
        rt_kprintf("read size:%d\n", count);
        buffer[count] = '\0';

        rt_kprintf("content:%s\n", buffer);

        result = lfs_file_close(&lfs_obj, &file);
        rt_kprintf("close result:%d\n", result);
        return 0;
    }

lfs_print_help:
    rt_kputs("lfsopt\r\n");
    rt_kputs("-format format disk\r\n");
    rt_kputs("-mount mount disk\r\n");
    rt_kputs("-write [fileName] [text]\r\n");
    rt_kputs("-read fileName\r\n");

    return 0;
}
MSH_CMD_EXPORT(lfsopt, littlefs operation);
