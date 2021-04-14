// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// ����ptʵ�ֵ�Э�̿��
// Authors: jdh99 <jdh821@163.com>

#ifndef ASYNC_H
#define ASYNC_H

#include <stdint.h>
#include <stdbool.h>

// ���м������
// ��������
#define ASYNC_ONLY_ONE_TIME 0
// �޼������
#define ASYNC_NO_WAIT -1

// ʱ�䵥λ
// ����
#define ASYNC_MILLISECOND 1000
// ��
#define ASYNC_SECOND 1000000
// ��
#define ASYNC_MINUTE 60000000
// Сʱ
#define ASYNC_HOUR 3600000000


// AsyncFunc Э�̺�������.�����ǻ���ptʵ�ֵĺ���
typedef int (*AsyncFunc)(void);

// AsyncLoad ģ������.mid��ͨ��tzmalloc������ڴ�id
void AsyncLoad(int mid);

// AsyncStart ����Э��
// interval�����м��.��λ:us
// interval��ASYNC_ONLY_ONE_TIMEʱ�ǵ�������,��ASYNC_NO_WAITʱ�޼������
bool AsyncStart(AsyncFunc func, int interval);

// AsyncStop ֹͣЭ��
void AsyncStop(AsyncFunc func);

// AsyncRun Э������
void AsyncRun(void);

#endif
