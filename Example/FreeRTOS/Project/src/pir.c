#include "pir.h"

#define CYCTIMES 100 //计数次数，无人的时候标准差在90左右


uint16_t pir_data[3]; // 16 Bit values (integer)
float databuffer[CYCTIMES];
uint8_t i=0;//Filter_Limit()循环参数
uint8_t firsttime=0;//databuffer填充的标志
uint8_t positive=0,negative=0;//正负脉冲次数

float limitdata=3.5;//门限值
uint8_t passdata=3;//超过门限值次数

/*******************************************************************************
  * @name		Init_Pir_Port_Out
  * @brief	Config GPIO Input Mode
  * @inparam	none
  * @outparam	none
  * @return		none
*******************************************************************************/
void Init_Pir_Port_Out(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(GPIO_CLK, ENABLE);
	
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = PIR_OUT_PIN;
	
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*******************************************************************************
  * @name		Init_Pir_Port_In
  * @brief		Config GPIO Output Mode
  * @inparam	none
  * @outparam	none
  * @return		none
*******************************************************************************/
void Init_Pir_Port_In(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(GPIO_CLK, ENABLE);	
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = PIR_OUT_PIN;
	
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}

/*******************************************************************************
  * @name		Get_Pir_Value(5868)
  * @brief		Config GPIO Output Mode
  * @inparam	none
  * @outparam	the counts of  ADC Output Offset for Temperature
  * @return		the counts of  ADC Output Offset for Temperature
*******************************************************************************/
uint16_t Get_Pir_Value(void)
{

	uint16_t i, j, k,GetBit;
	//uint16_t pir_data[3]; // 16 Bit values (integer)
	
	for (i=0; i<3;i++)
		pir_data[i] = 0; // initialize Pyroelectric Passive Infrared Detector
	Init_Pir_Port_In();  //设置为输入口
	
	do
	{ 
		GetBit=GPIOC->IDR&PIR_OUT_PIN;
	}while (GetBit == 0); // wait for GetBit = high	
	
	for (i=0; i < 65535; i++); // wait appr. 100-150 usec (tS)
	
	for (i=0; i<3; i++)
	{ for (j=0; j < 14; j++)
		{ 
			GPIO_ResetBits(GPIOC,PIR_OUT_PIN);  //设置为0
			Init_Pir_Port_Out();
			GPIO_ResetBits(GPIOC,PIR_OUT_PIN);  //设置为0
			for(k=0;k<10;k++);	//1 us
			GPIO_SetBits(GPIOC,PIR_OUT_PIN);//设置为1
			for(k=0;k<20;k++); //1 us
			Init_Pir_Port_In();  //设置为输入口
			for(k=0;k<100;k++); // 10us
			pir_data[i] <<=1; // shift Bits left
			GetBit=GPIOC->IDR&PIR_OUT_PIN;		
			if (GetBit)
				pir_data[i]++; // sample High bit
		}
  }  
		Init_Pir_Port_Out();
	  GPIO_ResetBits(GPIOC,PIR_OUT_PIN);  //设置为0
		for(k=0;k<20;k++);
		Init_Pir_Port_In();
		return(pir_data[1]); // return Array of 3x 14 Bit values of Pyroelectric Passive Infrared Detector
				
}


/*******************************************************************************
  * @name		Get_Pir_Value_1794(12.1ms,82Hz)
  * @brief		Config GPIO Output Mode
  * @inparam	none
  * @outparam	the counts of  ADC Output Offset for Temperature
  * @return		the counts of  ADC Output Offset for Temperature
*******************************************************************************/
uint16_t Get_Pir_Value_1794(void)
{

	uint16_t i, j, k,GetBit;
	//uint16_t pir_data[3]; // 16 Bit values (integer)
	
	for (i=0; i<2;i++)
		pir_data[i] = 0; // initialize Pyroelectric Passive Infrared Detector
	Init_Pir_Port_In();  //设置为输入口
	
	do
	{ 
		GetBit=GPIOC->IDR&PIR_OUT_PIN;
	}while (GetBit == 0); // wait for GetBit = high	
	
	for (i=0; i < 65535; i++); // wait appr. 100-150 usec (tS)
	
	for (i=0; i<2; i++)
	{ for (j=0; j < 14; j++)
		{ 
			GPIO_ResetBits(GPIOC,PIR_OUT_PIN);  //设置为0
			Init_Pir_Port_Out();
			GPIO_ResetBits(GPIOC,PIR_OUT_PIN);  //设置为0
			for(k=0;k<10;k++);	//1 us
			GPIO_SetBits(GPIOC,PIR_OUT_PIN);//设置为1
			for(k=0;k<20;k++); //1 us
			Init_Pir_Port_In();  //设置为输入口
			for(k=0;k<100;k++); // 10us
			pir_data[i] <<=1; // shift Bits left
			GetBit=GPIOC->IDR&PIR_OUT_PIN;		
			if (GetBit)
				pir_data[i]++; // sample High bit
		}
  }  
		Init_Pir_Port_Out();
	  GPIO_ResetBits(GPIOC,PIR_OUT_PIN);  //设置为0
		for(k=0;k<20;k++);
		Init_Pir_Port_In();
		return(pir_data[0]); // return Array of 3x 14 Bit values of Pyroelectric Passive Infrared Detector
				
}



/*********************************************
  * @name		Filter_Limit
  * @brief	递推法
  * @inparam	none
  * @outparam	0--没人，1--有人
  * @return		0--没人，1--有人
*********************************************/

uint8_t Filter_Limit()
{
	uint8_t count;
	uint16_t work;
	float temp;//有符号

	if(firsttime==0)//第一次给整个buffer填满数据或者检测到有人后重新填充数据
	{
		for(i=0;i<CYCTIMES-1;i++)
		{
			work = Get_Pir_Value();
			temp =  filterloop(work);
			databuffer[i] = temp;
//			debug_printf("%f\r\n", temp);
			ms_Delay(16);
		}
		firsttime=1;
	}

	work = Get_Pir_Value();
	temp = filterloop(work);
	databuffer[i++] = temp;
//	debug_printf("%f\r\n", temp);
	
	if(i==CYCTIMES)
	{
		i=0;

	}
	for(count=0;count<CYCTIMES;count++)
	{
		temp = databuffer[count];
		if(temp>limitdata)
			positive++;
		if(temp<-limitdata)
			negative++;
		if(positive>=passdata && negative>=passdata)
		{
			positive = 0;
			negative = 0;
			firsttime = 0;//以便下次重新填充数据
			return 1;//有人
		}
	}
	positive = 0;
	negative = 0;
	return 0;

}

/*********************************************
  * @name		Filter_Limit_1794
  * @brief	递推阈值法
  * @inparam	none
  * @outparam	0--没人，1--有人
  * @return		0--没人，1--有人
*********************************************/

uint8_t Filter_Limit_1794()
{
	uint8_t count;
	uint16_t work;
	float temp;//经过滤波器的输出是有符号的

	if(firsttime==0)//第一次给整个buffer填满数据或者检测到有人后重新填充数据
	{
		for(i=0;i<CYCTIMES-1;i++)
		{
			work = Get_Pir_Value_1794();
			temp =  filterloop(work);
			databuffer[i] = temp;
//			debug_printf("%f\r\n", temp);
			ms_Delay(16);
		}

		firsttime=1;

	}

	work = Get_Pir_Value_1794();
	temp = filterloop(work);
	databuffer[i++] = temp;
//	debug_printf("%f\r\n", temp);
	
	if(i==CYCTIMES)//buffer满了
	{
		i=0;

	}
	for(count=0;count<CYCTIMES;count++)
	{
		temp = databuffer[count];
		if(temp>limitdata)
			positive++;
		if(temp<-limitdata)
			negative++;
		if(positive>=passdata && negative>=passdata)
		{
			positive = 0;
			negative = 0;
			firsttime = 0;//以便下次重新填充数据
			return 1;//有人
		}
	}
	positive = 0;
	negative = 0;
	return 0;

}


/*********************************************
  * @name		Pir_Switch
  * @brief	检测到有人时，LED灯闪烁一次
  * @inparam	none
  * @outparam	none
  * @return		none
*********************************************/
void Pir_Switch()
{
	uint16_t i=292;//检测到有人，LED亮，蜂鸣器响，延时5s，LED灭
		if(1==Filter_Limit())
		{
			GPIO_SetBits(GPIOH, GPIO_Pin_6);
//			GPIO_SetBits(GPIOE, GPIO_Pin_3);			
			Buzzer_On(200);
			while(i--)
				filterloop(Get_Pir_Value());
//			ms_Delay(5000);
//			GPIO_ResetBits(GPIOE, GPIO_Pin_3);
			GPIO_ResetBits(GPIOH, GPIO_Pin_6);
		}

}

/*********************************************
  * @name		Pir_Switch
  * @brief	检测到有人时，LED灯闪烁一次
  * @inparam	none
  * @outparam	none
  * @return		none
*********************************************/
void Pir_Switch_1794()
{
	uint16_t i=292;//检测到有人，LED亮，蜂鸣器响，延时5s，LED灭
	float temp;
	temp = temp;
		if(1==Filter_Limit_1794())
		{
			GPIO_SetBits(GPIOH, GPIO_Pin_6);
//			GPIO_SetBits(GPIOE, GPIO_Pin_3);			
			Buzzer_On(200);
			while(i--)
			{	
				temp=filterloop(Get_Pir_Value_1794());
//				debug_printf("%f\r\n", temp);
			}
//			GPIO_ResetBits(GPIOE, GPIO_Pin_3);
			GPIO_ResetBits(GPIOH, GPIO_Pin_6);
		}

}

/*********************************************
  * @name		Printf_Pir_Data
  * @brief	串口输出人体红外数据
  * @inparam	none
  * @outparam	none
  * @return		none
*********************************************/
void Printf_Pir_Data(uint16_t type)
{
		//读人体红外
		if(type==5868)
			Get_Pir_Value();
		if(type==1794)
			Get_Pir_Value_1794();	
		//channel 1 
		debug_printf("%d\r\n", pir_data[0]);

		//channel 0 
		debug_printf("%d\r\n", pir_data[1]);
	
		//Temperature
		debug_printf("%d\r\n", pir_data[2]);

		
}
//滤波器：3阶 采样频率20Hz 带通0.3Hz-7Hz 
/* comparison test code from http://www-users.cs.york.ac.uk/~fisher/mkfilter/
   (the above page kicks ass, BTW)*/

#define NZEROS 6
#define NPOLES 6
#define GAIN   2.976558285e+00


static double xv[NZEROS+1], yv[NPOLES+1];

static double filterloop(double next)
{ 
  xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; 
  xv[6] = next / GAIN;
	yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; 

  yv[6] = (xv[6] - xv[0]) + 3 * (xv[2] - xv[4])
					+ ( 0.1089457597 * yv[0]) + ( 0.2210296547 * yv[1])
					+ ( -0.5058548557 * yv[2]) + ( -0.3660650521 * yv[3])
					+ ( -0.1338100547 * yv[4]) + ( 1.6733292117 * yv[5]);
  return(yv[6]);
}


void Pir_TEST(uint16_t type)
{
	if(type==5868)
	{
			Pir_Switch();
	}
	if(type==1794)
	{
			Pir_Switch_1794();
	}
}

//上电启动后要延时一段时间，得到0点值得波动
double LimitData()
{
		uint8_t times=200;
		uint16_t i,work;
		double temp,max,limit;
		
	while(times--)
		filterloop(Get_Pir_Value_1794());

	for(i=0;i<CYCTIMES*5;i++)
	{
		work = Get_Pir_Value_1794();
		temp =  filterloop(work);
		temp = fabs(temp);//取绝对值
		if(i==0)
		{
			max = temp;
		}
		else if(max<=temp)
			max = temp;			
//			debug_printf("%f\r\n", temp);
//			ms_Delay(16);
	}
	limit = max+0.5;
	return limit;
}

