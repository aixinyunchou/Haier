
#ifndef __TEM_RH_H_
#define __TEM_RH_H_

#include "includes.h"

//温湿度传感器引脚
#define TEM_RH_SCL_GPIO          GPIOE
#define TEM_RH_SCL_PIN           GPIO_Pin_2
#define TEM_RH_SCL_PINS          GPIO_PinSource2
#define TEM_RH_SCL_CLK           RCC_AHB1Periph_GPIOE

#define TEM_RH_SDA_GPIO          GPIOE
#define TEM_RH_SDA_PIN           GPIO_Pin_3
#define TEM_RH_SDA_PINS          GPIO_PinSource3
#define TEM_RH_SDA_CLK           RCC_AHB1Periph_GPIOE

#define TEM_RH_SCL_0(void)       TEM_RH_SCL_GPIO->BSRRH = TEM_RH_SCL_PIN
#define TEM_RH_SCL_1(void)       TEM_RH_SCL_GPIO->BSRRL = TEM_RH_SCL_PIN

#define TEM_RH_SDA_0(void)       TEM_RH_SDA_GPIO->BSRRH = TEM_RH_SDA_PIN
#define TEM_RH_SDA_1(void)       TEM_RH_SDA_GPIO->BSRRL = TEM_RH_SDA_PIN
#define TEM_RH_SDA_STATE(void)   TEM_RH_SDA_GPIO->IDR & TEM_RH_SDA_PIN


#define TEM_RH_ADDR 										0x80

/**************************************************************************
**SHT20
**************************************************************************/
#define TEM_MEAS_COMMAND  	 						0xf3//0b11110011	guest
#define RH_MEAS_COMMAND  	 							0xf5//0b11110101	guest
#define TEM_RH_WRITE_STATUS_COMMAND 		0xe6//0b11100110
#define TEM_RH_READ_STATUS_COMMAND 			0xe7//0b11100111
#define TEM_RH_RESET_COMMAND  					0xfe//0b11111110

//#define ADDR_READ									0x81
#define ADDR_WRITE											0x80

#define TEM_RH_ACK  										0 
#define TEM_RH_NACK  										1 
#define TEM_RH_DATA_LOW_VALUE  					0
#define TEM_RH_DATA_HIGH_VALUE 					1
#define TEM_RH_DELAY_TIME  							50
#define TEM_RH_WAIT_DATA_READY_TIME 		5000
#define TEM_RH_WAIT_ACK_TIME 						1000
#define TEM_RH_TRYTIMES  	 							5
#define TEM_RH_OPERATE_OK   						0
#define TEM_RH_OPERATE_ERROR   					0xff


/*函数声明*/
void Tem_Rh_Gpio_Init(void);
void Tem_Rh_RHStart(void);
void Tem_Rh_RHStop(void);
void SendAck(uint8_t AckBit);
uint8_t TEM_RH_SendByte(uint8_t ucdata);
uint8_t TEM_RH_ReadByte(void);
uint8_t TEM_RH_RdStr(uint8_t ucSla,uint8_t ucAddress,uint8_t *ucBuf,uint8_t ucCount);
uint8_t TEM_RH_WrStr(uint8_t ucSla, uint8_t ucAddress, uint8_t *ucData, uint8_t ucNo);
void Tem_Rh_test(void);
void Tem_Rh_test2(void);
void Tem_Rh_Test3(void);


#endif /* __TEM_RH_H_ */

