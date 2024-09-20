/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include "cxx_mutex.h"
#include <stddef.h>

using namespace Rtx;

Mutex::Mutex(const char *name)
{
    const osMutexAttr_t attr = {
        .name = name,
        .attr_bits = (osMutexRecursive|osMutexPrioInherit|osMutexRobust),
        .cb_mem = NULL,
        .cb_size = 0
    };
    this->mID = osMutexNew (&attr);
}

bool Mutex::lock(uint32_t millisec)
{
    osStatus_t sts = osMutexAcquire(mID, millisec);
    return (osOK == sts);
}

bool Mutex::trylock()
{
    return lock(0);
}

void Mutex::unlock()
{
    osMutexRelease(mID);
}

Mutex::~Mutex()
{
    osMutexDelete(mID);
}
