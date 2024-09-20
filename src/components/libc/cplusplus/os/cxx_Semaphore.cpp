/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include "cxx_semaphore.h"
#include <stddef.h>

using namespace Rtx;

Semaphore::Semaphore(const char *name, uint32_t max_count)
{
    const osSemaphoreAttr_t attr = {
        .name = name,
        .attr_bits = 0,
        .cb_mem = NULL,
        .cb_size = 0
    };
    this->mID = osSemaphoreNew(max_count, 0, &attr);
}

bool Semaphore::wait(uint32_t millisec)
{
    osStatus_t sts = osSemaphoreAcquire (mID, millisec);
    return (osOK == sts);
}

void Semaphore::release(void)
{
    osSemaphoreRelease(mID);
}

Semaphore::~Semaphore()
{
    osSemaphoreDelete(mID);
}
