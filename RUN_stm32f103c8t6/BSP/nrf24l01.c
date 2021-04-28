
#include "stm32f10x_conf.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "delay.h"

#include "nrf24l01.h"

//NRF24L01寄存器操作命令
// NRF24L01 SPI Instruction Set
// Based on NRF24L01 Mannuel, Page 18
#define NRF24L01_READ_REG        0x00  //读配置寄存器,低5位为寄存器地址
#define NRF24L01_WRITE_REG       0x20  //写配置寄存器,低5位为寄存器地址
#define NRF24L01_RD_RX_PLOAD     0x61  //读RX有效数据,1~32字节
#define NRF24L01_WR_TX_PLOAD     0xA0  //写TX有效数据,1~32字节
#define NRF24L01_FLUSH_TX        0xE1  //清除TX FIFO寄存器.发射模式下用
#define NRF24L01_FLUSH_RX        0xE2  //清除RX FIFO寄存器.接收模式下用
#define NRF24L01_REUSE_TX_PL     0xE3  //重新使用上一包数据,CE为高,数据包被不断发送.
#define NRF24L01_NOP             0xFF  //空操作,可以用来读状态寄存器     

// Memory Map of Register Address 
// Based on NRF24L01 Mannuel, Page 21
#define CONFIG                   0x00  //配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能;
                                       //bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能
#define EN_AA                    0x01  //使能自动应答功能  bit0~5,对应通道0~5
#define EN_RXADDR                0x02  //接收地址允许,bit0~5,对应通道0~5
#define SETUP_AW                 0x03  //设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节;
#define SETUP_RETR               0x04  //建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us
#define RF_CH                    0x05  //RF通道,bit6:0,工作通道频率;
#define RF_SETUP                 0x06  //RF寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益
#define STATUS                   0x07  //状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
                                       //bit5:数据发送完成中断;bit6:接收数据中断;
#define MAX_TX                   0x10  //达到最大发送次数中断
#define TX_OK                    0x20  //TX发送完成中断
#define RX_OK                    0x40  //接收到数据中断

#define OBSERVE_TX               0x08  //发送检测寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
#define CD                       0x09  //载波检测寄存器,bit0,载波检测;
#define RX_ADDR_P0               0x0A  //数据通道0接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P1               0x0B  //数据通道1接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P2               0x0C  //数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P3               0x0D  //数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P4               0x0E  //数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P5               0x0F  //数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define TX_ADDR                  0x10  //发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
#define RX_PW_P0                 0x11  //接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P1                 0x12  //接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P2                 0x13  //接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P3                 0x14  //接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P4                 0x15  //接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P5                 0x16  //接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define FIFO_STATUS              0x17  //FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留
                                       //bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环;

#define SPI_PIN_MOSI             GPIO_Pin_15 
#define SPI_PIN_MISO             GPIO_Pin_14 
#define SPI_PIN_SCL              GPIO_Pin_13 
#define SPI_PIN_CS               GPIO_Pin_4    

#define NRF_PIN_CE               GPIO_Pin_3
#define NRF_GPIO_CE              GPIOB

#define NRF_IRQ_Pin 

#define SPI_GPIO                 GPIOB
#define SPI_GPIO_RCC             RCC_APB2Periph_GPIOB

#define SPI_BASE                 SPI2
#define SPI_RCC                  RCC_APB1Periph_SPI2


#define CS(x)                    GPIO_WriteBit( SPI_GPIO    , SPI_PIN_CS, (BitAction)x)
#define CE(x)                    GPIO_WriteBit( NRF_GPIO_CE , NRF_PIN_CE, (BitAction)x)

uint8_t NRF24L01_TX_Addr[5] = {0};
uint8_t NRF24L01_RX_Addr[5] = {0};


static void __configGPIO(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( SPI_GPIO_RCC | RCC_APB2Periph_AFIO,ENABLE );
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    GPIO_InitStructure.GPIO_Pin     = SPI_PIN_CS;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init( SPI_GPIO, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin     = NRF_PIN_CE;
    GPIO_Init( NRF_GPIO_CE, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin     = SPI_PIN_MOSI | SPI_PIN_MISO | SPI_PIN_SCL;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_Init( SPI_GPIO, &GPIO_InitStructure );

}

static void __configSPI(void){
    SPI_InitTypeDef SPI_InitStructure;    

    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;  // SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;                  // 设置为主SPI
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;                  // SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;                     // 时钟悬空低
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;                   // 数据捕获于第一个时钟沿
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;                     // 内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;          // 八分频 = 9MHz
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;                 // 数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial     = 7;                                // SPI_CRCPolynomial定义了用于CRC值计算的多项式

    SPI_Init( SPI_BASE, &SPI_InitStructure);

    SPI_Cmd( SPI_BASE, ENABLE);
}

static uint8_t __transferByte( uint8_t data ){
    uint8_t retry=0;                     
    while (SPI_I2S_GetFlagStatus( SPI_BASE, SPI_I2S_FLAG_TXE) == RESET){
        retry++;
        if(retry>200)
            return 0;
    }              
    SPI_I2S_SendData( SPI_BASE, data); 
    retry=0;

    while (SPI_I2S_GetFlagStatus( SPI_BASE, SPI_I2S_FLAG_RXNE) == RESET){
        retry++;
        if(retry>200)
            return 0;
    }                                  
    return SPI_I2S_ReceiveData( SPI_BASE ); //返回通过SPIx最近接收的数据 

    // while((SPIx->SR&SPI_I2S_FLAG_TXE)==RESET);   // 等待发送区空      
    // SPIx->DR=Byte;         //发送一个byte   
    // while((SPIx->SR&SPI_I2S_FLAG_RXNE)==RESET);  // 等待接收完一个byte  
    // return SPIx->DR;                             // 返回收到的数据           
}

static uint8_t __readReg(uint8_t reg){
	CS(0);
	__transferByte(reg);
	uint8_t status = __transferByte(0xff);
	CS(1);
	return status;
}

static uint8_t _writeReg(uint8_t reg,uint8_t data){	
	CS(0);
	
	uint8_t status = __transferByte(reg);
	__transferByte(data); 
	
	CS(1);
	return status;
}

static uint8_t __writeBuf(uint8_t reg,uint8_t *p,uint8_t len){	
	CS(0);
	
	uint8_t status = __transferByte( reg );
	for(int i=0; i<len; i++)
		__transferByte(*p++); //Write data
	
	CS(1);
	return status;
}

static uint8_t __readBuf(uint8_t reg,uint8_t *pBuf,uint8_t len){	
	CS(0);
	
	uint8_t status = __transferByte( reg );
	for(int i=0; i<len; i++)
		pBuf[i]=__transferByte(0xff); 
	
	CS(1);
	return status;
}

void NRF24L01_Init(void){
    __configGPIO();
    __configSPI();
    if( !NRF24L01_Check() ){
        //...//
        return;
    }

}

bool NRF24L01_Check(void){
	uint8_t buf[5]={0};
	CE(0);
	__writeBuf  ( NRF24L01_WRITE_REG | TX_ADDR, NRF24L01_TX_Addr, 5 );
	__readBuf   ( NRF24L01_READ_REG  | TX_ADDR, buf             , 5 );
	
	for(int i=0; i<5; i++){
		if(buf[i] != NRF24L01_TX_Addr[i])
			return false;
	}
	return true;
}












