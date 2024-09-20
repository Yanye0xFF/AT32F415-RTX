/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016/10/1      Bernard      The first version
 */

#pragma once

#include <stdint.h>
#include "cmsis_os2.h"

namespace Rtx {

/** The Semaphore class is used to manage and protect access to a set of shared resources. */
class Semaphore
{
public:
    /** Create and Initialize a Semaphore object used for managing resources.
      @param max_count number of available resources; init count value is 0.
    */
    Semaphore(const char *name = "sem", uint32_t max_count = 3);
    ~Semaphore();

    /** Wait until a Semaphore resource becomes available.
      @param   millisec  timeout value or 0 in case of no time-out.
      @return  true on success.
    */
    bool wait(uint32_t millisec = 0xFFFFFFFFu);

    /** Release a Semaphore resource that was obtain with Semaphore::wait.
    */
    void release(void);

private:
    osSemaphoreId_t mID;
};

}
