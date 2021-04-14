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