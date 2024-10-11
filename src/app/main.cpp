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
#include "musl_libc.h"
#include "lfs_posix.h"

// PF7
#define SYSLED_CLEAR()     (GPIOF->clr = 0x80u);
#define SYSLED_SET()       (GPIOF->scr = 0x80u);

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

    while(1) {
        SYSLED_CLEAR();
        osDelay(200);
        SYSLED_SET();
        osDelay(200);
    }
}

int spiflash(int argc, char **argv) {
    int addr = 0, size = 0;
    int err, i;
    uint8_t buf[128];

    if(argc == 4) {
        addr = musl_atoi(argv[2]);
        size = musl_atoi(argv[3]);
        if(rt_strcmp("erase", argv[1]) == 0) {
            err = (int)sfud_erase(addr, size);
            rt_kprintf("erase addr:%d, len:%d, state:%d\n", addr, size, err);

        }else if(rt_strcmp("read", argv[1]) == 0) {
            if(size > 128) {
                size = 128;
            }
            err = (int)sfud_read(addr, size, buf);
            rt_kprintf("read addr:%d, len:%d, state:%d", addr, size, err);
            for(i = 0; i < size; i++) {
                if((i & 0x7) == 0) {
                    rt_kprintf("\n");
                }
                rt_kprintf("%02x ", buf[i]);
            }
        }
    }else {
        rt_kprintf("help:\nspiflash erase [addr] [size]\n");
        rt_kprintf("spiflash read [addr] [size(128 max)]\n");
    }
    return 0;
}
MSH_CMD_EXPORT(spiflash, spiflash operation);

int lfsopt(int argc, char **argv) {
    int result = 0;
    int32_t count;
    char buffer[64];
    int fd;
    struct stat sts;

    if(argc < 2) {
        goto lfs_print_help;
    }

    if(rt_strcmp(argv[1], "format") == 0) {
        rt_kprintf("format result:%d\n", result);
        return 0;

    }else if(rt_strcmp(argv[1], "mount") == 0) {
        rt_kprintf("mount result:%d\n", result);
        return 0;

    }else if(rt_strcmp(argv[1], "write") == 0) {
        if(argc < 4) {
            goto lfs_print_help;
        }

        fd = open(argv[2], (O_WRONLY|O_CREAT));
        rt_kprintf("open result:%d\n", fd);

        count = write(fd, (const void *)argv[3], rt_strlen(argv[3]));
        rt_kprintf("write size:%d\n", count);

        result = fsync(fd);
        rt_kprintf("flush result:%d\n", result);

        result = close(fd);
        rt_kprintf("close result:%d\n", result);
        return 0;

    }else if(rt_strcmp(argv[1], "read") == 0) {
        if(argc < 3) {
            goto lfs_print_help;
        }

        fd = open(argv[2], O_RDONLY);
        rt_kprintf("open result:%d\n", fd);

        result = fstat(fd, &sts);
        rt_kprintf("stat result:%d, file size:%d\n", result, sts.st_size);

        count = read(fd, buffer, 64);
        rt_kprintf("read size:%d\n", count);
        if(count > 0) {
            buffer[count] = '\0';
            rt_kprintf("content:%s\n", buffer);
        }

        result = close(fd);
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


/*
>> NO FS
section            size        addr
.isr_vector         384   134217728
.text             23432   134218112
.ARM                  8   134241544
.data               232   536870912
.stack             2048   536871144
.bss               5504   536873192

>> WITH LFS
section            size        addr
.isr_vector         384   134217728
.text             40596   134218112
.ARM                  8   134258708
.data               232   536870912
.stack             2048   536871144
.bss               5640   536873192
*/

