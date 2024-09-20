/*
 * lfs_port.h
 * @brief
 * Created on: Sep 20, 2024
 * Author: Yanye
 */

#ifndef COMPONENTS_LITTLEFS_LFS_PORT_H_
#define COMPONENTS_LITTLEFS_LFS_PORT_H_

#include "lfs.h"

#define LFS_LOCK_TIMEOUT 3000

#ifdef __cplusplus
extern "C" {
#endif

extern const struct lfs_config lfs_cfg;
extern lfs_t lfs_obj;

int lfs_port_init(void);

#ifdef __cplusplus
}
#endif

#endif /* COMPONENTS_LITTLEFS_LFS_PORT_H_ */
