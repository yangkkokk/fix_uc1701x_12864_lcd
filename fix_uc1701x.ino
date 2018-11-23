/*  
    因为这个驱动程序是移植自8051的，原作者是厂家，但是因为不
    知道啥原因，时序总是对不上，所以特别写了这个修复驱动程序
    用于arduino等软件体系



  液晶演示程序
  液晶模块型号：JLX12864G-086-PN(或12864G-0088)，串行接口！
    驱动IC是:UC1701X或兼容的IC,比如ST7565R
    编写：叶建人,11月5日,2013年
    重写人:杨飞,11月28日,2018年
*/

//======================================================


#define cs1 D2   //接口定义:lcd_cs1就是LCD的cs1
#define reset D3 //接口定义:lcd_reset就是LCD的reset
#define rs D4    //接口定义:lcd_rs就是LCD的rs

#define sid D5  //接口定义:lcd_sid就是LCD的sid
#define sclk D6  //接口定义:lcd_sclk就是LCD的sclk
#define led D7


#define uchar unsigned char 
#define uint unsigned int
#define ulong unsigned long


//用“zimo221.exe”取模出来的汉字的16进制码：
char  zhuang1[]={
/*--  文字:  状  --*/
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x08,0x30,0x00,0xFF,0x20,0x20,0x20,0x20,0xFF,0x20,0xE1,0x26,0x2C,0x20,0x20,0x00,
0x04,0x02,0x01,0xFF,0x40,0x20,0x18,0x07,0x00,0x00,0x03,0x0C,0x30,0x60,0x20,0x00};

char  tai1[]={
/*--  文字:  态  --*/
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x00,0x04,0x04,0x04,0x84,0x44,0x34,0x4F,0x94,0x24,0x44,0x84,0x84,0x04,0x00,0x00,
0x00,0x60,0x39,0x01,0x00,0x3C,0x40,0x42,0x4C,0x40,0x40,0x70,0x04,0x09,0x31,0x00};

char  shi1[]={
/*--  文字:  使  --*/
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x40,0x20,0xF0,0x1C,0x07,0xF2,0x94,0x94,0x94,0xFF,0x94,0x94,0x94,0xF4,0x04,0x00,
0x00,0x00,0x7F,0x00,0x40,0x41,0x22,0x14,0x0C,0x13,0x10,0x30,0x20,0x61,0x20,0x00};

char  yong1[]={
/*--  文字:  用  --*/
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x00,0x00,0x00,0xFE,0x22,0x22,0x22,0x22,0xFE,0x22,0x22,0x22,0x22,0xFE,0x00,0x00,
0x80,0x40,0x30,0x0F,0x02,0x02,0x02,0x02,0xFF,0x02,0x02,0x42,0x82,0x7F,0x00,0x00};



/*
void delay(int i)               
{
  int j,k;
  for(j=0;j<i;j++)
    for(k=0;k<110;k++);
}


//短延时
void delay_us(int i)               
{
  int j,k;
  for(j=0;j<i;j++)
    for(k=0;k<1;k++);
}
*/



/*写指令到LCD模块*/
void transfer_command(int data1)   
{/*
  char i;
  digitalWrite(cs1,0);
  digitalWrite(rs,0);
  for(i=0;i<8;i++)
  {
    digitalWrite(sclk,0);
    if(data1&0x80) digitalWrite(sid,1);
    else digitalWrite(sid,0);
    digitalWrite(sclk,1);
    data1<<(1);
  }
  digitalWrite(cs1,1);
  */
  digitalWrite(cs1,0);
  digitalWrite(rs,0);
  delayMicroseconds(76);
  
  shiftOut(sid, sclk, MSBFIRST, data1); 
  digitalWrite(cs1,1);
}

/*写数据到LCD模块*/
void transfer_data(int data1)
{/*
  char i;
  digitalWrite(cs1,0);
  digitalWrite(rs,1);
  for(i=0;i<8;i++)
  {
    digitalWrite(sclk,0);
    if(data1&0x80) digitalWrite(sid,1);
    else digitalWrite(sid,0);
    digitalWrite(sclk,1);
    data1<<(1);
  }
  digitalWrite(cs1,1);
  */
  digitalWrite(cs1,0);
  digitalWrite(rs,1);
  delayMicroseconds(76);
  shiftOut( sid, sclk,MSBFIRST, data1); //串行脉冲发送函数
  digitalWrite(cs1,1);
}


/*LCD模块初始化*/
void initial_lcd()
{
  pinMode(cs1,OUTPUT);   //设置各个I/O为输出
  pinMode(reset,OUTPUT);  
  pinMode(rs,OUTPUT);
  pinMode(sclk,OUTPUT);
  pinMode(sid,OUTPUT);  
  pinMode(led,OUTPUT); 
/*
digitalWrite(cs1,1);
digitalWrite(reset,1);
digitalWrite(rs,1);
digitalWrite(sid,1);
digitalWrite(sclk,1);
digitalWrite(led,0);
*/

  digitalWrite(cs1,0);
    digitalWrite(reset,0);        /*低电平复位*/
    delay(500);
    digitalWrite(reset,1);        /*复位完毕*/
    delay(200);        
  transfer_command(0xe2);  /*软复位*/
  delay(50);
  transfer_command(0x2c);  /*升压步聚1*/
  delay(50);  
  transfer_command(0x2e);  /*升压步聚2*/
  delay(50);
  transfer_command(0x2f);  /*升压步聚3*/
  delay(50);
  transfer_command(0x23);  /*粗调对比度，可设置范围0x20～0x27*/
  transfer_command(0x81);  /*微调对比度*/
  transfer_command(0x28);  /*0x28,微调对比度的值，可设置范围0x00～0x3f*/
  transfer_command(0xa2);  /*1/9偏压比（bias）*/
  transfer_command(0xc8);  /*行扫描顺序：从上到下*/
  transfer_command(0xa0);  /*列扫描顺序：从左到右*/
  transfer_command(0x40);  /*起始行：第一行开始*/
  transfer_command(0xaf);  /*开显示*/
 
}


void lcd_address(uchar page,uchar column)
{
  column=column-1;                //我们平常所说的第1列，在LCD驱动IC里是第0列。所以在这里减去1.
  page=page-1;
  transfer_command(0xb0+page);        //设置页地址。每页是8行。一个画面的64行被分成8个页。我们平常所说的第1页，在LCD驱动IC里是第0页，所以在这里减去1*/
  transfer_command(((column>>(4))&0x0f)+0x10);  //设置列地址的高4位
  transfer_command(column&0x0f);        //设置列地址的低4位
}

/*全屏清屏*/
void clear_screen()
{
  uchar i,j;
  for(i=0;i<9;i++)
  {
    lcd_address(1+i,1);
    for(j=0;j<132;j++)
    {
      transfer_data(0x00);
    }
  }
}




//显示16x16点阵图像、汉字。
//括号里的参数分别是（页，列，闪烁与否，反显与否，数据指针）
void disp_blink_16x16(uchar page,uchar column,uchar blink,uchar reverse,uchar *dp)
{
  uchar i,j,k,*data_temp;
  if(blink==0)
  {
    data_temp=dp;
    for(j=0;j<2;j++)
    {
      lcd_address(page+j,column);
      for (i=0;i<16;i++)
      { 
        if(reverse==1)
        {
          transfer_data(~*data_temp); /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
        }
        else
        {
          transfer_data(*data_temp);    /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
        }
        data_temp++;
      }
    }
  }

  else
  {
    for(k=0;k<10;k++)
    {
      data_temp=dp;
      for(j=0;j<2;j++)
      {
        lcd_address(page+j,column);
        for (i=0;i<16;i++)
        { 
          transfer_data(~*data_temp);   /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
          data_temp++;
        }
      }
      delay(2000);
      data_temp=dp;
      for(j=0;j<2;j++)
      {
        lcd_address(page+j,column);
        for (i=0;i<16;i++)
        { 
          transfer_data(*data_temp);    /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
          data_temp++;
        }
      }
      delay(2000);

      }
  }
}

//显示16x16点阵图像、汉字。
//括号里的参数分别是（页，列，带下划线闪烁与否，反显与否，数据指针）
void disp_underline_16x16(uchar page,uchar column,uchar under,uchar reverse,uchar *dp)
{
  uchar i,j,k,*data_temp,data1;
  if(under==0)
  {
    data_temp=dp;
    for(j=0;j<2;j++)
    {
      lcd_address(page+j,column);
      for (i=0;i<16;i++)
      { 
        if(reverse==1)
        {
          transfer_data(~*data_temp); /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
        }
        else
        {
          transfer_data(*data_temp);    /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
        }
        data_temp++;
      }
    }
  }

  else
  {
    for(k=0;k<10;k++)
    {
      data_temp=dp;
  
      lcd_address(page,column);
      for (i=0;i<16;i++)
      { 
        transfer_data(*data_temp);    /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
        data_temp++;
      }
      lcd_address(page+1,column);
      for (i=0;i<16;i++)
      { 
        data1=*data_temp|0x80;
        transfer_data(data1);   /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
        data_temp++;
      }
      delay(2000);


      data_temp=dp;
      for(j=0;j<2;j++)
      {
        lcd_address(page+j,column);
        for (i=0;i<16;i++)
        { 
          transfer_data(*data_temp);    /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
          data_temp++;
        }
      }
      delay(2000);

      }
  }
}
//显示16x16点阵图像、汉字。
//括号里的参数分别是（页，列，带右竖线光标闪烁与否，反显与否，数据指针）
void disp_cursor_16x16(uchar page,uchar column,uchar cursor,uchar reverse,uchar *dp)
{
  uchar i,j,k,*data_temp;
  if(cursor==0)
  {
    data_temp=dp;
    for(j=0;j<2;j++)
    {
      lcd_address(page+j,column);
      for (i=0;i<16;i++)
      { 
        if(reverse==1)
        {
          transfer_data(~*data_temp); /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
        }
        else
        {
          transfer_data(*data_temp);    /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
        }
        data_temp++;
      }
    }
  }

  else
  {
    for(k=0;k<10;k++)
    {
      data_temp=dp;
  
      lcd_address(page,column);
      for (i=0;i<15;i++)
      { 
        transfer_data(*data_temp);    /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
        data_temp++;
      }
      transfer_data(0xff);
      data_temp++;
      lcd_address(page+1,column);
      for (i=0;i<15;i++)
      { 
        transfer_data(*data_temp);    /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
        data_temp++;
      }
      transfer_data(0xff);
      data_temp++;
      delay(2000);


      data_temp=dp;
      for(j=0;j<2;j++)
      {
        lcd_address(page+j,column);
        for (i=0;i<16;i++)
        { 
          transfer_data(*data_temp);    /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
          data_temp++;
        }
      }
      delay(2000);

      }
  }
}


void setup()
{ 
  //digitalWrite(led,1);
  initial_lcd();
  digitalWrite(led,1);
}
  void loop()
  {
    


    clear_screen();               //clear all dots    
    disp_blink_16x16(5, 1,0,1,zhuang1);   //显示单个汉字"状". 括号里的参数分别是（页，列，闪烁与否，反显与否，数据指针）
    disp_blink_16x16(5,17,0,0,tai1);    //显示单个汉字"态",不闪烁，反显
    disp_blink_16x16(5,33,1,0,shi1);    //显示单个汉字"使",闪烁，不反显
    disp_blink_16x16(5,49,1,1,yong1);   //显示单个汉字"用",闪烁，反显（当有闪烁时，反显不起作用）
    //waitkey();
    delay(2000);
    clear_screen();               //clear all dots    
    disp_underline_16x16(5, 1,0,1,zhuang1); //显示单个汉字"状". 括号里的参数分别是（页，列，是否有下划线闪烁，反显与否，数据指针）
    disp_underline_16x16(5,17,0,0,tai1);  //显示单个汉字"态"
    disp_underline_16x16(5,33,1,0,shi1);  //显示单个汉字"使"
    disp_underline_16x16(5,49,1,1,yong1); //显示单个汉字"用"
    delay(2000);

    clear_screen();               //clear all dots    
    disp_cursor_16x16(5, 1,0,1,zhuang1);  //显示单个汉字"状". 括号里的参数分别是（页，列，是否右竖线光标闪烁，反显与否，数据指针）
    disp_cursor_16x16(5,17,0,0,tai1);   //显示单个汉字"态"
    disp_cursor_16x16(5,33,1,0,shi1);     //显示单个汉字"使"
    disp_cursor_16x16(5,49,1,1,yong1);    //显示单个汉字"用"
    delay(2000);

  }
