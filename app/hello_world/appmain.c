/*
 * Copyright (c) 2021 Sung Ho Park
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ubinos.h>

#include <stdio.h>
#include <stdlib.h>

static void task1_func(void * arg);
static void task2_func(void * arg);

int appmain(int argc, char * argv[])
{
    int r;
    (void) r;

    r = task_create(NULL, task1_func, NULL, task_getmiddlepriority(), 0, "task1");
    ubi_assert(r == 0);

    ubik_comp_start();

    return 0;
}

static void task1_func(void * arg)
{
    int r;
    (void) r;
    unsigned int delayms;

    printf("\n\n\n");
    printf("================================================================================\n");
    printf("hello_world (build time: %s %s)\n", __TIME__, __DATE__);
    printf("================================================================================\n");
    printf("\n");

    r = task_create(NULL, task2_func, NULL, task_getmiddlepriority(), 0, "task2");
    ubi_assert(r == 0);

    printf("\n");
    for (unsigned int i = 0; ; i++) {
        delayms = (rand() % 10 + 1) * 200;
        printf("1: hello (%u) (delay = %4d ms)\n", i, delayms);
        task_sleepms(delayms);
    }
}

static void task2_func(void * arg)
{
    int r;
    (void) r;
    unsigned int delayms;

    printf("\n");
    for (unsigned int i = 0; ; i++) {
        delayms = (rand() % 10 + 1) * 200;
        printf("2: hello (%u) (delay = %4d ms)\n", i, delayms);
        task_sleepms(delayms);
    }
}

