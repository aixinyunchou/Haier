
#include "stm32f4xx.h"
#include "DC_Timer.h"

extern void MainCallback( xTimerHandle pxTimer);
extern void SendCallback( xTimerHandle pxTimer);
xTimerHandle xTimerMain, xTimerInit , xTimerSend;

void DC_InitTimer(void)
{
	xTimerMain = xTimerCreate("TimerMain",(240000 / portTICK_RATE_MS),pdFALSE,(void *)0,MainCallback);
	xTimerInit = xTimerCreate("TimerInit",(2000   / portTICK_RATE_MS),pdTRUE ,(void *)1,SendCallback);
	xTimerSend = xTimerCreate("TimerSend",(200    / portTICK_RATE_MS),pdTRUE ,(void *)2,SendCallback);
}

void DC_StartTimer(void)
{
	if(xTimerMain != NULL) xTimerStart( xTimerMain, 0 );
	if(xTimerInit != NULL) xTimerStart( xTimerInit, 0 );
}

