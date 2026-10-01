/**
 * @file FreeRTOSConfig.h
 * @brief FreeRTOS Kernel Configuration for STM32F4xx Automotive Cluster Node.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configUSE_PREEMPTION                    1
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCPU_CLOCK_HZ                      (SystemCoreClock)
#define configTICK_RATE_HZ                      ((TickType_t)1000)
#define configMAX_PRIORITIES                    (7)
#define configMINIMAL_STACK_SIZE                ((uint16_t)128)
#define configTOTAL_HEAP_SIZE                   ((size_t)(32 * 1024))
#define configMAX_TASK_NAME_LEN                 (16)
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configQUEUE_REGISTRY_SIZE               8

/* Task Priorities */
#define TASK_PRIO_CAN_RX                        (configMAX_PRIORITIES - 1) /* Highest */
#define TASK_PRIO_GUI_RENDER                    (tskIDLE_PRIORITY + 3)
#define TASK_PRIO_ODO_STORAGE                   (tskIDLE_PRIORITY + 2)
#define TASK_PRIO_DIAGNOSTICS                   (tskIDLE_PRIORITY + 1)

#endif /* FREERTOS_CONFIG_H */
