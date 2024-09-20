/*
 * lfs_port.c
 * @brief
 * Created on: Sep 20, 2024
 * Author: Yanye
 */

#include "lfs_port.h"
#include <stddef.h>
#include "cmsis_os2.h"
#include "sfud.h"

int lfs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
int lfs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
int lfs_erase(const struct lfs_config *c, lfs_block_t block);
int lfs_sync(const struct lfs_config *c);
int lfs_lock(const struct lfs_config *c);
int lfs_unlock(const struct lfs_config *c);

const struct lfs_config lfs_cfg = {
    .context = NULL,
    .read = lfs_read,
    .prog = lfs_prog,
    .erase = lfs_erase,
    .sync = lfs_sync,
    .lock = lfs_lock,
    .unlock = lfs_unlock,
    .read_size = 1,
    .prog_size = 256,
    .block_size = 4096,
    .block_count = 256,
    .block_cycles = 1000,
    .cache_size = 1024,
    .lookahead_size = 32,
    .compact_thresh = 3604,
    .name_max = LFS_NAME_MAX,
    .file_max = LFS_FILE_MAX,
    .attr_max = LFS_ATTR_MAX,
    .metadata_max = 4096,
    .inline_max = 512,
};

lfs_t lfs_obj;

static osMutexId_t lfs_mtx;

int lfs_port_init(void) {
    const osMutexAttr_t attr = {
        .name = "lfs",
        .attr_bits = (osMutexRecursive|osMutexPrioInherit|osMutexRobust),
        .cb_mem = NULL,
        .cb_size = 0
    };
    lfs_mtx = osMutexNew(&attr);

    return 0;
}

int lfs_read(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size) {
    // lfs_error
    sfud_err err = sfud_read(4096 * block + off, size, (uint8_t *)buffer);
    return (err == SFUD_SUCCESS) ? LFS_ERR_OK : LFS_ERR_IO;
}

int lfs_prog(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size) {

    sfud_err err = sfud_write(4096 * block + off, size, (const uint8_t *)buffer);
    return (err == SFUD_SUCCESS) ? LFS_ERR_OK : LFS_ERR_IO;
}

int lfs_erase(const struct lfs_config *c, lfs_block_t block) {
    sfud_err err = sfud_erase(4096 * block, 4096);
    return (err == SFUD_SUCCESS) ? LFS_ERR_OK : LFS_ERR_IO;
}

int lfs_sync(const struct lfs_config *c) {
    (void)c;
    return LFS_ERR_OK;
}

int lfs_lock(const struct lfs_config *c) {
    osStatus_t sts = osMutexAcquire(lfs_mtx, LFS_LOCK_TIMEOUT);
    if(sts != osOK) {
        return LFS_ERR_CORRUPT;
    }
    return LFS_ERR_OK;
}

int lfs_unlock(const struct lfs_config *c) {
    osMutexRelease(lfs_mtx);
    return LFS_ERR_OK;
}
