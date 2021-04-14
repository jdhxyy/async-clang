// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// 基于pt实现的协程框架
// Authors: jdh99 <jdh821@163.com>

#include "async.h"
#include "pt.h"
#include "tzmalloc.h"
#include "tzlist.h"
#include "tztime.h"

#include <string.h>

#pragma pack(1)

typedef struct {
    AsyncFunc func;
    int interval;
    int lastResult;
    int count;
    uint64_t timestamp;
} tItem;

#pragma pack()

static int gMid = -1;
static intptr_t gList = 0;

static TZListNode* getNode(AsyncFunc func);
static TZListNode* createNode(void);
static void checkNode(TZListNode* node, uint64_t now);

// AsyncLoad 模块载入.mid是通过tzmalloc申请的内存id
void AsyncLoad(int mid) {
    gMid = mid;
    gList = TZListCreateList(gMid);
}

// AsyncStart 启动协程
// interval是运行间隔.单位:us
// interval是ASYNC_ONLY_ONE_TIME时是单次运行,是ASYNC_NO_WAIT时无间隔运行
bool AsyncStart(AsyncFunc func, int interval) {
    TZListNode* node = getNode(func);
    if (node == NULL) {
        node = createNode();
        if (node == NULL) {
            return false;
        }
        TZListAppend(gList, node);
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
    TZListNode* node = TZListGetHeader(gList);
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
    TZListNode* node = TZListCreateNode(gList);
    if (node == NULL) {
        return NULL;
    }
    node->Data = TZMalloc(gMid, sizeof(tItem));
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
    TZListRemove(gList, node);
}

// AsyncRun 协程运行
void AsyncRun(void) {
    uint64_t now = TZTimeGet();
    TZListNode* node = TZListGetHeader(gList);
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
    if (item->lastResult != PT_ENDED || item->interval == ASYNC_NO_WAIT) {
        // 未结束的以及不等待的直接执行
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
            TZListRemove(gList, node);
        }
        return;
    }

    // 查看间隔是否到
    if (now - item->timestamp >= (uint64_t)item->interval) {
        item->lastResult = item->func();
        item->timestamp = now;
    }
}
