# async

## 介绍
基于protothreads开发的C语言协程库。

## 开源
- [github项目地址](https://github.com/jdhxyy/async-clang)
- [gitee项目地址](https://gitee.com/jdhxyy/async-clang)

## 特点
- 极小的系统占用，适合单片机等资源有限的场所
- 支持单次运行任务和周期运行任务
- 任务运行在协程之上，不用加锁，数据不会冲突

## 依赖
依赖的包：
```text
https://github.com/jdhxyy/tztime.git
https://github.com/jdhxyy/tzmalloc.git
https://github.com/jdhxyy/tzlist.git
https://github.com/jdhxyy/pt.git
```

使用cip可以一键安装依赖的包:[cip：C/C++包管理工具](https://blog.csdn.net/jdh99/article/details/115590099)

## API
```c
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
```

## 注意
- 在多线程环境下使用使用函数AsyncStart，AsyncStop得加互斥锁，防止资源冲突
- 调用AsyncStart函数即可启动协程，协程函数必须是pt函数
- AsyncRun函数可放在独立的线程中循环执行

## 测试用例
在windows环境下开发测试。共启动4个协程任务：
- task1：500ms打印一次
- task2：1s打印一次
- task3：单次运行
- task4：无等待运行。任务函数内控制2s打印一次

```c
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>

#include "async.h"
#include "tzmalloc.h"
#include "tztime.h"
#include "pt.h"

#define RAM_INTERNAL 0

static int gMid = -1;

static uint64_t getTime(void);

static int task1(void);
static int task2(void);
static int task3(void);
static int task4(void);

int main() {
    TZTimeLoad(getTime);
    TZMallocLoad(RAM_INTERNAL, 20, 100 * 1024, malloc(100 * 1024));
    gMid = TZMallocRegister(RAM_INTERNAL, "dcom", 4096);

    AsyncLoad(gMid);
    AsyncStart(task1, 500 * ASYNC_MILLISECOND);
    AsyncStart(task2, 1 * ASYNC_SECOND);
    AsyncStart(task3, ASYNC_ONLY_ONE_TIME);
    AsyncStart(task4, ASYNC_NO_WAIT);
    while (1) {
        AsyncRun();
    }
    return 0;
}

static uint64_t getTime(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000000 + t.tv_usec;
}

static int task1(void) {
    static struct pt pt;
    uint64_t now;
    int ms;

    PT_BEGIN(&pt);

    now = getTime();
    ms = (int)(now / 1000);
    printf("%d task1\n", ms);

    PT_END(&pt);
}

static int task2(void) {
    static struct pt pt;
    uint64_t now;
    int ms;

    PT_BEGIN(&pt);

    now = getTime();
    ms = (int)(now / 1000);
    printf("%d task2\n", ms);

    PT_END(&pt);
}

static int task3(void) {
    static struct pt pt;
    uint64_t now;
    int ms;

    PT_BEGIN(&pt);

    now = getTime();
    ms = (int)(now / 1000);
    printf("%d task3\n", ms);

    PT_END(&pt);
}

static int task4(void) {
    static struct pt pt;
    static uint64_t time;
    uint64_t now;
    int ms;

    PT_BEGIN(&pt);

    time = getTime();
    PT_WAIT_UNTIL(&pt, getTime() - time > 2 * ASYNC_SECOND);

    now = getTime();
    ms = (int)(now / 1000);
    printf("%d task4\n", ms);
    PT_END(&pt);
}
```

测试输出：
```text
17582 task3
18083 task1
18583 task2
18584 task1
19084 task1
19583 task2
19584 task4
19585 task1
20086 task1
20584 task2
20587 task1
21087 task1
21584 task2
21584 task4
21587 task1
22087 task1
22584 task2
22588 task1
23088 task1
23584 task4
23584 task2
23588 task1
24089 task1
24585 task2
24590 task1
25090 task1
25585 task2
25585 task4
25591 task1
26091 task1
26585 task2
26591 task1
27091 task1
27586 task4
27586 task2
27592 task1
```
