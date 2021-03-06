/*
 * FreeRTOS Kernel V10.4.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

/*
 * Warning: if the stack is too low, a task can
 * overflow without any protection and it can crash
 * randomly.
 */
#define TASK_STACK_SIZE 1000

/*
 * The pico pi runs at 125 MHZ by default
 */
uint32_t SystemCoreClock = 125000000; //HZ
const int LED_PIN=25;

__attribute__ ((format (printf, 2, 3))) static void print_log(const char* taskName, const char* format, ...)
{
    va_list arglist;

    printf("%-*s: ", configMAX_TASK_NAME_LEN, taskName);
    va_start(arglist, format);
    vprintf(format, arglist);
    va_end(arglist);
}

#if (configSUPPORT_STATIC_ALLOCATION == 1)
static StackType_t xBlinkStack[ TASK_STACK_SIZE ];
static StaticTask_t xBlinkTaskBuffer;
#endif
void vBlinkTaskCode(void * param)
{
    const int id = (int)param;
    TaskHandle_t taskHandle = xTaskGetCurrentTaskHandle();
    const char* taskName = pcTaskGetName(taskHandle);
    print_log(taskName, "Starting blink task with id: %d\n", id);
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN,GPIO_OUT);
    while(1)
    {
        print_log(taskName, "blink on\n");
        gpio_put(LED_PIN ,1);
        vTaskDelay(pdMS_TO_TICKS(250));
        print_log(taskName, "blink off\n");
        gpio_put(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(250));
        taskYIELD();
    }
}

#if (configSUPPORT_STATIC_ALLOCATION == 1)
static StackType_t xConsoleStack[ TASK_STACK_SIZE ];
static StaticTask_t xConsoleTaskBuffer;
#endif
void vConsoleTaskCode(void * param)
{
    const int id = (int)param;
    TaskHandle_t taskHandle = xTaskGetCurrentTaskHandle();
    const char* taskName = pcTaskGetName(taskHandle);
    print_log(taskName, "Starting console task with id: %d\n", id);
    while(1)
    {
        print_log(taskName, "Hello from task %d\n", id);
        vTaskDelay(pdMS_TO_TICKS(id * 1000));
        taskYIELD();
    }
}
#if (configSUPPORT_STATIC_ALLOCATION == 1)
static StackType_t xConsoleStack2[ TASK_STACK_SIZE ];
static StaticTask_t xConsoleTaskBuffer2;
#endif

void vApplicationStackOverflowHook (  TaskHandle_t xTask, char *pcTaskName )
{
    printf("Stack overflow %p: %s. Infinite loop.\n", xTask, pcTaskName);
    while(1);
}

int main()
{
    const char* taskName = "INIT";
#if (PICO_STDIO_USB == 1)
    stdio_init_all();
    //Pause to make sure the USB has loaded and all the output comes
    sleep_ms(2000);
#endif
#if (PICO_STDIO_UART == 1)
    setup_default_uart();
#endif
    printf("********************************************\n");
    printf("    WELCOME ON FREE RTOS ON PICO PI         \n");
    printf("********************************************\n");
    print_log(taskName, "Initialising RTOS tasks\n");

#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
    print_log(taskName, "Creating dynamic task\n");
    xTaskCreate( vTaskCode, "NAME", TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
#endif

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    print_log(taskName, "Creating static tasks\n");
    TaskHandle_t xBlinkHandle = xTaskCreateStatic(
        vBlinkTaskCode,         /* Function that implements the task. */
        "BLINK",                 /* Text name for the task. */
        TASK_STACK_SIZE,        /* The number of indexes in the xBlinkStack array. */
        ( void * ) 0,           /* Parameter passed into the task. */
        tskIDLE_PRIORITY,       /* Priority at which the task is created. */
        xBlinkStack,            /* Array to use as the task's stack. */
        &xBlinkTaskBuffer );    /* Variable to hold the task's data structure. */

    TaskHandle_t xConsoleHandle = xTaskCreateStatic(
        vConsoleTaskCode,       /* Function that implements the task. */
        "CONS1",                 /* Text name for the task. */
        TASK_STACK_SIZE,        /* The number of indexes in the xConsoleStack array. */
        ( void * ) 1,           /* Parameter passed into the task. */
        tskIDLE_PRIORITY,       /* Priority at which the task is created. */
        xConsoleStack,          /* Array to use as the task's stack. */
        &xConsoleTaskBuffer );  /* Variable to hold the task's data structure. */

    TaskHandle_t xConsoleHandle2 = xTaskCreateStatic(
        vConsoleTaskCode,       /* Function that implements the task. */
        "CONS2",                 /* Text name for the task. */
        TASK_STACK_SIZE,        /* The number of indexes in the xConsoleStack array. */
        ( void * ) 2,           /* Parameter passed into the task. */
        tskIDLE_PRIORITY,       /* Priority at which the task is created. */
        xConsoleStack2,          /* Array to use as the task's stack. */
        &xConsoleTaskBuffer2 );  /* Variable to hold the task's data structure. */
    print_log(taskName, "Launching scheduler\n");
    vTaskStartScheduler();

    //We should never reach here
    print_log(taskName, "BOOOM. The scheduler has return. Inifite loop\n");
    gpio_put(LED_PIN,0);
    while(1);
#endif
}

