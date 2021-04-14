// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// ����ptʵ�ֵ�Э�̿��
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

// AsyncLoad ģ������.mid��ͨ��tzmalloc������ڴ�id
void AsyncLoad(int mid) {
    gMid = mid;
    gList = TZListCreateList(gMid);
}

// AsyncStart ����Э��
// interval�����м��.��λ:us
// interval��ASYNC_ONLY_ONE_TIMEʱ�ǵ�������,��ASYNC_NO_WAITʱ�޼������
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

// AsyncStop ֹͣЭ��
void AsyncStop(AsyncFunc func) {
    TZListNode* node = getNode(func);
    if (node == NULL) {
        return;
    }
    TZListRemove(gList, node);
}

// AsyncRun Э������
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
        // δ�������Լ����ȴ���ֱ��ִ��
        item->lastResult = item->func();
        item->timestamp = now;
        return;
    }

    if (item->interval == ASYNC_ONLY_ONE_TIME) {
        // ��������
        if (item->count == 0) {
            item->lastResult = item->func();
            item->count++;
        } else {
            TZListRemove(gList, node);
        }
        return;
    }

    // �鿴����Ƿ�
    if (now - item->timestamp >= (uint64_t)item->interval) {
        item->lastResult = item->func();
        item->timestamp = now;
    }
}
