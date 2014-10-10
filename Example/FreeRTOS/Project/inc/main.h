/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   This file contains all the functions prototypes for the main.c 
  *          file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4x7_eth_bsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
//#define USE_LCD        /* enable LCD  */  
//#define USE_DHCP       /* enable DHCP, if disabled static address is used*/
   
/* Uncomment SERIAL_DEBUG to enables retarget of printf to  serial port (COM1 on STM32 evalboard) 
for debug purpose */   
//#define SERIAL_DEBUG 
 
/* MAC ADDRESS*/
#define MAC_ADDR0   02
#define MAC_ADDR1   00
#define MAC_ADDR2   00
#define MAC_ADDR3   00
#define MAC_ADDR4   00
#define MAC_ADDR5   00
 
/*Static IP ADDRESS*/
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   106
   
/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1  

#define MII_MODE

/* Uncomment the define below to clock the PHY from external 25MHz crystal (only for MII mode) */
#ifdef 	MII_MODE
 #define PHY_CLOCK_MCO
#endif


/*--------------- Tasks Priority -------------*/
#define MAIN_TASK    					1 
#define WIFI_TASK							2 
#define TOUCH_TASK  					5
#define DC_TASK    		 				4 
#define AC_TASK      					3 

/*--------------- Tasks Stack ----------------*/
#define STACK_MAIN_TASK				200
#define STACK_WIFI_TASK				200
#define STACK_TOUCH_TASK			1000
#define STACK_DC_TASK					200
#define STACK_AC_TASK 				200

/*-------------- Tasks List ------------------*/
void Init(void);
void Main_task(void * pvParameters);
void Wifi_task(void * pvParameters);
void Touch_task(void * pvParameters);
void DC_task(void * pvParameters);
void AC_task(void * pvParameters);

void queue_init();
void ms_Delay(uint32_t time);
void us_Delay(uint32_t time);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

