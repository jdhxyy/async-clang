// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// ����ptʵ�ֵ�Э�̿��
// Authors: jdh99 <jdh821@163.com>

#ifndef ASYNC_H
#define ASYNC_H

#include "pt.h"

#include <stdint.h>
#include <stdbool.h>

#define ASYNC_TAG "async"
#define ASYNC_MALLOC_SIZE 4096

// ���м������
// ��������
#define ASYNC_ONLY_ONE_TIME 0
// �޼������
#define ASYNC_NO_WAIT ((uint64_t)-1)

// ʱ�䵥λ
// ����
#define ASYNC_MILLISECOND 1000ull
// ��
#define ASYNC_SECOND 1000000ull
// ��
#define ASYNC_MINUTE 60000000ull
// Сʱ
#define ASYNC_HOUR 3600000000ull

// ASYNC_WAIT �ȴ�.interval�ǵȴ����.��λ:us
// ע��:ֻ��ͨ��AsyncStart����������ſɵ��ñ�����
#define ASYNC_WAIT(pt, interval)    \
    do {                            \
        AsyncWait(interval);        \
        PT_YIELD(pt);               \
    } while(0)

// AsyncFunc Э�̺�������.�����ǻ���ptʵ�ֵĺ���
typedef int (*AsyncFunc)(void);

// AsyncStart ����Э��
// interval�����м��.��λ:us
// interval��ASYNC_ONLY_ONE_TIMEʱ�ǵ�������,��ASYNC_NO_WAITʱ�޼������
bool AsyncStart(AsyncFunc func, uint64_t interval);

// AsyncStop ֹͣЭ��
void AsyncStop(AsyncFunc func);

// AsyncRun Э������
void AsyncRun(void);

// AsyncWait �ȴ�.interval�ǵȴ����.��λ:us
void AsyncWait(uint64_t interval);

#endif
