#include "24tft.h"
#include "ASCII.h"
#include "GB1616.h"	//16*16汉字字模

#define SPILCD_W 176
#define SPILCD_H 240


void Delayms(unsigned short time)
{
	unsigned short i,j;
	for(i=0;i<time;i++)
		for(j=0;j<2600;j++)	;
}

void LCD_WriteByteSPI(unsigned char byte)
{
	  unsigned char buf;
    unsigned char i;
    for(i=0;i<8;i++) 
    {
        buf=(byte>>(7-i))&0x1;
        SPI_SDA(buf);
				SPI_DCLK(0);
        SPI_DCLK(1);
    }	
}
void LCD_WriteoneSPI(unsigned char byte)
{
	  unsigned char buf;
    unsigned char i;
	
    for(i=0;i<4;i++) 
    {
        buf=(byte>>(3-i))&0x1;
        SPI_SDA(buf);
				SPI_DCLK(0);
        SPI_DCLK(1);
    }	

}
void WriteComm(unsigned char dat)
{
	SPI_CS(0);
	lcd_RS(0);
	LCD_WriteByteSPI(dat);	//upper eight bits
	lcd_RS(1);
	SPI_CS(1);
}
void LCD_WriteRegIndex(unsigned char Index)
{
	lcd_RS(0);
	LCD_WriteByteSPI(Index);	//upper eight bits
	lcd_RS(1);
}
void LCD_WriteData(unsigned short dat)
{
		lcd_RS(1);
    LCD_WriteByteSPI(dat);
}

//函数名：LCD_WR_REG
//参  数：无
//        准备开始写入GRAM
void SPILCD_WriteRAM_Prepare(void)
{
	LCD_WriteRegIndex(0x22);   //写RAM
}	 

/*************************************************
函数名：Lcd光标起点定位函数
功能：指定320240液晶上的一点作为写数据的起始点
入口参数：x 坐标 0~239
          y 坐标 0~319
返回值：无
*************************************************/
void Lcd_SetCursor(u16 x,u16 y)
{ 
  LCD_WriteRegIndex(0x20);
  LCD_WriteData(x);//水平坐标
  LCD_WriteRegIndex(0x21);
  LCD_WriteData(y);//垂直坐标 
} 

//函数名：LCD_WR_REG
//参  数：无
//        写GRAM数据
void SPILCD_WriteRAM(unsigned short RGB_Code)
{							    
    LCD_WriteData(RGB_Code); 
}
//函数名：LCD_SetWindow
//参  数：Xpos:横坐标
//				Ypos:纵坐标
void SPILCD_SetWindow(unsigned short xstat,unsigned short xend,unsigned short ystat,unsigned short yend)
{
	//HX8367-A

	//Set GRAM Area 
	LCD_WR_REG(0x02,xstat>>8); 
	LCD_WR_REG(0x03,xstat&0xff);     //Column Start 
	LCD_WR_REG(0x04,xend>>8); 
	LCD_WR_REG(0x05,xend&0xff);     //Column End 
	 
	LCD_WR_REG(0x06,ystat>>8); 
	LCD_WR_REG(0x07,ystat&0xff);     //Row Start 
	LCD_WR_REG(0x08,yend>>8); 
	LCD_WR_REG(0x09,yend&0xff);     //Row End 
	  
	 LCD_WriteRegIndex(0x22);

}
//开启反色显示
void LCD_InvDisplayOn()
{
  LCD_WriteRegIndex(0x21);  
}
//关闭反色显示
void LCD_InvDisplayOff()
{
  LCD_WriteRegIndex(0x20); 
}

/******************************************
函数名：Lcd写命令函数
功能：向Lcd指定位置写入应有命令或数据
入口参数：Index 要寻址的寄存器地址
          ConfigTemp 写入的数据或命令值
******************************************/
void LCD_WR_REG(u16 Index,u16 CongfigTemp)
{
	LCD_WriteRegIndex(Index);
	LCD_WriteByteSPI(CongfigTemp);
}

void ILI9325_CMO24_Initial(void)
{
	//复位
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);Delayms(100);
	GPIO_SetBits(GPIOA, GPIO_Pin_2);Delayms(100);
	
	SPI_CS(0);
	Delayms(25);

	WriteComm(0xae);

	WriteComm(0x15);    //   设置列地址
	WriteComm(0x00);    //  start column   0
	WriteComm(0x3f);    //  end column   127

	WriteComm(0x75);    //   设置行地址
	WriteComm(0x00);    //  start row   0
	WriteComm(0x7f);    //  end row   127

	WriteComm(0x81);	 // 设置对比度
	WriteComm(0x80);


	WriteComm(0xa0);    //segment remap
	WriteComm(0x51);	  //

	WriteComm(0xa1);	 // start line
	WriteComm(0x00);

	WriteComm(0xa2);	 // display offset
	WriteComm(0x00);

	WriteComm(0xa4);    //normal display

	WriteComm(0xa8);    // set multiplex ratio
	WriteComm(0x7f);

	WriteComm(0xb1);	 // set phase leghth
	WriteComm(0xf1);

	WriteComm(0xb3);  // set dclk
	WriteComm(0x00);  //80Hz:0xc1	90Hz:0xe1  100Hz:0x00	110Hz:0x30 120Hz:0x50 130Hz:0x70

	WriteComm(0xab);	 // 
	WriteComm(0x01);	 //


	/*WriteComm(0xb8);
	WriteComm(0x00);
	WriteComm(0x08);
	WriteComm(0x10);
	WriteComm(0x18);
	WriteComm(0x20);
	WriteComm(0x28);
	WriteComm(0x30);
	WriteComm(0x38);*/

	WriteComm(0xb6);	 // set phase leghth
	WriteComm(0x0f);

	WriteComm(0xbe);
	WriteComm(0x0f);

	WriteComm(0xbc);
	WriteComm(0x08);

	WriteComm(0xd5);
	WriteComm(0x62);

	WriteComm(0xfd);
	WriteComm(0x12);

	WriteComm(0xaf); 
}

//函数名：SPILCD_Init
//参  数：X Y 坐标 
//        在X Y上打点
void SPILCD_DrawPoint(unsigned short x,unsigned short y,unsigned short color)
{
	SPILCD_SetWindow(x,x+1,y,y+1);//设置光标位置 
	SPILCD_WriteRAM_Prepare();     //开始写入GRAM	 
	SPILCD_WriteRAM(color);
}
void SPILCD_Fill(unsigned short xsta,unsigned short ysta,unsigned short xend,unsigned short yend,unsigned short color)
{                    
	unsigned short i,j;
	//设置窗口		
	SPILCD_SetWindow(xsta,xend,ysta,yend);
	for(i=xsta;i<=xend;i++)
		for(j=ysta;j<=yend;j++)
	{
		   	SPILCD_WriteRAM(color);	  //显示所填充的颜色. 
// 		k=40000;while(k--);
	}
	//恢复设置
	SPILCD_SetWindow(0,SPILCD_W-1,0,SPILCD_H-1);	    
}
//函数名：SPILCD_Clear
//参  数：Color 颜色      
void SPILCD_Clear(unsigned short Color)
{
	unsigned short x,y;  
	WriteComm(0x15);//SET COLUMN ADDR 
	WriteComm(0x00); 
	WriteComm(127); 
	WriteComm(0x75);//SET ROW ADDR 
	WriteComm(0x00); 
	WriteComm(95); 	
//	SPILCD_SetWindow(0,SPILCD_W-1,0,SPILCD_H-1);	
SPI_CS(0);	
	for(x=0;x<128;x++)
		for(y=0;y<48;y++)
	{
		SPILCD_WriteRAM(Color);//显示所填充的颜色. 
// 		i=40000;while(i--);
	}
	SPI_CS(1);
} 
//函数名：SPILCD_Clear_Fast
//参  数：single_Color 单色  只能是0或1，0是黑色，1为白色      
void SPILCD_Clear_Fast(unsigned char single_Color)
{
	unsigned int x,y;
  int temp;	
	SPILCD_SetWindow(0,SPILCD_W-1,0,SPILCD_H-1);	
	SPI_SDA(single_Color);
	lcd_RS(1);
	temp=SPILCD_W<<4;
	for(x=0;x<temp;x++)
		for(y=0;y<SPILCD_H;y++)
	{
    SPI_DCLK(0);
    SPI_DCLK(1);
	}
} 
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
//SPILCD DRIVER  LV2层    ----   LV1 在LV0的基础上实现
//单字符显示
//字符串显示
//数字显示

//函数名：SPILCD_ShowChar
//参  数：
//(x,y): 
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16
//mode:叠加方式(1)还是非叠加方式(0)
void SPILCD_ShowChar(unsigned char x,unsigned char y,unsigned char num)
{       
	unsigned char temp;
	unsigned int pos,t,i;  
	unsigned char size; 
	size=16;	//找到字体大小
  WriteComm(0x15);//SET COLUMN ADDR 
	WriteComm(x); 
	WriteComm(x+3); 
	WriteComm(0x75);//SET ROW ADDR 
	WriteComm(0x0+y); 
	WriteComm(0x0+y+15); 
	lcd_RS(1);
	SPI_CS(0);  
	num=num-' ';//得到偏移后的值
	i=num*16;

		for(pos=0;pos<size;pos++)
		{

			temp=nAsciiDot[i+pos];	//调通调用艺术字体
			for(t=0;t<8;t++)
		   {                 
		      if(temp&0x80)
						LCD_WriteoneSPI(0xff);
					else 
						LCD_WriteoneSPI(0x00);
		      temp<<=1; 
		    }
		}	 
}  
void DrawPixel(u16 x, u16 y, int Color)
{
	SPILCD_SetWindow(x,x,y,y); 
  SPILCD_WriteRAM(Color);							  
}
void PutGB1616(unsigned char x, unsigned char  y, unsigned char c[2])
{
	unsigned int i,j,k;
	unsigned short m;
	WriteComm(0x15);//SET COLUMN ADDR 
	WriteComm(0x0+x); 
	WriteComm(0x0+x+7); 
	WriteComm(0x75);//SET ROW ADDR 
	WriteComm(0x0+y); 
	WriteComm(0x0+y+15); 
//	SPILCD_SetWindow(0,SPILCD_W-1,0,SPILCD_H-1);	
	lcd_RS(1);
  SPI_CS(0);	

// 	SPILCD_SetWindow(0,SPILCD_H-1,0,SPILCD_W-1);
	for (k=0;k<64;k++) { //64标示自建汉字库中的个数，循环查询内码
	  if ((codeGB_16[k].Index[0]==c[0])&&(codeGB_16[k].Index[1]==c[1]))
			{ 
    	for(i=0;i<32;i++) 
			{
				m=codeGB_16[k].Msk[i];
				for(j=0;j<8;j++) 
				{		
					if((m&0x80)==0x80) {
						LCD_WriteoneSPI(0xff);
						}
					else {
						LCD_WriteoneSPI(0x00);
						}
					m=m<<1;
				} 
				if(i%2){y++;x=x-8;}
				else x=x+8;
		  }
		}  
	  }	
	}
void LCD_PutString(unsigned char x, unsigned char y, unsigned char *s) 
{
	unsigned char l=0;
	while(*s) 
		{
			if( *s < 0x80) 
				{
					SPILCD_ShowChar(x+l,y,*s);
					s++;l+=4;
				}
			else
				{
					PutGB1616(x+l,y,(unsigned char*)s);
					s+=2;l=l+8;
				}
		}
}
/******************************************
函数名：Lcd图像填充100*100
功能：向Lcd指定位置填充图像
入口参数：
******************************************/
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic)
{
  unsigned long i;
	unsigned int j;
	SPILCD_SetWindow(x,x+pic_H-1,y,y+pic_V-1);

// 	lcd_RS(1);
	for (i = 0; i < pic_H*pic_V*2; i++)
	{
    LCD_WriteByteSPI(pic[i]);
	}
// 	SPILCD_SetWindow(0,319,0,239);//写完图片后恢复整个显示区域

}
