#include "24tft.h"
#include "ASCII.h"
#include "GB1616.h"	//16*16ºº×Ö×ÖÄ£

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

//º¯ÊýÃû£ºLCD_WR_REG
//²Î  Êý£ºÎÞ
//        ×¼±¸¿ªÊ¼Ð´ÈëGRAM
void SPILCD_WriteRAM_Prepare(void)
{
	LCD_WriteRegIndex(0x22);   //Ð´RAM
}	 

/*************************************************
º¯ÊýÃû£ºLcd¹â±êÆðµã¶¨Î»º¯Êý
¹¦ÄÜ£ºÖ¸¶¨320240Òº¾§ÉÏµÄÒ»µã×÷ÎªÐ´Êý¾ÝµÄÆðÊ¼µã
Èë¿Ú²ÎÊý£ºx ×ø±ê 0~239
          y ×ø±ê 0~319
·µ»ØÖµ£ºÎÞ
*************************************************/
void Lcd_SetCursor(u16 x,u16 y)
{ 
  LCD_WriteRegIndex(0x20);
  LCD_WriteData(x);//Ë®Æ½×ø±ê
  LCD_WriteRegIndex(0x21);
  LCD_WriteData(y);//´¹Ö±×ø±ê 
} 

//º¯ÊýÃû£ºLCD_WR_REG
//²Î  Êý£ºÎÞ
//        Ð´GRAMÊý¾Ý
void SPILCD_WriteRAM(unsigned short RGB_Code)
{							    
    LCD_WriteData(RGB_Code); 
}
//º¯ÊýÃû£ºLCD_SetWindow
//²Î  Êý£ºXpos:ºá×ø±ê
//				Ypos:×Ý×ø±ê
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
//¿ªÆô·´É«ÏÔÊ¾
void LCD_InvDisplayOn()
{
  LCD_WriteRegIndex(0x21);  
}
//¹Ø±Õ·´É«ÏÔÊ¾
void LCD_InvDisplayOff()
{
  LCD_WriteRegIndex(0x20); 
}

/******************************************
º¯ÊýÃû£ºLcdÐ´ÃüÁîº¯Êý
¹¦ÄÜ£ºÏòLcdÖ¸¶¨Î»ÖÃÐ´ÈëÓ¦ÓÐÃüÁî»òÊý¾Ý
Èë¿Ú²ÎÊý£ºIndex ÒªÑ°Ö·µÄ¼Ä´æÆ÷µØÖ·
          ConfigTemp Ð´ÈëµÄÊý¾Ý»òÃüÁîÖµ
******************************************/
void LCD_WR_REG(u16 Index,u16 CongfigTemp)
{
	LCD_WriteRegIndex(Index);
	LCD_WriteByteSPI(CongfigTemp);
}

void ILI9325_CMO24_Initial(void)
{
	//¸´Î»
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);Delayms(100);
	GPIO_SetBits(GPIOA, GPIO_Pin_2);Delayms(100);
	
	SPI_CS(0);
	Delayms(25);

	WriteComm(0xae);

	WriteComm(0x15);    //   ÉèÖÃÁÐµØÖ·
	WriteComm(0x00);    //  start column   0
	WriteComm(0x3f);    //  end column   127

	WriteComm(0x75);    //   ÉèÖÃÐÐµØÖ·
	WriteComm(0x00);    //  start row   0
	WriteComm(0x7f);    //  end row   127

	WriteComm(0x81);	 // ÉèÖÃ¶Ô±È¶È
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

//º¯ÊýÃû£ºSPILCD_Init
//²Î  Êý£ºX Y ×ø±ê 
//        ÔÚX YÉÏ´òµã
void SPILCD_DrawPoint(unsigned short x,unsigned short y,unsigned short color)
{
	SPILCD_SetWindow(x,x+1,y,y+1);//ÉèÖÃ¹â±êÎ»ÖÃ 
	SPILCD_WriteRAM_Prepare();     //¿ªÊ¼Ð´ÈëGRAM	 
	SPILCD_WriteRAM(color);
}
void SPILCD_Fill(unsigned short xsta,unsigned short ysta,unsigned short xend,unsigned short yend,unsigned short color)
{                    
	unsigned short i,j;
	//ÉèÖÃ´°¿Ú		
	SPILCD_SetWindow(xsta,xend,ysta,yend);
	for(i=xsta;i<=xend;i++)
		for(j=ysta;j<=yend;j++)
	{
		   	SPILCD_WriteRAM(color);	  //ÏÔÊ¾ËùÌî³äµÄÑÕÉ«. 
// 		k=40000;while(k--);
	}
	//»Ö¸´ÉèÖÃ
	SPILCD_SetWindow(0,SPILCD_W-1,0,SPILCD_H-1);	    
}
//º¯ÊýÃû£ºSPILCD_Clear
//²Î  Êý£ºColor ÑÕÉ«      
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
		SPILCD_WriteRAM(Color);//ÏÔÊ¾ËùÌî³äµÄÑÕÉ«. 
// 		i=40000;while(i--);
	}
	SPI_CS(1);
} 
//º¯ÊýÃû£ºSPILCD_Clear_Fast
//²Î  Êý£ºsingle_Color µ¥É«  Ö»ÄÜÊÇ0»ò1£¬0ÊÇºÚÉ«£¬1Îª°×É«      
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
//SPILCD DRIVER  LV2²ã    ----   LV1 ÔÚLV0µÄ»ù´¡ÉÏÊµÏÖ
//µ¥×Ö·ûÏÔÊ¾
//×Ö·û´®ÏÔÊ¾
//Êý×ÖÏÔÊ¾

//º¯ÊýÃû£ºSPILCD_ShowChar
//²Î  Êý£º
//(x,y): 
//num:ÒªÏÔÊ¾µÄ×Ö·û:" "--->"~"
//size:×ÖÌå´óÐ¡ 12/16
//mode:µþ¼Ó·½Ê½(1)»¹ÊÇ·Çµþ¼Ó·½Ê½(0)
void SPILCD_ShowChar(unsigned char x,unsigned char y,unsigned char num)
{       
	unsigned char temp;
	unsigned int pos,t,i;  
	unsigned char size; 
	size=16;	//ÕÒµ½×ÖÌå´óÐ¡
  WriteComm(0x15);//SET COLUMN ADDR 
	WriteComm(x); 
	WriteComm(x+3); 
	WriteComm(0x75);//SET ROW ADDR 
	WriteComm(0x0+y); 
	WriteComm(0x0+y+15); 
	lcd_RS(1);
	SPI_CS(0);  
	num=num-' ';//µÃµ½Æ«ÒÆºóµÄÖµ
	i=num*16;

		for(pos=0;pos<size;pos++)
		{

			temp=nAsciiDot[i+pos];	//µ÷Í¨µ÷ÓÃÒÕÊõ×ÖÌå
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
	for (k=0;k<64;k++) { //64±êÊ¾×Ô½¨ºº×Ö¿âÖÐµÄ¸öÊý£¬Ñ­»·²éÑ¯ÄÚÂë
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
º¯ÊýÃû£ºLcdÍ¼ÏñÌî³ä100*100
¹¦ÄÜ£ºÏòLcdÖ¸¶¨Î»ÖÃÌî³äÍ¼Ïñ
Èë¿Ú²ÎÊý£º
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
// 	SPILCD_SetWindow(0,319,0,239);//Ð´ÍêÍ¼Æ¬ºó»Ö¸´Õû¸öÏÔÊ¾ÇøÓò

}

void SPILCD_drawline(unsigned char x,unsigned char y,uint32_t len)
{       
	unsigned char temp,pos;
	unsigned int t,i;  
	unsigned char size; 
  WriteComm(0x15);//SET COLUMN ADDR 
	WriteComm(x); 
	WriteComm(x+63); 
	WriteComm(0x75);//SET ROW ADDR 
	WriteComm(y); 
	WriteComm(y); 
	lcd_RS(1);
	SPI_CS(0);
	for(pos=0;pos<len;pos++)
	{             
		LCD_WriteoneSPI(0xff);
	}	 
		for(pos=0;pos<128-len;pos++)
	{             
		LCD_WriteoneSPI(0x00);
	}	 
}  
