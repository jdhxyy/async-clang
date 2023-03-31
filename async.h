// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// 基于pt实现的协程框架
// Authors: jdh99 <jdh821@163.com>

#ifndef ASYNC_H
#define ASYNC_H

#include "pt.h"

#include <stdint.h>
#include <stdbool.h>

#define ASYNC_TAG "async"
#define ASYNC_MALLOC_SIZE 4096

// 运行间隔类型
// 单次运行
#define ASYNC_ONLY_ONE_TIME 0
// 无间隔运行
#define ASYNC_NO_WAIT ((uint64_t)-1)

// 时间单位
// 毫秒
#define ASYNC_MILLISECOND 1000ull
// 秒
#define ASYNC_SECOND 1000000ull
// 分
#define ASYNC_MINUTE 60000000ull
// 小时
#define ASYNC_HOUR 3600000000ull

// ASYNC_WAIT 等待.interval是等待间隔.单位:us
// 注意:只有通过AsyncStart启动的任务才可调用本函数
#define ASYNC_WAIT(pt, interval)    \
    do {                            \
        AsyncWait(interval);        \
        PT_YIELD(pt);               \
    } while(0)

// AsyncFunc 协程函数类型.必须是基于pt实现的函数
typedef int (*AsyncFunc)(void);

// AsyncStart 启动协程
// interval是运行间隔.单位:us
// interval是ASYNC_ONLY_ONE_TIME时是单次运行,是ASYNC_NO_WAIT时无间隔运行
bool AsyncStart(AsyncFunc func, uint64_t interval);

// AsyncStop 停止协程
void AsyncStop(AsyncFunc func);

// AsyncRun 协程运行
void AsyncRun(void);

// AsyncWait 等待.interval是等待间隔.单位:us
void AsyncWait(uint64_t interval);

#endif
