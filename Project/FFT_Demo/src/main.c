#include "stm32f10x.h"
#include <math.h>
#include "stm32_dsp.h"
#include "table_fft.h"
#include "stm32f10x_usart.h"
#include "adc.h"
#include "stm32f10x_adc.h"
#include "stdio.h"
#include "24tft.h"

/** @addtogroup FFT_Demo
  * @{
  */ 


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PI2  6.28318530717959

#define NPT 1024          /* NPT = No of FFT point*/
#define DISPLAY_RIGHT 310 /* 224 for centered, 319 for right-aligned */
#define DISPLAY_LEFT 150  /* 224 for centered, 319 for right-aligned */
#define LED PBout(5)

USART_InitTypeDef USART_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint16_t TableFFT[];
extern volatile uint32_t TimingDelay ;
uint32_t lBUFIN[NPT];         /* Complex input vector */
uint32_t lBUFOUT[NPT];        /* Complex output vector */
uint32_t lBUFMAG[NPT + NPT/2];/* Magnitude vector */

/* Private function prototypes -----------------------------------------------*/
void MyDualSweep(uint32_t freqinc1,uint32_t freqinc2);
void MygSin(long nfill, long Fs, long Freq1, long Freq2, long Ampli);
void DSPDemoInit(void);
void STM_EVAL_COMInit(USART_InitTypeDef* USART_InitStruct);
void powerMag(long nfill, char* strPara);
void Send_String(const char *Data);
void LED_Init(void);
void adcfft(void);
void GPIO_Config(void);
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program.
  * @param  None
  * @retval : None
  */
int main(void)
{

  DSPDemoInit();
	STM_EVAL_COMInit(&USART_InitStructure);
  LED_Init();
	Adc_Init();
	GPIO_Config();
	ILI9325_CMO24_Initial();
  SPILCD_Clear(0x00);	
	LCD_PutString(0,20,"1.2´ç OLED"); 
  while (1)
  {
		adcfft();
	}
}


void adcfft(void)
{
	int i;
	char j[8];
	for(i=0;i<NPT;i++)
	 {
     lBUFIN[i]=(Get_Adc(ADC_Channel_8))<<16;
	 }
	 cr4_fft_1024_stm32(lBUFOUT,lBUFIN, NPT);
	 powerMag(NPT,"2SIDED");
	 for(i=0;i<NPT/2;i++)
	 {
     sprintf(j,"%ld",lBUFMAG[i]);
     Send_String(j);
	 }
}

void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;

 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //Ê¹ÄÜPB,PE¶Ë¿ÚÊ±ÖÓ
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //LED0-->PB.5 ¶Ë¿ÚÅäÖÃ
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //PB.5 Êä³ö¸ß
}


void Send_String(const char *Data)
{
	 u8 i;
   for(i=0;Data[i]!=0x00;i++)
	 {
	   USART_SendData(USART1,Data[i]);//Ïò´®¿Ú1·¢ËÍÊý¾Ý
	   while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//µÈ´ý·¢ËÍ½áÊø
	 }
	 USART_SendData(USART1,0x0d);//Ïò´®¿Ú1·¢ËÍÊý¾Ý
   while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//µÈ´ý·¢ËÍ½áÊø 
   USART_SendData(USART1,0x0a);//Ïò´®¿Ú1·¢ËÍÊý¾Ý
   while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//µÈ´ý·¢ËÍ½áÊø
}

void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);

																	  //BL        //RS            CS          SCK          MISO         SDA    
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	
//	GPIO_SetBits(GPIOA, GPIO_Pin_2);
// 	GPIO_SetBits(GPIOC, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);	 // turn off all led
}



/**
  * @brief  Compute power magnitude of the FFT transform
  * @param ill: length of the array holding power mag
  *   : strPara: if set to "1SIDED", removes aliases part of spectrum (not tested)
  * @retval : None
  */
void powerMag(long nfill, char* strPara)
{
  int16_t lX,lY;
  uint32_t i;
  float X,Y,Mag;
  for (i=0; i < nfill/2; i++)
  {
    lX= (lBUFOUT[i]<<16)>>16; /* sine_cosine --> cos */
    lY= (lBUFOUT[i] >> 16);   /* sine_cosine --> sin */    
    X=  NPT*((float)lX)/32768;
    Y = NPT*((float)lY)/32768;
    Mag = sqrt(X*X+ Y*Y)/nfill;
    lBUFMAG[i] = (uint32_t)(Mag*65536);   
  }
}




/**
  * @brief  Inserts a delay time.
  * @param ount: specifies the delay time length (time base 10 ms).
  * @retval : None
  */
void Delay(uint32_t nCount)
{
  /* Configure the systick */
  __enable_irq(); 
  	
	
     /* Setup SysTick Timer for 10 msec interrupts */
  if (SysTick_Config(SystemFrequency /100)) /* SystemFrequency is defined in
   “system_stm32f10x.h” and equal to HCLK frequency */
   {
    /* Capture error */
     while (1);
    }     
  /* Enable the SysTick Counter */
  TimingDelay = nCount;
   while (TimingDelay )  {}	    
   TimingDelay=0;
   __disable_irq();

}


void STM_EVAL_COMInit(USART_InitTypeDef* USART_InitStruct)
{
	
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 

  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(USART1, USART_InitStruct);
    
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
}




/**
  * @brief  Initializes the DSP lib demo (clock, peripherals and LCD).
  * @param  None
  * @retval : None
  */
void DSPDemoInit(void)
{
  ErrorStatus HSEStartUpStatus = ERROR;

  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }


  /* TIM1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  /* TIM2  and TIM4 clocks enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM4, ENABLE);



}

#ifdef  USE_FULL_ASSERT


/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 



/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
