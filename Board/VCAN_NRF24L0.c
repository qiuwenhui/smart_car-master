
#include "common.h"
#include "MK60_port.h"
#include "MK60_gpio.h"
#include "MK60_spi.h"
#include "VCAN_NRF24L0.h"

extern u8 NRF_flag;
extern u8 NRF_rxbuf[32];
// �û����� ���ͺ� ���յ�ַ��Ƶ��

const uint8 TX_ADDRESS[5] = {0x12,0x34,0x56,0x78,0x9a };   // ����һ����̬���͵�ַ
const uint8 RX_ADDRESS[5] = {0x12,0x34,0x56,0x78,0x9a };

uint8 nrf_writereg(uint8 reg, uint8 dat)
{
    uint8 buff[2];
    buff[0] = reg;          //�ȷ��ͼĴ���
    buff[1] = dat;          //�ٷ�������
    spi_mosi(NRF_SPI, NRF_CS, buff, buff, 2); //����buff�����ݣ����ɼ��� buff��
    /*����״̬�Ĵ�����ֵ*/
    return buff[0];
}

uint8 nrf_readreg(uint8 reg, uint8 *dat)
{
    uint8 buff[2];
    buff[0] = reg;          //�ȷ��ͼĴ���
    spi_mosi(NRF_SPI, NRF_CS, buff, buff, 2); //����buff���ݣ������浽buff��
    *dat = buff[1];                         //��ȡ�ڶ�������
    /*����״̬�Ĵ�����ֵ*/
    return buff[0];
}
uint8 nrf_writebuf(uint8 reg , uint8 *pBuf, uint32 len)
{
    spi_mosi_cmd(NRF_SPI, NRF_CS, &reg , NULL, pBuf, NULL, 1 , len); //���� reg ��pBuf ���ݣ�������
    return reg;    //����NRF24L01��״̬
}

uint8 nrf_readbuf(uint8 reg, uint8 *pBuf, uint32 len)
{
    spi_mosi_cmd(NRF_SPI, NRF_CS, &reg , NULL, NULL, pBuf, 1 , len); //����reg�����յ�buff
    return reg;    //����NRF24L01��״̬
}



u8 NRF_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	nrf_writebuf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	nrf_readbuf(TX_ADDR,buf,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//���24L01����	
	return 0;		 //��⵽24L01
}	 	 

void nrf_init(u8 model)
{
    spi_init(NRF_SPI, NRF_CS, MASTER,12500*1000);                     //��ʼ��SPI,����ģʽ
    gpio_init(NRF_CE_PTXn, GPO, LOW);                               //��ʼ��CE��Ĭ�Ͻ������ģʽ
    gpio_init(NRF_IRQ_PTXn, GPI, LOW);                              //��ʼ��IRQ�ܽ�Ϊ����
    //����NRF�Ĵ���
    NRF_CE_LOW();

    nrf_writebuf(NRF_WRITE_REG + RX_ADDR_P0,(u8*)RX_ADDRESS, RX_ADR_WIDTH); //дRX�ڵ��ַ
    nrf_writebuf(NRF_WRITE_REG + TX_ADDR, (u8*)TX_ADDRESS, TX_ADR_WIDTH); //дTX�ڵ��ַ    
    nrf_writereg(NRF_WRITE_REG + EN_AA, 0x01);                      //ʹ��ͨ��0���Զ�Ӧ��
    nrf_writereg(NRF_WRITE_REG + EN_RXADDR, 0x01);                  //ʹ��ͨ��0�Ľ��յ�ַ 
    nrf_writereg(NRF_WRITE_REG + SETUP_RETR, 0x1a);                 //�����Զ��ط����ʱ��:250us + 86us;����Զ��ط�����:15��
    nrf_writereg(NRF_WRITE_REG + RF_CH, 40);                    //����RFͨ��ΪCHANAL
    nrf_writereg(NRF_WRITE_REG + RF_SETUP, 0x0f);                   //����TX�������,0db����,2Mbps,���������濪��

    if(model==RX_MODE)				//RX
    {
            nrf_writereg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);								//ѡ��ͨ��0����Ч���ݿ�� 
            nrf_writereg(NRF_WRITE_REG + CONFIG, 0x0f);   		 //CONFIG 0000 1111 IRQ�շ�����жϿ���,16λCRC,������
    }
    else if(model==TX_MODE)		//TX
    {
            nrf_writereg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);								//ѡ��ͨ��0����Ч���ݿ�� 
            nrf_writereg(NRF_WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����жϿ���,16λCRC,������
    }
    else if(model==RX2_MODE)		//RX2
    {
            nrf_writereg(FLUSH_TX,0xff);
            nrf_writereg(FLUSH_RX,0xff);
            nrf_writereg(NRF_WRITE_REG + CONFIG, 0x0f);   		 // IRQ�շ�����жϿ���,16λCRC,������
            
            nrf_writereg(0x50,0x73);
    
            nrf_writereg(NRF_WRITE_REG+0x1c,0x01); //DYNPD DPL_P0 
            nrf_writereg(NRF_WRITE_REG+0x1d,0x06);	//FEATURE 0000 0110 
    }
    else								//TX2
    {
            nrf_writereg(NRF_WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����жϿ���,16λCRC,������
            nrf_writereg(FLUSH_TX,0xff);
            nrf_writereg(FLUSH_RX,0xff);
            
            nrf_writereg(0x50,0x73);

            nrf_writereg(NRF_WRITE_REG+0x1c,0x01);
            nrf_writereg(NRF_WRITE_REG+0x1d,0x06);
    }
        
        
    NRF_CE_HIGH();

}


u8 NRF_TxPacket(u8 *txbuf)
{
  u8 sta=0;
  u8 rx_len=8;
  
  NRF_CE_LOW();
  
  nrf_writebuf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
  
  NRF_CE_HIGH();//��������	   
  
  while(gpio_get(NRF_IRQ_PTXn)==1);//�ȴ��������
  
  nrf_readreg(0x07,&sta);  //��ȡ״̬�Ĵ�����ֵ	  
  nrf_writereg(NRF_WRITE_REG+0x07,sta); //���TX_DS��MAX_RT�жϱ�־  
  
  NRF_flag=sta;
  
  if(sta& MAX_RT)//�ﵽ����ط�����
  {
    nrf_writereg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
    return MAX_RT; 
  }

  
  if(sta == 46)//0010 1110 �����ж���λ RX FIFO��
  {
          return TX_DS;
          
  }	
  if(sta == 32)//0010 0000 �����ж���λ RX FIFO ͨ��0 
  {
      nrf_readreg(0x60,&rx_len);
      if(rx_len<33)
      {
        // read receive payload from RX_FIFO buffer
        //0110 0001 ��ȡ���ն˷��͵Ĺ������� �������ݴ�FIFO������ɾ���� ����RXģʽ������
        nrf_readbuf(RD_RX_PLOAD,NRF_rxbuf,rx_len);// read receive payload from RX_FIFO buffer
      
      }
      else 
      {
              //1110 0010 ɾ�����ն� FIFO����  ����RXģʽ��
              nrf_writereg(FLUSH_RX,0xff);//��ջ�����
      }
      
      return 0xdd;
  }

    return 0xff;//����ԭ����ʧ��
}


u8 NRF_RxPacket(u8 *rxbuf)
{
  u8 sta;		    							   

  nrf_readreg(0x07,&sta);  //��ȡ״̬�Ĵ�����ֵ    	 
  nrf_writereg(NRF_WRITE_REG+0x07,sta); //���TX_DS��MAX_RT�жϱ�־
  if(sta& RX_DR)//���յ�����
  {
      nrf_readbuf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
      return sta; 
  }
  return sta;//û�յ��κ�����
}


























