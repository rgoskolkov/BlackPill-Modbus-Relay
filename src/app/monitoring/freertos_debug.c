#include "FreeRTOS.h"
#include "task.h"

/* This is the critical symbol that OpenOCD uses to enable FreeRTOS support */
const int __attribute__((used)) uxTopUsedPriority = 7;

/* External declarations of FreeRTOS variables that OpenOCD looks for */
extern List_t * volatile pxReadyTasksLists;
extern List_t * volatile pxDelayedTaskList1;
extern List_t * volatile pxDelayedTaskList2;
extern List_t * volatile pxOverflowDelayedTaskList;
extern List_t volatile * xPendingReadyList;