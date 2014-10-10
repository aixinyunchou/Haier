

#if !defined( _I2C_H )
#define _I2C_H

#include "includes.h"

/*管脚定义*/
//距离感应光感应传感器、EEPROM引脚
#define I2C_SCL_GPIO          GPIOH
#define I2C_SCL_PIN           GPIO_Pin_7
#define I2C_SCL_PINS          GPIO_PinSource7
#define I2C_SCL_CLK           RCC_AHB1Periph_GPIOH

#define I2C_SDA_GPIO          GPIOH
#define I2C_SDA_PIN           GPIO_Pin_8
#define I2C_SDA_PINS          GPIO_PinSource8
#define I2C_SDA_CLK           RCC_AHB1Periph_GPIOH

////温湿度传感器引脚
//#define I2C_SCL_GPIO          GPIOH
//#define I2C_SCL_PIN           GPIO_Pin_7
//#define I2C_SCL_PINS          GPIO_PinSource7
//#define I2C_SCL_CLK           RCC_AHB1Periph_GPIOH

//#define I2C_SDA_GPIO          GPIOH
//#define I2C_SDA_PIN           GPIO_Pin_8
//#define I2C_SDA_PINS          GPIO_PinSource8
//#define I2C_SDA_CLK           RCC_AHB1Periph_GPIOH

#define TWI_SCL_0(void)       I2C_SCL_GPIO->BSRRH = I2C_SCL_PIN
#define TWI_SCL_1(void)       I2C_SCL_GPIO->BSRRL = I2C_SCL_PIN

#define TWI_SDA_0(void)       I2C_SDA_GPIO->BSRRH = I2C_SDA_PIN
#define TWI_SDA_1(void)       I2C_SDA_GPIO->BSRRL = I2C_SDA_PIN
#define TWI_SDA_STATE(void)   I2C_SDA_GPIO->IDR & I2C_SDA_PIN

/*函数声明*/
void TWI_Initialize(void);
void I2CStart(void);
void I2CStop(void);
void SendAck(uint8_t AckBit);
uint8_t I2CSendByte(uint8_t ucdata);
uint8_t I2CReadByte(void);
uint8_t I2CRdStr(uint8_t ucSla,uint8_t ucAddress,uint8_t *ucBuf,uint8_t ucCount);
uint8_t I2CWrStr(uint8_t ucSla, uint8_t ucAddress, uint8_t *ucData, uint8_t ucNo);
uint8_t I2CRdStr2(uint8_t ucSla,uint16_t usAddress, uint8_t *ucBuf,uint8_t ucCount);
uint8_t I2CWrStr2C(uint8_t ucSla,uint16_t usAddress, uint8_t *ucData, uint8_t ucNo);
uint8_t I2CWrStr2(uint8_t ucSla,uint16_t usAddress, uint8_t *ucData, uint8_t ucNo);
//void TWI_Init_Tem_Rh(void);

#endif
