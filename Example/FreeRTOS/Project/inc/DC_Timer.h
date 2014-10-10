
#ifndef __DC_TIMER_H
#define __DC_TIMER_H
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

void DC_InitTimer(void);
void DC_StartTimer(void);

#endif
