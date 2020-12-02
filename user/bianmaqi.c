
#include "include.h"
#include "bianmaqi.h"
#include "futaba_pid.h"


s16 left_count=0,right_count=0;
s16 left_array[20]={0}, right_array[20]={0};

u16 Target_count = 160;
s16 Target_count_left=0 ,  Target_count_right=0;   
s16 Ave_count=0;

s16 Error_left=0,Error_right=0;
s16 Pre_Error_left=0,Pre_Error_right=0;
s16 Integration_Error_left=0,Integration_Error_right=0;
s16 Feedback_left=0,Feedback_right=0;

s32 left_dianji_pwm=2250,right_dianji_pwm=2250;


s16 speed_P=500;
s16 speed_I=15;
s16 speed_D=500;

extern u8 stop_flag;
extern s16 Track_C_Line_1[10];


 
//λ��ʽPID   ��������ֱ����
 
//extern s16 Track_C_Line_1[10];

void speed()
{
  #define FILTER_NUM 5
  static uint8_t filter_cnt=0;
  s16 temp1=0,temp2=0;
  u8 i=0;

  float duoji_error;
  float count_yz;
  
    s32 P_pwm, I_pwm, D_pwm;
   // ********************FIR�˲�******************* 
    
    left_array[filter_cnt]  = lptmr_pulse_get(); 
    lptmr_pulse_clean();
    
    right_array[filter_cnt]=DMA_count_get(DMA_CH0);  //��
    DMA_count_reset(DMA_CH0);
  
    for(i=0;i<FILTER_NUM;i++)
    {
         temp1 += left_array[i];
         temp2 += right_array[i];
    }
    left_count = temp1 / FILTER_NUM;
    right_count = temp2 / FILTER_NUM;
    
    filter_cnt++;
    if(filter_cnt>=FILTER_NUM)	filter_cnt=0;
        
  ///////
  if(left_count<0)  
    left_count=-left_count;
  if(right_count<0)  
    right_count=-right_count;  


 /////
  if(!stop_flag)
  {

 /*      
       //����1.12  ����0.87
      if(duoji_pwm>1540 && duoji_pwm<=1700)
      {
          Target_count_right=(s16)(Target_count*(((float)(duoji_pwm-1520)/1500)+1));
          Target_count_left =(s16)(Target_count*((1-(float)(duoji_pwm-1520)/1500)));
          
      }
      //�Ҵ�� ���ּ���
      if(duoji_pwm<1500 && duoji_pwm>=1350)
      {
          Target_count_right=(s16)(Target_count*((1-(float)(1520-duoji_pwm)/1500)));
          Target_count_left =(s16)(Target_count*(((float)(1520-duoji_pwm)/1500)+1));
      }*/
       
   
      duoji_error = (float) duoji_pwm-1490 ;
      
      if(duoji_error<0)
        duoji_error=-duoji_error;
      
      if(duoji_error < 80 )
      {
        count_yz=1;
      }
      else 
      {
   //     count_yz = (-0.0035) * duoji_error + 1.108;
    count_yz = (-0.0028) * duoji_error + 1.108;    // ����� ���ٵ� 0.7
   //      count_yz = (-0.0021) * duoji_error + 1.108;    // ����� ���ٵ� 0.814
        
      }



      
       // 1350 1490 1630
       //���棬 ���ּ���                  1490 1530 1630 
      if(duoji_pwm>1530 && duoji_pwm<=1630)
      {
          Target_count_left = (s16)(Target_count*((1-(float)(duoji_pwm-1490)/500)) * count_yz);  // ���ֲ���0.72  ����ӦΪ0.77
          Target_count_right = Target_count  * count_yz;
        
     //   Target_count_left = Target_count * count_yz ;
      //   Target_count_right = Target_count * count_yz ;
         
  //        Target_count_left = (s16)(Target_count*((1-(float)(duoji_pwm-1490)/780)) );
  //        Target_count_right = Target_count ;          
      
      }
      //�Ҵ�� ���ּ���  0.77     1350 1450 1490 
      else if(duoji_pwm<1450 && duoji_pwm>=1350)
      {

   //             Target_count_left = Target_count * count_yz ;
    //     Target_count_right = Target_count * count_yz ;
         
          Target_count_right=(s16)(Target_count*((1-(float)(1490-duoji_pwm)/500))  * count_yz);
          Target_count_left = Target_count  * count_yz;
          
    //      Target_count_right=(s16)(Target_count*((1-(float)(1490-duoji_pwm)/780)) );
    //      Target_count_left = Target_count ;
      }       
      /////////
      else
      {
          Target_count_right = Target_count ; 
          Target_count_left = Target_count ;     
      }
        
    

      if(Target_count_right>200)  
        Target_count_right=200;
      if(Target_count_left>200)  
        Target_count_left=200;     
    

      // ********************����PID����*******************
        Pre_Error_left = Error_left;
        Error_left = Target_count_left-left_count;
        Integration_Error_left += Error_left;

        P_pwm = speed_P * Error_left                    / 10; 
        I_pwm = speed_I * Integration_Error_left        / 10;
        D_pwm = speed_D * (Error_left-Pre_Error_left)   / 10;          
        
        
        if(Integration_Error_left>5000)  
          Integration_Error_left=5000;
         if(Integration_Error_left<-5000)  
          Integration_Error_left=-5000;      
  
         if(P_pwm>5000)  
          P_pwm=5000;               
         if(I_pwm>3000)  
          I_pwm=3000;     
         
        left_dianji_pwm = P_pwm + I_pwm + D_pwm;       
        
        
      if(left_dianji_pwm>8000)  
        left_dianji_pwm=8000;   
      if(left_dianji_pwm<1000)  
        left_dianji_pwm=1000;         
    
      
      // *******************�ҵ��PID����********************
        Pre_Error_right=Error_right;
        Error_right=Target_count_right-right_count;
        Integration_Error_right += Error_right;
        
        P_pwm = speed_P * Error_right                    / 10; 
        I_pwm = speed_I * Integration_Error_right        / 10;
        D_pwm = speed_D * (Error_right-Pre_Error_right)   / 10;   
        
        
        if(Integration_Error_right>5000)  
          Integration_Error_right=5000;
        if(Integration_Error_right<-5000)  
          Integration_Error_right=-5000;        
         if(P_pwm>5000)  
          P_pwm=5000;               
         if(I_pwm>3000)  
          I_pwm=3000;          

        right_dianji_pwm = P_pwm + I_pwm + D_pwm;    

      if(right_dianji_pwm>8000)  
        right_dianji_pwm=8000;   
      if(right_dianji_pwm<1000)  
        right_dianji_pwm=1000;       

  }
  else
  {
      left_dianji_pwm=0;
      right_dianji_pwm=0;      
      Integration_Error_right=0;
      Integration_Error_left=0;
  }
  

  FTM_PWM_Duty(FTM0, FTM_CH5,right_dianji_pwm );   //����       
  FTM_PWM_Duty(FTM0, FTM_CH6,left_dianji_pwm );   //�ҵ��        

  
   //  FTM_PWM_Duty(FTM0, FTM_CH4,left_dianji_pwm );  //���� 
  
 //   FTM_PWM_Duty(FTM0, FTM_CH5,3000 );    //�ҵ��  ��ת
  //  FTM_PWM_Duty(FTM0, FTM_CH6,3000 );    //��
  
  
}

//�����ת��ƽ��ֵ PID���� �������ͬPWMֵ
 /*
void speed()
{
  #define FILTER_NUM 5
  static uint8_t filter_cnt=0;
  s16 temp1=0,temp2=0;
  u8 i=0;

  s32 P_pwm, I_pwm, D_pwm;
  
   // ********************FIR�˲�******************* 
    
    left_array[filter_cnt]  = lptmr_pulse_get(); 
    lptmr_pulse_clean();
    
    right_array[filter_cnt]=DMA_count_get(DMA_CH0);  //��
    DMA_count_reset(DMA_CH0);
  
    for(i=0;i<FILTER_NUM;i++)
    {
         temp1 += left_array[i];
         temp2 += right_array[i];
    }
    left_count = temp1 / FILTER_NUM;
    right_count = temp2 / FILTER_NUM;
    
    filter_cnt++;
    if(filter_cnt>=FILTER_NUM)	filter_cnt=0;
        

  if(left_count<0)  
    left_count=-left_count;
  if(right_count<0)  
    right_count=-right_count;  

  Ave_count = ( left_count + right_count ) >> 1;
  
  if(!stop_flag)
  {

//    if(Ave_count > Target_count - 7  && Ave_count <= Target_count)
//    {
    
//    }
//    else
//    {
      // ********************����PID����*******************
      
        Pre_Error_left = Error_left;
        Error_left = Target_count - Ave_count;
        Integration_Error_left += Error_left;
        
        P_pwm = speed_P * Error_left                    / 10; 
        I_pwm = speed_I * Integration_Error_left        / 10;
        D_pwm = speed_D * (Error_left-Pre_Error_left)   / 10;
        
         if(Integration_Error_left>5000)  
          Integration_Error_left=5000;        
         if(Integration_Error_left<-5000)  
          Integration_Error_left=-5000;    
         
         if(P_pwm>5000)  
          P_pwm=5000;               
         if(I_pwm>3000)  
          I_pwm=3000;        
      
      left_dianji_pwm = P_pwm + I_pwm + D_pwm;
                        
      
      if(left_dianji_pwm>8000)  
        left_dianji_pwm=8000;   
      if(left_dianji_pwm<1000)  
        left_dianji_pwm=1000;       
      
 //   }

  }
  else
  {
      left_dianji_pwm=0;
      Integration_Error_left=0;
  }

  FTM_PWM_Duty(FTM0, FTM_CH5,left_dianji_pwm );   //����         PWM 20%
  FTM_PWM_Duty(FTM0, FTM_CH6,left_dianji_pwm );   //�ҵ��         PWM 20%


}*/




//bang-bang���� Ч������
/*
void speed()
{
  #define FILTER_NUM 5
  static uint8_t filter_cnt=0;
  s16 temp1=0,temp2=0;
  u8 i=0;
  
   // ********************FIR�˲�******************* 
    
    left_array[filter_cnt]  = lptmr_pulse_get(); 
    lptmr_pulse_clean();
    
    right_array[filter_cnt]=DMA_count_get(DMA_CH0);  //��
    DMA_count_reset(DMA_CH0);

    for(i=0;i<FILTER_NUM;i++)
    {
         temp1 += left_array[i];
         temp2 += right_array[i];
    }
    left_count = temp1 / FILTER_NUM;
    right_count = temp2 / FILTER_NUM;
    
    filter_cnt++;
    if(filter_cnt>=FILTER_NUM)	filter_cnt=0;
        

#define PWM_HIGH 5000
#define PWM_LOW  0
  
  if(Enable)
  {
    if(left_count<100)
      left_dianji_pwm = PWM_HIGH;
    else
      left_dianji_pwm = PWM_LOW;
    
    if(right_count<100)
      right_dianji_pwm = PWM_HIGH;    
    else
      right_dianji_pwm = PWM_LOW;    
  }
  else
  {    
      left_dianji_pwm = 0;
      right_dianji_pwm = 0;            
          
  }
  
  if(stop_flag)  
  {
//    left_dianji_pwm=0;
  }

  FTM_PWM_Duty(FTM0, FTM_CH5,left_dianji_pwm );   //����         PWM 20%
  FTM_PWM_Duty(FTM0, FTM_CH6,right_dianji_pwm );   //�ҵ��         PWM 20%
 
  
}
*/



//�ٶȲ� λ��PID ����
/*
void speed()
{
  int32 out_p,out_i,out_d,
    
  left_count2=left_count1;
  right_count2=right_count1;  
  left_count1=left_count0;
  right_count1=right_count0;

  left_count0=FTM_QUAD_get(FTM2);
  FTM_QUAD_clean(FTM2);
  right_count0=FTM_QUAD_get(FTM1);   //FTM1 �ҵ��
  FTM_QUAD_clean(FTM1);
   
  left_count=(left_count0+left_count1+left_count2)/3;
  right_count=(right_count0+right_count1+right_count2)/3;
  
  if(left_count<0)  
    left_count=-left_count;
  if(right_count<0)  
    right_count=-right_count;  
   
  


    if(Enable)
    {
       
     
        // *************************�����ٶȲ� ����PID**************
          S_Error_left = Target_count_left - left_count;       //e(k)
          S_DError_left = S_Error_left - S_Pre_Error_left;      //e(k)-e(k-1)
          S_DDError_left = S_DError_left - S_Pre_DError_left;   //e(k)-e(k-1)-(e(k-1)-e(k-2))
          
          S_Pre_Error_left = S_Error_left;                                     //�洢��ǰƫ��
          S_Pre_DError_left = S_DError_left;                                   //�洢��ǰƫ�����ϴ�ƫ��֮��
          if((S_Error_left < Speed_Dead) && (S_Error_left > -Speed_Dead))     //���õ�������
          {
             Feedback_left = 0;              
          }
          
          else
          {
            out_p = S_Kp * S_DError_left;   //������
            out_i = S_Ki * S_Error_left;    //������
            out_d = S_Kd * S_DDError_left;  //΢����
            Feedback_left = (int32)((out_p + out_i + out_d)/100.0);
          }
          left_dianji_pwm = left_dianji_pwm + Feedback_left;
                
        // *************************�ҵ���ٶȲ� ����PID**************
          S_Error_right = Target_count_right - left_count;       //e(k)
          S_DError_right = S_Error_right - S_Pre_Error_right;      //e(k)-e(k-1)
          S_DDError_right = S_DError_right - S_Pre_DError_right;   //e(k)-e(k-1)-(e(k-1)-e(k-2))
          
          S_Pre_Error_right = S_Error_right;                                     //�洢��ǰƫ��
          S_Pre_DError_right = S_DError_right;                                   //�洢��ǰƫ�����ϴ�ƫ��֮��
          if((S_Error_right < Speed_Dead) && (S_Error_right > -Speed_Dead))     //���õ�������
          {
             Feedback_right = 0;              
          }
          
          else
          {
            out_p = S_Kp * S_DError_right;   //������
            out_i = S_Ki * S_Error_right;    //������
            out_d = S_Kd * S_DDError_right;  //΢����
            Feedback_right = (int32)((out_p + out_i + out_d)/100.0);
          }

            right_dianji_pwm=right_dianji_pwm+Feedback_right;

          
              if(left_dianji_pwm>5000)  
                left_dianji_pwm=5000;
              if(right_dianji_pwm>5000)  
                right_dianji_pwm=5000;      
     }
    else
    {    
        left_dianji_pwm=0;
        right_dianji_pwm=0;      
    }   
    
    FTM_PWM_Duty(FTM0, FTM_CH4,left_dianji_pwm );   //����
    FTM_PWM_Duty(FTM0, FTM_CH6,right_dianji_pwm);    
   

}

*/





  
//����PID
   /*
void speed()
{
  #define FILTER_NUM 5
  static uint8_t filter_cnt=0;
  s16 temp1=0,temp2=0;
  u8 i=0;
   // ********************FIR�˲�******************* 
    
    left_array[filter_cnt]  = lptmr_pulse_get(); 
    lptmr_pulse_clean();
    
    right_array[filter_cnt]=DMA_count_get(DMA_CH0);  //��
    DMA_count_reset(DMA_CH0);
  
    for(i=0;i<FILTER_NUM;i++)
    {
         temp1 += left_array[i];
         temp2 += right_array[i];
    }
    left_count = temp1 / FILTER_NUM;
    right_count = temp2 / FILTER_NUM;
    
    filter_cnt++;
    if(filter_cnt>=FILTER_NUM)	filter_cnt=0;
        

  if(left_count<0)  
    left_count=-left_count;
  if(right_count<0)  
    right_count=-right_count;  


  if(!stop_flag)
  {

      // ********************����PID����*******************
        Target_count_left = Target_count;
      
        Pre_Error_left=Error_left;
        Error_left=Target_count_left-left_count;
        Integration_Error_left += Error_left;
                
        if(Integration_Error_left>1000)  
          Integration_Error_left=1000;
        
      Feedback_left=  speed_P * Error_left
                    + speed_I * Integration_Error_left
                    + speed_D * (Error_left-Pre_Error_left);
      
      Feedback_left /= 10;
      
      if(Feedback_left>500)  
        Feedback_left=500;   
      if(Feedback_left<-200)  
        Feedback_left=-200;       
      
      // *******************�ҵ��PID����********************
      Target_count_right = Target_count;
      
        Pre_Error_right=Error_right;
        Error_right=Target_count_right-right_count;
        Integration_Error_right += Error_right;
        
        if(right_count > Target_count )
            Integration_Error_right = 0;
        
        if(Integration_Error_right>1000)  
          Integration_Error_right=1000;
          
        Feedback_right=  speed_P * Error_right
                       + speed_I * Integration_Error_right
                       + speed_D * (Error_right-Pre_Error_right);
      
        Feedback_right /= 10;

        if(Feedback_right>500)  
          Feedback_right=500;  
        if(Feedback_right<-200)  
          Feedback_right=-200;        
        
      // *******************��������********************
        left_dianji_pwm  = left_dianji_pwm  + Feedback_left;
        right_dianji_pwm = right_dianji_pwm + Feedback_right;

      if(left_dianji_pwm>7000)  
        left_dianji_pwm=7000;
      if(left_dianji_pwm<0)  
        left_dianji_pwm=0;
      
      if(right_dianji_pwm>7000)  
        right_dianji_pwm=7000;      
      if(right_dianji_pwm<0)  
        right_dianji_pwm=0;
    }

  
  else
  {
      left_dianji_pwm=0;
      right_dianji_pwm=0;      
      Feedback_left=0;
      Feedback_right=0;
      Integration_Error_right=0;
      Integration_Error_left=0;
  }

  FTM_PWM_Duty(FTM0, FTM_CH5,left_dianji_pwm );   //����         PWM 20%
  FTM_PWM_Duty(FTM0, FTM_CH6,right_dianji_pwm );   //�ҵ��         PWM 20%

}*/


//�������PID  �����ת����ƽ���� �������ͬPWMֵ
/*
void speed()
{
  #define FILTER_NUM 5
  static uint8_t filter_cnt=0;
  s16 temp1=0,temp2=0;
  u8 i=0;
   // ********************FIR�˲�******************* 
    
    left_array[filter_cnt]  = lptmr_pulse_get(); 
    lptmr_pulse_clean();
    
    right_array[filter_cnt]=DMA_count_get(DMA_CH0);  //��
    DMA_count_reset(DMA_CH0);
  
    for(i=0;i<FILTER_NUM;i++)
    {
         temp1 += left_array[i];
         temp2 += right_array[i];
    }
    left_count = temp1 / FILTER_NUM;
    right_count = temp2 / FILTER_NUM;
    
    filter_cnt++;
    if(filter_cnt>=FILTER_NUM)	filter_cnt=0;
        

  if(left_count<0)  
    left_count=-left_count;
  if(right_count<0)  
    right_count=-right_count;  

    Ave_count = ( left_count + right_count ) >> 1;

  if(!stop_flag)
  {

    
      // ********************����PID����*******************
      
        Pre_Error_left=Error_left;
        Error_left=Target_count-Ave_count;
        Integration_Error_left += Error_left;
                
        if(Integration_Error_left>2000)  
          Integration_Error_left=2000;
        
      Feedback_left=  speed_P * Error_left
                    + speed_I * Integration_Error_left
                    + speed_D * (Error_left-Pre_Error_left);
      
      Feedback_left /= 10;
      
      if(Feedback_left>2000)  
        Feedback_left=2000;   
      if(Feedback_left<-2000)  
        Feedback_left=-2000;       
      
      // *******************��������********************
        left_dianji_pwm  = left_dianji_pwm  + Feedback_left;


      if(left_dianji_pwm>8000)  
        left_dianji_pwm=8000;
      if(left_dianji_pwm<0)  
        left_dianji_pwm=0;

    }

  
  else
  {
      left_dianji_pwm=0;
 
      Feedback_left=0;

      Integration_Error_left=0;
  }

  FTM_PWM_Duty(FTM0, FTM_CH5,left_dianji_pwm );   //����         PWM 20%
  FTM_PWM_Duty(FTM0, FTM_CH6,left_dianji_pwm );   //�ҵ��         PWM 20%

}*/

 u16 NRF_pwm;
 
 //����
 /*
void speed()
{
  #define FILTER_NUM 5
  static uint8_t filter_cnt=0;
  s16 temp1=0,temp2=0;
  u8 i=0;

  
   // ********************FIR�˲�******************* 
    
    left_array[filter_cnt]  = lptmr_pulse_get(); 
    lptmr_pulse_clean();
    
    right_array[filter_cnt]=DMA_count_get(DMA_CH0);  //��
    DMA_count_reset(DMA_CH0);
  
    for(i=0;i<FILTER_NUM;i++)
    {
         temp1 += left_array[i];
         temp2 += right_array[i];
    }
    left_count = temp1 / FILTER_NUM;
    right_count = temp2 / FILTER_NUM;
    
    filter_cnt++;
    if(filter_cnt>=FILTER_NUM)	filter_cnt=0;
        

  if(left_count<0)  
    left_count=-left_count;
  if(right_count<0)  
    right_count=-right_count;  
  
  if(!stop_flag)  
  {
    FTM_PWM_Duty(FTM0, FTM_CH5,NRF_pwm );   //����         PWM 20%
    FTM_PWM_Duty(FTM0, FTM_CH6,NRF_pwm );   //�ҵ��         PWM 20%      
  }
  else
  {
    FTM_PWM_Duty(FTM0, FTM_CH5,0 );   //����       
    FTM_PWM_Duty(FTM0, FTM_CH6,0 );   //�ҵ��    
  
  }


}*/

 





