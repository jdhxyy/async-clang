// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// 基于pt实现的协程框架
// Authors: jdh99 <jdh821@163.com>

#ifndef ASYNC_H
#define ASYNC_H

#include <stdint.h>
#include <stdbool.h>

// 运行间隔类型
// 单次运行
#define ASYNC_ONLY_ONE_TIME 0
// 无间隔运行
#define ASYNC_NO_WAIT -1

// 时间单位
// 毫秒
#define ASYNC_MILLISECOND 1000
// 秒
#define ASYNC_SECOND 1000000
// 分
#define ASYNC_MINUTE 60000000
// 小时
#define ASYNC_HOUR 3600000000


// AsyncFunc 协程函数类型.必须是基于pt实现的函数
typedef int (*AsyncFunc)(void);

// AsyncLoad 模块载入.mid是通过tzmalloc申请的内存id
void AsyncLoad(int mid);

// AsyncStart 启动协程
// interval是运行间隔.单位:us
// interval是ASYNC_ONLY_ONE_TIME时是单次运行,是ASYNC_NO_WAIT时无间隔运行
bool AsyncStart(AsyncFunc func, int interval);

// AsyncStop 停止协程
void AsyncStop(AsyncFunc func);

// AsyncRun 协程运行
void AsyncRun(void);

#endif
