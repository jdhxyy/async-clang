// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// 基于pt实现的协程框架
// Authors: jdh99 <jdh821@163.com>

#include "async.h"

#include "pt.h"
#include "tzmalloc.h"
#include "tzlist.h"
#include "tztime.h"
#include "lagan.h"

#include <string.h>

#pragma pack(1)

typedef struct {
    AsyncFunc func;
    uint64_t interval;
    int lastResult;
    int count;
    uint64_t timestamp;

    // 等待截至时间
    uint64_t waitEndTime;
} tItem;

#pragma pack()

static tItem* itemNow = NULL;

static int getMid(void);
static intptr_t getList(void);

static TZListNode* getNode(AsyncFunc func);
static TZListNode* createNode(void);
static void checkNode(TZListNode* node, uint64_t now);

static int getMid(void) {
    static int mid = -1;
    if (mid == -1) {
        mid = TZMallocRegister(0, ASYNC_TAG, ASYNC_MALLOC_SIZE);
        if (mid == -1) {
            LE(ASYNC_TAG, "malloc register failed!");
        }
    }
    return mid;
}

static intptr_t getList(void) {
    static intptr_t list = 0;
    if (list == 0) {
        list = TZListCreateList(getMid());
        if (list == 0) {
            LE(ASYNC_TAG, "create list failed!");
        }
    }
    return list;
}

// AsyncStart 启动协程
// interval是运行间隔.单位:us
// interval是ASYNC_ONLY_ONE_TIME时是单次运行,是ASYNC_NO_WAIT时无间隔运行
bool AsyncStart(AsyncFunc func, uint64_t interval) {
    TZListNode* node = getNode(func);
    if (node == NULL) {
        node = createNode();
        if (node == NULL) {
            LE(ASYNC_TAG, "async start failed!create node failed!");
            return false;
        }
        TZListAppend(getList(), node);
    }

    tItem* item = (tItem*)node->Data;
    item->func = func;
    item->interval = interval;
    item->timestamp = TZTimeGet();
    item->lastResult = PT_ENDED;
    item->count = 0;  
    return true;
}

static TZListNode* getNode(AsyncFunc func) {
    TZListNode* node = TZListGetHeader(getList());
    tItem* item = NULL;
    for (;;) {
        if (node == NULL) {
            break;
        }
        item = (tItem*)node->Data;
        if (item->func == func) {
            return node;
        }
        node = node->Next;
    }
    return NULL;
}

static TZListNode* createNode(void) {
    TZListNode* node = TZListCreateNode(getList());
    if (node == NULL) {
        return NULL;
    }
    node->Data = TZMalloc(getMid(), sizeof(tItem));
    if (node->Data == NULL) {
        TZFree(node);
        return NULL;
    }
    return node;
}

// AsyncStop 停止协程
void AsyncStop(AsyncFunc func) {
    TZListNode* node = getNode(func);
    if (node == NULL) {
        return;
    }
    TZListRemove(getList(), node);
}

// AsyncRun 协程运行
void AsyncRun(void) {
    uint64_t now = TZTimeGet();
    TZListNode* node = TZListGetHeader(getList());
    TZListNode* nodeNext = NULL;
    for (;;) {
        if (node == NULL) {
            break;
        }
        nodeNext = node->Next;
        checkNode(node, now);
        node = nodeNext;
    }
}

static void checkNode(TZListNode* node, uint64_t now) {
    tItem* item = (tItem*)node->Data;
    itemNow = item;
    if (item->lastResult != PT_ENDED || item->interval == ASYNC_NO_WAIT) {
        if (item->waitEndTime != 0 && now < item->waitEndTime) {
            // 等待中返回
            return;
        }
        // 未结束的以及不等待的直接执行
        item->waitEndTime = 0;
        item->lastResult = item->func();
        item->timestamp = now;
        return;
    }

    if (item->interval == ASYNC_ONLY_ONE_TIME) {
        // 单次运行
        if (item->count == 0) {
            item->lastResult = item->func();
            item->count++;
        } else {
            TZListRemove(getList(), node);
        }
        return;
    }

    // 查看间隔是否到
    if (now - item->timestamp >= (uint64_t)item->interval) {
        item->lastResult = item->func();
        item->timestamp = now;
    }
}

// AsyncWait 等待.interval是等待间隔.单位:us
void AsyncWait(uint64_t interval) {
    itemNow->waitEndTime = TZTimeGet() + interval;
}
