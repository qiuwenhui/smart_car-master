#include "common.h"


u8 Pixel[128]={0};
u8 CCD2_Pixel[128]={0};

s16 L_Line_1[10] = {20,20,20,20,20,20,20,20,20,20};
s16 R_Line_1[10] = {102,102,102,102,102,102,102,102,102,102};
s16 Track_C_Line_1[10] = {64,64,64,64,64,64,64,64,64,64};
s16 CCD2_C_Line_1[10] = {64,64,64,64,64,64,64,64,64,64};

u8 CCD2_Enable=0;
u8 stop_count=0,stop_flag=1;

#define THRESHOLD 38

void ccd_data_processing()
{

  int16 old_center_line;     
  u8 i=0;

   old_center_line = Track_C_Line_1[0];    
  
  if(old_center_line>=28 && old_center_line<=100)
  {
    for(i = old_center_line;i < 124;i++)
    {
       R_Line_1[0] = i; 
       if(Pixel[i-4] > ( Pixel[i] + THRESHOLD))   break; //�����ز�ֵ
    }
    for(i = old_center_line;i > 4;i--)
    {
       L_Line_1[0] = i;  
       if(Pixel[i+4] >( Pixel[i] + THRESHOLD))    break;         
    }
  }
  else if(old_center_line<28)
  {
    for(i = 9;i<124;i++)
    {
       R_Line_1[0] = i;  
       if(Pixel[i-4] >( Pixel[i] + THRESHOLD))    break;  
    }
    L_Line_1[0] = 5;
  }
  else if(old_center_line>100)
  {
    for(i = 119;i>4;i--)
    {
       L_Line_1[0] = i;  
       if(Pixel[i+4] >( Pixel[i] + THRESHOLD))    break;  
    }
    R_Line_1[0] = 123;
  } 

  
  
   //��������
  if((R_Line_1[0] != 123) || (L_Line_1[0] != 5))
  {
    CCD2_Enable=0;
    stop_count=0;
    
     Track_C_Line_1[0] = (R_Line_1[0]+L_Line_1[0]) >> 1; //Ϊ�˼ӿ����Ч�ʣ���������λ��ʽ����     
     
  }
  else
  { 
      CCD2_Enable=1;

      // ****  CCD2���ݴ���  *******************************
    for(i = 50;i < 124;i++)
    {
       R_Line_1[1] = i; 
       if(CCD2_Pixel[i-4] > ( CCD2_Pixel[i] + THRESHOLD))   break; //�����ز�ֵ
    }
    for(i = 80;i > 4;i--)
    {
       L_Line_1[1] = i;  
       if(CCD2_Pixel[i+4] >( CCD2_Pixel[i] + THRESHOLD))    break;         
    }          

    if(R_Line_1[1] == 123 && L_Line_1[1] ==5 )  
    {
      stop_count++;
    
    }
      if(stop_count>20)
        stop_flag=1;
    
      
      Track_C_Line_1[0] = (R_Line_1[1]+L_Line_1[1]) >> 1;
      
      
   // Track_C_Line_1[0] = 64;   //Ϊ����Ӧʮ���䣬�޺��ߵ�����£� ʹ����Ϊ64��
  }
  
  
 
}




u8 PixelAverage(u8 len, u8 *data) {
  unsigned char i;
  unsigned int sum = 0;
  for(i = 0; i<len; i++) {
    sum = sum + *data++;
  }
  return ((unsigned char)(sum/len));
}


/* �ع�ʱ�䣬��λms */
u8 IntegrationTime = 4;
extern u8 Pixel[128];

    /* 128�����ص��ƽ��ADֵ */
    u8 PixelAverageValue;

void CalculateIntegrationTime(void) 
{


    /* ����128�����ص��ƽ��ADֵ */
    PixelAverageValue = PixelAverage(128,Pixel);
    
    if(PixelAverageValue>140)
      IntegrationTime -= 1;
    if(PixelAverageValue<100)
      IntegrationTime += 1;    
    
    if(IntegrationTime>9)
      IntegrationTime = 9;
    if(IntegrationTime<3)
      IntegrationTime = 3;      
        
    

  /*  
    uart_putchar(UART3,0);   
    uart_putchar(UART3,PixelAverageValue);  
    uart_putchar(UART3,0);
    uart_putchar(UART3,PixelAverageVoltage);
    uart_putchar(UART3,0);
    */
    
    
    
}

