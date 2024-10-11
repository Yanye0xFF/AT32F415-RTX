/*
 * ringblk_buf.h
 *
 * @brief 基于ringblk_buf.c定制的固定块容器
 * Created on: Mar 2, 2023
 * Author: Yanye
 */

#ifndef _RINGBLK_BUF_H_
#define _RINGBLK_BUF_H_

#include "rtthread.h"
#include "stdint.h"
/*
 * Introduction:
 * The rbb is the ring buffer which is composed with many blocks. It is different from the ring buffer.
 * The ring buffer is only composed with chars. The rbb put and get supported zero copies. So the rbb
 * is very suitable for put block and get block by a certain order. Such as DMA block transmit,
 * communicate frame send/recv, and so on.
 */

#ifdef __cplusplus
extern "C" {
#endif

enum rt_rbb_status
{
    /* block in use */
    RT_RBB_BLK_USED = 0,
    /* initialized status rt_rbb_init or rt_rbb_blk_free */
    RT_RBB_BLK_FREE,

    RT_RBB_BLK_PEND,
};
typedef enum rt_rbb_status rt_rbb_status_t;

/**
 * the block of rbb
 */
struct rt_rbb_blk {
    // point to next entity
    rt_slist_t list;
    rt_rbb_status_t status :8;
    /* less then 2^24 */
    rt_size_t size :24;
    rt_uint8_t *buf;
};
typedef struct rt_rbb_blk *rt_rbb_blk_t;

/**
 * ring block buffer
 */
struct rt_rbb
{
    rt_uint8_t *buf;
    rt_size_t buf_size;
    /* all of blocks */
    rt_rbb_blk_t blk_set;
    rt_size_t blk_max_num;
    /* saved the initialized and put status blocks */
    rt_slist_t blk_list;
    /* free node list */
    rt_slist_t free_list;
    volatile uint32_t alloc_lock;
};
typedef struct rt_rbb *rt_rbb_t;

/* rbb (ring block buffer) API */
void rt_rbb_init(rt_rbb_t rbb, rt_uint8_t *buf, rt_size_t buf_size, rt_rbb_blk_t block_set, rt_size_t blk_max_num);

/* rbb block API */
rt_rbb_blk_t rt_rbb_blk_alloc(rt_rbb_t rbb);
void rt_rbb_blk_pend(rt_rbb_blk_t block);
void rt_rbb_blk_use(rt_rbb_blk_t block);

rt_rbb_blk_t rt_rbb_blk_get_pend(rt_rbb_t rbb);
rt_rbb_blk_t rt_rbb_blk_get_used(rt_rbb_t rbb);
rt_rbb_blk_t rt_rbb_blk_get_next(rt_rbb_blk_t blk);

rt_size_t rt_rbb_blk_count_used(rt_rbb_t rbb);
#define COUNT_FREE_LIST     0
#define COUNT_BLOCK_LIST    1
rt_size_t rt_rbb_blk_count_all(rt_rbb_t rbb, uint8_t type);

void rt_rbb_blk_free_top(rt_rbb_t rbb, uint32_t size);
void rt_rbb_blk_free_any(rt_rbb_t rbb, rt_rbb_blk_t prev_block, rt_rbb_blk_t cur_block);
void rt_rbb_blk_free_all(rt_rbb_t rbb);

#ifdef __cplusplus
}
#endif

#endif /* _RINGBLK_BUF_H_ */
