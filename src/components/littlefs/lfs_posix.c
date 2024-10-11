/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-05-27     Yi.qiu       The first version
 * 2018-02-07     Bernard      Change the 3rd parameter of open/fcntl/ioctl to '...'
 * 2021-08-26     linzhenxing  add setcwd and modify getcwd\chdir
 */

#include "lfs_posix.h"

#include <sys/types.h>
#include <stddef.h>

#include "cmsis_os2.h"
#include "rtthread.h"
#include "lfs.h"
#include "sfud.h"

// timeout in ms
#define LFS_LOCK_TIMEOUT       3000u
// little-fs partition start address, size in KB, must align with 4KB
#define LFS_PARTITION_START    312u

static int lfs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
static int lfs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
static int lfs_erase(const struct lfs_config *c, lfs_block_t block);
static int lfs_sync(const struct lfs_config *c);
static int lfs_lock(const struct lfs_config *c);
static int lfs_unlock(const struct lfs_config *c);

static struct lfs_config lfs_cfg = {
    .context = NULL,
    .read = lfs_read,
    .prog = lfs_prog,
    .erase = lfs_erase,
    .sync = lfs_sync,
    .lock = lfs_lock,
    .unlock = lfs_unlock,
    .read_size = 1,
    .prog_size = 1,
    .block_size = 4096,
    // adjust block_count in lfs_posix_init()
    .block_count = 0,
    .block_cycles = 1000,
    .cache_size = 1024,
    .lookahead_size = 32,
    .compact_thresh = 3604,
    .read_buffer = NULL,
    .prog_buffer = NULL,
    .lookahead_buffer = NULL,
    .name_max = LFS_NAME_MAX,
    .file_max = LFS_FILE_MAX,
    .attr_max = LFS_ATTR_MAX,
    .metadata_max = 4096,
    .inline_max = 512,
};

static lfs_t lfs_obj;
static osMutexId_t lfs_mtx = NULL;
static uint32_t init_error = 0;

int lfs_posix_init(void) {
    uint32_t devId;
    uint32_t size = 0;
    sfud_err err;
    int result;

    const osMutexAttr_t attr = {
        .name = "lfs",
        .attr_bits = (osMutexRecursive|osMutexPrioInherit|osMutexRobust),
        .cb_mem = NULL,
        .cb_size = 0u
    };

    lfs_mtx = osMutexNew(&attr);
    if(lfs_mtx == NULL) {
        init_error = 1;
        return 0;
    }

    err = sfud_get_id(&devId);
    devId = (devId & 0xFFu);
    // 512K, 1M, 2M, 4M, 8M, 16M
    if((err == SFUD_SUCCESS) && (devId >= 0x13u) && (devId <= 0x18u)) {
        size = (0x1u << (devId - 0x13u)) << 9u; // size in KB
        // SECONDARY_BOOT1_SIZE + SECONDARY_BOOT2_SIZE + NVS_TLV_SIZE + OTA_TEMP_SEC_SIZE
        size -= LFS_PARTITION_START;
    }
    if(size == 0) {
        init_error = 1;
        return 0;
    }
    // 200K for 512, 712K for 1024
    lfs_cfg.block_count = (size >> 2u);

    // mount file system
    result = lfs_mount(&lfs_obj, &lfs_cfg);
    if(LFS_ERR_OK != result) {
        result = lfs_format(&lfs_obj, &lfs_cfg);
        if(LFS_ERR_OK != result) {
            init_error = 1;
            return 0;
        }
        result = lfs_mount(&lfs_obj, &lfs_cfg);
        if(LFS_ERR_OK != result) {
            init_error = 1;
        }
    }
    return 0;
}
INIT_COMPONENT_EXPORT(lfs_posix_init);

static int lfs_read(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size) {
    uint32_t addr = (LFS_PARTITION_START * 1024u) + (block * SFUD_FLASH_PAGE_SIZE) + off;
    sfud_err err = sfud_read(addr, size, (uint8_t *)buffer);
    return (err == SFUD_SUCCESS) ? LFS_ERR_OK : LFS_ERR_IO;
}

static int lfs_prog(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size) {
    uint32_t addr = (LFS_PARTITION_START * 1024u) + (block * SFUD_FLASH_PAGE_SIZE) + off;
    sfud_err err = sfud_write(addr, size, (const uint8_t *)buffer);
    return (err == SFUD_SUCCESS) ? LFS_ERR_OK : LFS_ERR_IO;
}

static int lfs_erase(const struct lfs_config *c, lfs_block_t block) {
    uint32_t addr = (LFS_PARTITION_START * 1024u) + (block * SFUD_FLASH_PAGE_SIZE);
    sfud_err err = sfud_erase(addr, SFUD_FLASH_PAGE_SIZE);
    return (err == SFUD_SUCCESS) ? LFS_ERR_OK : LFS_ERR_IO;
}

static int lfs_sync(const struct lfs_config *c) {
    (void)c;
    return LFS_ERR_OK;
}

static int lfs_lock(const struct lfs_config *c) {
    osStatus_t sts = osMutexAcquire(lfs_mtx, LFS_LOCK_TIMEOUT);
    if(sts != osOK) {
        return LFS_ERR_CORRUPT;
    }
    return LFS_ERR_OK;
}

static int lfs_unlock(const struct lfs_config *c) {
    osMutexRelease(lfs_mtx);
    return LFS_ERR_OK;
}

/**
 * @addtogroup FsPosixApi
 * @{
 */

/**
 * this function is a POSIX compliant version, which will open a file and
 * return a file descriptor according specified flags.
 *
 * @param file the path name of file.
 * @param flags the file open flags.
 *
 * @return the non-negative integer on successful open, others for failed.
 */
int open(const char *file, int flags, ...) {
    int result;
    int lfs_flag = 0;
    // mode_t mode = 0; // same as uint32_t

    if (file == NULL || init_error) {
        return -1;
    }

    // ignore file permission
    /*
    if ((flags & O_CREAT) || (flags & O_TMPFILE) == O_TMPFILE)
    {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, mode_t);
        va_end(ap);
    }
    */

    // major class
    if(flags == O_RDONLY) {
        lfs_flag = LFS_O_RDONLY;
    }else if ((flags & O_WRONLY) == O_WRONLY) {
        lfs_flag = LFS_O_WRONLY;
    }else if((flags & O_RDWR) == O_RDWR) {
        lfs_flag = LFS_O_RDWR;
    }
    // minor class
    if((flags & O_APPEND) == O_APPEND) {
        lfs_flag |= LFS_O_APPEND;
    }
    if((flags & O_CREAT) == O_CREAT) {
        lfs_flag |= LFS_O_CREAT;
    }
    if((flags & O_TRUNC) == O_TRUNC) {
        lfs_flag |= LFS_O_TRUNC;
    }
    if((flags & O_EXCL) == O_EXCL) {
        lfs_flag |= LFS_O_EXCL;
    }

    if(lfs_flag == 0) {
        return -1;
    }

    lfs_file_t *fp = (lfs_file_t *)rt_malloc(sizeof(lfs_file_t));
    if(fp == NULL) {
        return -1;
    }

    result = lfs_file_open(&lfs_obj, fp, file, lfs_flag);
    if(LFS_ERR_OK == result) {
        return (int)fp;
    }

    rt_free(fp);
    return -1;
}

/**
 * this function is a POSIX compliant version,
 * which will create a new file or rewrite an existing one
 *
 * @param path the path name of file.
 * @param mode the file permission bits to be used in creating the file (not used, can be 0)
 *
 * @return the non-negative integer on successful open, others for failed.
 */
int creat(const char *path, mode_t mode)
{
    return open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

/**
 * this function is a POSIX compliant version, which will close the open
 * file descriptor.
 *
 * @param fd the file descriptor.
 *
 * @return 0 on successful, -1 on failed.
 */
int close(int fd) {
    if(fd <= STDERR_FILENO) {
        return -1;
    }

    lfs_file_t *fp = (lfs_file_t *)fd;
    int result = lfs_file_close(&lfs_obj, fp);
    rt_free(fp);

    return (result == LFS_ERR_OK) ? 0 : -1;
}

/**
 * this function is a POSIX compliant version, which will read specified data
 * buffer length for an open file descriptor.
 *
 * @param fd the file descriptor.
 * @param buf the buffer to save the read data.
 * @param len the maximal length of data buffer
 *
 * @return the actual read data buffer length. If the returned value is 0, it
 * may be reach the end of file, please check errno.
 */
ssize_t read(int fd, void *buf, size_t len) {
    if ((buf == NULL) || (fd <= STDERR_FILENO)) {
        return -1;
    }

    lfs_file_t *fp = (lfs_file_t *)fd;
    lfs_ssize_t result = lfs_file_read(&lfs_obj, fp, buf, (lfs_size_t)len);
    if(result < 0) {
        return -1;
    }

    return result;
}

/**
 * this function is a POSIX compliant version, which will write specified data
 * buffer length for an open file descriptor.
 *
 * @param fd the file descriptor
 * @param buf the data buffer to be written.
 * @param len the data buffer length.
 *
 * @return the actual written data buffer length.
 */
ssize_t write(int fd, const void *buf, size_t len) {
    if ((buf == NULL) || (fd <= STDERR_FILENO)) {
        return -1;
    }

    lfs_file_t *fp = (lfs_file_t *)fd;
    lfs_ssize_t result = lfs_file_write(&lfs_obj, fp, buf, (lfs_size_t)len);
    if(result < 0) {
        return -1;
    }

    return result;
}

/**
 * this function is a POSIX compliant version, which will seek the offset for
 * an open file descriptor.
 *
 * @param fd the file descriptor.
 * @param offset the offset to be seeked.
 * @param whence the directory of seek.
 *
 * @return the current read/write position in the file, or -1 on failed.
 */
off_t lseek(int fd, off_t offset, int whence) {
    if (fd <= STDERR_FILENO) {
        return -1;
    }

    lfs_file_t *fp = (lfs_file_t *)fd;
    lfs_soff_t result = lfs_file_seek(&lfs_obj, fp, (lfs_soff_t)offset, whence);
    if(result < 0) {
        return -1;
    }

    return result;
}

/**
 * this function is a POSIX compliant version, which will rename old file name
 * to new file name.
 *
 * @param old_file the old file name.
 * @param new_file the new file name.
 *
 * @return 0 on successful, -1 on failed.
 *
 * note: the old and new file name must be belong to a same file system.
 */
int rename(const char *old_file, const char *new_file) {
    if ((old_file == NULL) || (new_file == NULL) || init_error) {
        return -1;
    }

    int result = lfs_rename(&lfs_obj, old_file, new_file);
    if (result < 0) {
        return -1;
    }

    return 0;
}

/**
 * this function is a POSIX compliant version, which will unlink (remove) a
 * specified path file from file system.
 *
 * @param pathname the specified path name to be unlinked.
 *
 * @return 0 on successful, -1 on failed.
 */
int unlink(const char *pathname) {
    if ((pathname == NULL) || init_error) {
        return -1;
    }
    int result = lfs_remove(&lfs_obj, pathname);
    return (result < 0) ? -1 : 0;
}

#ifndef _WIN32 /* we can not implement these functions */
/**
 * this function is a POSIX compliant version, which will get file information.
 *
 * @param file the file name
 * @param buf the data buffer to save stat description.
 *
 * @return 0 on successful, -1 on failed.
 */
int stat(const char *file, struct stat *buf) {
    int result;
    struct lfs_info info;

    if ((file == NULL) || (buf == NULL) || init_error) {
        return -1;
    }

    result = lfs_stat(&lfs_obj, file, &info);
    if(result == LFS_ERR_OK) {
        buf->st_uid = info.type;
        buf->st_size = info.size;
        return 0;
    }

    return -1;
}

/**
 * this function is a POSIX compliant version, which will get file status.
 *
 * @param fildes the file description
 * @param buf the data buffer to save stat description.
 *
 * @return 0 on successful, -1 on failed.
 */
int fstat(int fildes, struct stat *buf) {
    if ((buf == NULL) || (fildes <= STDERR_FILENO)) {
        return -1;
    }

    lfs_file_t *fp = (lfs_file_t *)fildes;
    int32_t size = lfs_file_size(&lfs_obj, fp);
    if(size >= 0) {
        buf->st_size = size;
        return 0;
    }

    return -1;
}
#endif

/**
 * this function is a POSIX compliant version, which shall request that all data
 * for the open file descriptor named by fildes is to be transferred to the storage
 * device associated with the file described by fildes.
 *
 * @param fildes the file description
 *
 * @return 0 on successful completion. Otherwise, -1 shall be returned and errno
 * set to indicate the error.
 */
int fsync(int fildes) {
    if (fildes <= STDERR_FILENO) {
        return -1;
    }
    lfs_file_t *fp = (lfs_file_t *)fildes;
    int ret = lfs_file_sync(&lfs_obj, fp);

    return (ret < 0) ? -1 : 0;
}

/**
 *
 * this function is a POSIX compliant version, which cause the regular file
 * referenced by fd to be truncated to a size of precisely length bytes.
 * @param fd the file descriptor.
 * @param length the length to be truncated.
 *
 * @return Upon successful completion, ftruncate() shall return 0;
 * otherwise, -1 shall be returned and errno set to indicate the error.
 */
int ftruncate(int fd, off_t length) {
    if (fd <= STDERR_FILENO) {
        return -1;
    }
    lfs_file_t *fp = (lfs_file_t *)fd;
    int result = lfs_file_truncate(&lfs_obj, fp, (lfs_off_t)length);

    return (result < 0) ? -1 : 0;
}

/**
 * this function is a POSIX compliant version, which will return the
 * information about a mounted file system.
 *
 * @param path the path which mounted file system, can pass NULL
 * @param buf the buffer to save the returned information.
 *
 * @return 0 on successful, others on failed.
 */

int statfs(const char *path, struct statfs *buf) {
    int result;
    struct lfs_fsinfo info;

    (void)path;

    if((buf == NULL) || init_error) {
        return -1;
    }

    result = lfs_fs_stat(&lfs_obj, &info);
    int32_t size = lfs_fs_size(&lfs_obj);

    if(result == LFS_ERR_OK) {
        buf->f_type = info.disk_version;
        buf->f_bsize = info.block_size;
        buf->f_blocks = info.block_count;
        if(size > 0) {
            buf->f_bfree = (size / info.block_size);
        }else {
            buf->f_bfree = 0;
        }
        buf->f_fsid = info.attr_max;
        buf->f_namelen = info.name_max;
    }

    return result;
}

/**
 * this function is a POSIX compliant version, which will make a directory
 *
 * @param path the directory path to be made.
 * @param mode
 *
 * @return 0 on successful, others on failed.
 */
int mkdir(const char *path, mode_t mode) {
    (void)mode;
    if ((path == NULL) || init_error) {
        return -1;
    }

    int result = lfs_mkdir(&lfs_obj, path);
    return result;
}

/**@}*/
