#ifndef __RTC_H
#define __RTC_H
#include "includes.h"

#define RTC_CLOCK_SOURCE_LSE

extern uint8_t RTC_HandlerFlag;
extern int8_t  RTC_Error;
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int8_t RTC_Configuration(void);
void rtc_init(void);

#endif /* __RTC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
