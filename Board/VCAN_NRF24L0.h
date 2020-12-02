
#ifndef _VCAN_NRF24L0_H_
#define _VCAN_NRF24L0_H_   

//extern const uint8 TX_ADDRESS[5];  
//extern const uint8 RX_ADDRESS[5];

//������Ӳ������
#define NRF_SPI         SPI1
#define NRF_CS          SPIn_PCS0

#define NRF_CE_PTXn     PTE12
#define NRF_IRQ_PTXn    PTE11

//gpio����CE��IRQ
#define NRF_CE_HIGH()       GPIO_SET(NRF_CE_PTXn,1)
#define NRF_CE_LOW()        GPIO_SET(NRF_CE_PTXn,0)           //CE�õ�

// �ڲ����ò���
#define TX_ADR_WIDTH    5                       //�����ַ���
#define RX_ADR_WIDTH    5                       //���յ�ַ���
#define TX_PLOAD_WIDTH  32                     //��������ͨ����Ч���ݿ��0~32Byte
#define RX_PLOAD_WIDTH  32                     //��������ͨ����Ч���ݿ��0~32Byte


#define CHANAL          40                              //Ƶ��ѡ��


/******************************** NRF24L01+ �Ĵ������� �궨��***************************************/

// SPI(nRF24L01) commands , NRF��SPI����궨�壬���NRF����ʹ���ĵ�
#define NRF_READ_REG    0x00    // Define read command to register
#define NRF_WRITE_REG   0x20    // Define write command to register
#define RD_RX_PLOAD     0x61    // Define RX payload register address
#define WR_TX_PLOAD     0xA0    // Define TX payload register address
#define FLUSH_TX        0xE1    // Define flush TX register command
#define FLUSH_RX        0xE2    // Define flush RX register command
#define REUSE_TX_PL     0xE3    // Define reuse TX payload register command
#define NOP             0xFF    // Define No Operation, might be used to read status register

// SPI(nRF24L01) registers(addresses) ��NRF24L01 ��ؼĴ�����ַ�ĺ궨��
#define CONFIG      0x00        // 'Config' register address
#define EN_AA       0x01        // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR   0x02        // 'Enabled RX addresses' register address
#define SETUP_AW    0x03        // 'Setup address width' register address
#define SETUP_RETR  0x04        // 'Setup Auto. Retrans' register address
#define RF_CH       0x05        // 'RF channel' register address
#define RF_SETUP    0x06        // 'RF setup' register address
#define STATUS      0x07        // 'Status' register address
#define OBSERVE_TX  0x08        // 'Observe TX' register address
#define CD          0x09        // 'Carrier Detect' register address
#define RX_ADDR_P0  0x0A        // 'RX address pipe0' register address
#define RX_ADDR_P1  0x0B        // 'RX address pipe1' register address
#define RX_ADDR_P2  0x0C        // 'RX address pipe2' register address
#define RX_ADDR_P3  0x0D        // 'RX address pipe3' register address
#define RX_ADDR_P4  0x0E        // 'RX address pipe4' register address
#define RX_ADDR_P5  0x0F        // 'RX address pipe5' register address
#define TX_ADDR     0x10        // 'TX address' register address
#define RX_PW_P0    0x11        // 'RX payload width, pipe0' register address
#define RX_PW_P1    0x12        // 'RX payload width, pipe1' register address
#define RX_PW_P2    0x13        // 'RX payload width, pipe2' register address
#define RX_PW_P3    0x14        // 'RX payload width, pipe3' register address
#define RX_PW_P4    0x15        // 'RX payload width, pipe4' register address
#define RX_PW_P5    0x16        // 'RX payload width, pipe5' register address
#define FIFO_STATUS 0x17        // 'FIFO Status Register' register address


//������Ҫ��״̬���
#define TX_FULL     0x01        //TX FIFO �Ĵ�������־�� 1 Ϊ ����0Ϊ ����
#define MAX_RT      0x10        //�ﵽ����ط������жϱ�־λ
#define TX_DS       0x20        //��������жϱ�־λ
#define RX_DR       0x40        //���յ������жϱ�־λ


#define RX_MODE				1			//��ͨ����
#define TX_MODE				2			//��ͨ����
#define RX2_MODE			3			//����ģʽ2,����˫����
#define TX2_MODE			4			//����ģʽ2,����˫����

//��������
void   nrf_init(u8 );              //��ʼ��NRF24L01+
u8 NRF_Check(void);
u8 NRF_TxPacket(u8 *);
u8 NRF_RxPacket(u8 *);

#endif      //_VCAN_NRF24L0_H_












