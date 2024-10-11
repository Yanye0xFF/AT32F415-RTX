/*
 * lfs_posix.h
 * @brief
 * Created on: Oct 10, 2024
 * Author: Yanye
 */

#ifndef COMPONENTS_LITTLEFS_LFS_POSIX_H_
#define COMPONENTS_LITTLEFS_LFS_POSIX_H_

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

struct statfs {
    long f_type;     //文件系统的类型
    long f_bsize;   //经优化后的传输块的大小
    long f_blocks;  //文件系统数据块总数
    long f_bfree;    //可用块数
    long f_bavail;   //普通用户能够获得的块数
    long f_files;      //文件结点总数
    long f_ffree;     //可用文件结点数
    long f_fsid;     //文件系统标识
    long f_namelen;  //文件名的最大长度
};

#endif /* COMPONENTS_LITTLEFS_LFS_POSIX_H_ */
