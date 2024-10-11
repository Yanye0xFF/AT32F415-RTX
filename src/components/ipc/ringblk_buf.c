/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-25     armink       the first version
 */

#include "ringblk_buf.h"
#include "cmsis_gcc.h"
#include <stdbool.h>

/**
 * ring block buffer object initialization
 *
 * @param rbb ring block buffer object
 * @param buf buffer
 * @param buf_size buffer size
 * @param block_set block set
 * @param blk_max_num max block number
 *
 * @note When your application need align access, please make the buffer address is aligned.
 */
void  rt_rbb_init(rt_rbb_t rbb, rt_uint8_t *buf, rt_size_t buf_size,
        rt_rbb_blk_t block_set, rt_size_t blk_max_num) {
    rt_size_t i;
    rt_size_t split;
    rt_slist_t *iterator;

    rbb->buf = buf;
    rbb->buf_size = buf_size;
    rbb->blk_set = block_set;
    rbb->blk_max_num = blk_max_num;
    split = buf_size / blk_max_num;

    block_set[0].status = RT_RBB_BLK_FREE;
    block_set[0].size = 0;
    block_set[0].buf = buf;
    rbb->free_list.next = &block_set[0].list;

    iterator = &block_set[0].list;

    /* initialize block status */
    for (i = 1; i < blk_max_num; i++) {
        block_set[i].status = RT_RBB_BLK_FREE;
        block_set[i].size = 0;
        block_set[i].buf = (buf + (i * split));

        iterator->next = &block_set[i].list;
        iterator = &block_set[i].list;
    }
    iterator->next = RT_NULL;
    rbb->blk_list.next = RT_NULL;
    rbb->alloc_lock = 0;
}

/**
 * Allocate a block by given size. The block will add to blk_list when allocate success.
 *
 * @param rbb ring block buffer object
 * @param blk_size block size
 *
 * @note When your application need align access, please make the blk_szie is aligned.
 *
 * @return != RT_NULL: allocated block
 *            RT_NULL: allocate failed
 */
rt_rbb_blk_t  rt_rbb_blk_alloc(rt_rbb_t rbb)
{
    rt_slist_t *node;
    rt_slist_t *tail;
    rt_rbb_blk_t block;
    int retry = 3;

    while(rbb->alloc_lock && retry) {
        // alloc_lock只会锁2个周期
        __NOP(); __NOP(); __NOP();
        retry--;
    }

    if(retry == 0) {
        return RT_NULL;
    }
    
    node = rt_slist_first(&rbb->free_list);
    if(node == RT_NULL) {
        return RT_NULL;
    }
    rbb->free_list.next = node->next;
    node->next = RT_NULL;

    tail = rt_slist_tail(&rbb->blk_list);
    tail->next = node;

    block = (rt_rbb_blk_t)node;

    return block;
}

/**
 * put a block to ring block buffer object
 *
 * @param block the block
 */
void  rt_rbb_blk_pend(rt_rbb_blk_t block)
{
    block->status = RT_RBB_BLK_PEND;
}

void  rt_rbb_blk_use(rt_rbb_blk_t block)
{
    block->status = RT_RBB_BLK_USED;
}

/**
 * get top block from the ring block buffer object
 *
 * @param rbb ring block buffer object
 *
 * @return != RT_NULL: block
 *            RT_NULL: get failed
 */
rt_rbb_blk_t  rt_rbb_blk_get_pend(rt_rbb_t rbb) {
    rt_slist_t *node;
    rt_rbb_blk_t block = RT_NULL;

    rt_slist_for_each(node, &rbb->blk_list) {
        block = (rt_rbb_blk_t)node;
        if(block->status == RT_RBB_BLK_PEND) {
            break;
        }
        block = RT_NULL;
    }

    return block;
}

rt_rbb_blk_t  rt_rbb_blk_get_used(rt_rbb_t rbb) {
    rt_slist_t *head;
    rt_rbb_blk_t block = RT_NULL;

    head = rbb->blk_list.next;

    if(head != RT_NULL) {
        block = (rt_rbb_blk_t)head;
        if(block->status != RT_RBB_BLK_USED) {
            block = RT_NULL;
        }
    }

    return block;
}

rt_rbb_blk_t rt_rbb_blk_get_next(rt_rbb_blk_t blk) {
    if(!blk) {
        return RT_NULL;
    }
    rt_slist_t *next = blk->list.next;

    return (rt_rbb_blk_t)next;
}

/**
 * @brief count entity from 'rbb' which status equals 'RT_RBB_BLK_USED'.
 * */
rt_size_t  rt_rbb_blk_count_used(rt_rbb_t rbb) {
    rt_slist_t *node;
    rt_rbb_blk_t blk;
    rt_size_t count = 0;

    rt_slist_for_each(node, &rbb->blk_list) {
        blk = (rt_rbb_blk_t)node;
        if(blk->status == RT_RBB_BLK_USED) {
            count++;
        }
    }

    return count;
}

/**
 * @brief count all entity from 'rbb',
 *        status include 'COUNT_FREE_LIST' and 'COUNT_BLOCK_LIST'.
 * */
rt_size_t  rt_rbb_blk_count_all(rt_rbb_t rbb, uint8_t type) {
    rt_slist_t *node;
    rt_size_t count = 0;

    if(COUNT_FREE_LIST == type) {
        node = rbb->free_list.next;
    }else {
        node = rbb->blk_list.next;
    }

    for(; node != RT_NULL; node = node->next) {
        count++;
    }

    return count;
}

/**
 * @brief 从blk_list顶部向下依次释放size个元素
 * @param rbb 数据容器
 * @param size 被释放的元素个数，函数内已加入范围检查
 * */
void  rt_rbb_blk_free_top(rt_rbb_t rbb, uint32_t size)
{
    rt_slist_t *head;
    rt_slist_t *node;
    rt_rbb_blk_t blk;
    uint32_t i;
    
    head = rbb->blk_list.next;
    if((head == RT_NULL) || (size == 0)) {
    	return;
    }
        
    node = &(rbb->blk_list);

    for(i = 0; i < size; i++) {
        node = rt_slist_next(node);
        blk = (rt_rbb_blk_t)node;
        blk->status = RT_RBB_BLK_FREE;
        // 防止size大于blk_list实际容量导致NPE
        if(node->next == RT_NULL) {
            break;
        }
    }

    /* remove it on rbb block list */
    rbb->blk_list.next = node->next;
    
    rbb->alloc_lock = 1;
    node->next = rbb->free_list.next;
    rbb->free_list.next = head;
    rbb->alloc_lock = 0;
}

/**
 * @brief 释放blk_list链表中任意一个元素
 * @param cbb 输入捕获数据容器
 * @param prev_block 前一个元素，当被释放的元素是blk_list第一个元素时，填NULL
 * @param cur_block 被释放的元素，将被加入free_list
 * */
void  rt_rbb_blk_free_any(rt_rbb_t rbb, rt_rbb_blk_t prev_block, rt_rbb_blk_t cur_block) {
    rt_slist_t *node;
    rt_slist_t *prev = &(rbb->blk_list);
    bool find = false;

    if(cur_block == RT_NULL) {
		return;
	}
    if(prev_block == RT_NULL) {
        rt_slist_for_each(node, &rbb->blk_list) {
            if(node == (rt_slist_t *)cur_block) {
                find = true;
                break;
            }
            prev = node;
        }
        if(!find) {
            return;
        }
        prev->next = cur_block->list.next;
    }else {
        // TODO 二次检查prev_block和cur_block是否在blk_list链表中
        prev_block->list.next = cur_block->list.next;
    }

    cur_block->status = RT_RBB_BLK_FREE;
    
    rbb->alloc_lock = 1;
    cur_block->list.next = rbb->free_list.next;
    rbb->free_list.next = &(cur_block->list);
    rbb->alloc_lock = 0;
}

void  rt_rbb_blk_free_all(rt_rbb_t rbb) {
    rt_size_t i;
    rt_slist_t *iterator;

    // disable function "rt_cbb_blk_alloc"
    rbb->free_list.next = RT_NULL;
    rbb->blk_list.next = RT_NULL;

    rbb->blk_set[0].status = RT_RBB_BLK_FREE;
    rbb->blk_set[0].size = 0;
    iterator = &rbb->blk_set[0].list;

    /* initialize block status */
    for (i = 1; i < rbb->blk_max_num; i++) {
    	rbb->blk_set[i].status = RT_RBB_BLK_FREE;
        rbb->blk_set[i].size = 0;

        iterator->next = &rbb->blk_set[i].list;
        iterator = &rbb->blk_set[i].list;
    }
    iterator->next = RT_NULL;
    // enable function "rt_cbb_blk_alloc"
    rbb->free_list.next = &rbb->blk_set[0].list;
    rbb->alloc_lock = 0;
}
