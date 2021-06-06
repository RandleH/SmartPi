
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

#define DATA_LEN                 32

#define SPI_PIN_MOSI             GPIO_Pin_15 
#define SPI_PIN_MISO             GPIO_Pin_14 
#define SPI_PIN_SCL              GPIO_Pin_13 
#define SPI_GPIO                 GPIOB

#define NRF_PIN_CS               GPIO_Pin_4
#define NRF_GPIO_CS              GPIOB  

#define NRF_PIN_CE               GPIO_Pin_3
#define NRF_GPIO_CE              GPIOB

#define NRF_PIN_IRQ              GPIO_Pin_5 
#define NRF_GPIO_IRQ             GPIOB
#define NRF_PORTSRC_IRQ          GPIO_PortSourceGPIOB, GPIO_PinSource5
#define NRF_IRQn                 EXTI9_5_IRQn
#define NRF_LINE                 EXTI_Line5

#define SPI_BASE                 SPI2



#define RCC_ENABLE               do{ RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | \
                                                             RCC_APB2Periph_AFIO  , ENABLE );\
                                     RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2  , ENABLE );\
                                 } while(0)


#define CS(x)                    GPIO_WriteBit( NRF_GPIO_CS , NRF_PIN_CS, (BitAction)x)
#define CE(x)                    GPIO_WriteBit( NRF_GPIO_CE , NRF_PIN_CE, (BitAction)x)



uint8_t NRF24L01_TX_Addr[5] = {0xff,0xff,0xff,0xff,0x7e};
uint8_t NRF24L01_RX_Addr[5] = {0xff,0xff,0xff,0xff,0x7e};

static struct{
    bool    aa;
    uint8_t rfch;
    bool    mode;     // 0 = recv; 1 = send;
    bool    crc;
    uint8_t pipe;

}cache;


static void __configGPIO(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    // RCC_APB2PeriphClockCmd( SPI_GPIO_RCC | RCC_APB2Periph_AFIO,ENABLE );
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    GPIO_InitStructure.GPIO_Pin     = NRF_PIN_CS;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init( NRF_GPIO_CS, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin     = NRF_PIN_CE;
    GPIO_Init( NRF_GPIO_CE, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin     = SPI_PIN_MOSI | SPI_PIN_MISO | SPI_PIN_SCL;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_Init( SPI_GPIO, &GPIO_InitStructure );


    GPIO_InitStructure.GPIO_Pin     = NRF_PIN_IRQ;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
    GPIO_Init( NRF_GPIO_IRQ, &GPIO_InitStructure );
    GPIO_EXTILineConfig( NRF_PORTSRC_IRQ );

}

#include "stm32f10x_exti.h"
#include "misc.h"
static void __configIRQ(void){
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = NRF_IRQn;

    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    EXTI_InitTypeDef EXIT_InitStructure;

    EXIT_InitStructure.EXTI_Line     = NRF_LINE ;

    EXIT_InitStructure.EXTI_Mode     = EXTI_Mode_Interrupt;
    EXIT_InitStructure.EXTI_Trigger  = EXTI_Trigger_Falling;
    EXIT_InitStructure.EXTI_LineCmd  = ENABLE;
    EXTI_Init(&EXIT_InitStructure);
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

static uint8_t __writeReg(uint8_t reg,uint8_t data){    
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

#include "stm32f10x_conf.h"
void NRF24L01_init(void){
    RCC_ENABLE;
    __configGPIO();
    __configIRQ();
    __configSPI();
    if( !NRF24L01_check() ){
        //...//
        assert_param(0);
        return;
    }
}

bool NRF24L01_check(void){
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

void NRF24L01_tx(void){
    CE(0);
    
    __writeBuf( NRF24L01_WRITE_REG|TX_ADDR   , (uint8_t *)NRF24L01_TX_Addr, sizeof(NRF24L01_TX_Addr));  /* 设置主机地址 */
    __writeBuf( NRF24L01_WRITE_REG|RX_ADDR_P0, (uint8_t *)NRF24L01_RX_Addr, sizeof(NRF24L01_RX_Addr));  /* 设置目标地址 发送通道号：0 */
    
    /* 配置发送属性 详见说明书 Page21 */
    __writeReg( NRF24L01_WRITE_REG|EN_AA     , 0x01);   /* Enable ‘Auto Acknowledgment’ Function */
    __writeReg( NRF24L01_WRITE_REG|EN_RXADDR , 0x01);   /* Enabled RX Addresses */
#if 0    
    __writeReg(NRF24L01_WRITE_REG|SETUP_AW, 0x02);      /* Setup of Address Widths */
#endif
    __writeReg( NRF24L01_WRITE_REG|SETUP_RETR, 0x1a);   /* Setup of Automatic Retransmission */    
    __writeReg( NRF24L01_WRITE_REG|RF_CH     , 40  );   /* RF Channel */
    __writeReg( NRF24L01_WRITE_REG|RF_SETUP  , 0x0f);   /* RF Setup Register */
    __writeReg( NRF24L01_WRITE_REG|CONFIG    , 0x0e);   /* TxMode | EN_CRC | 16BIT_CRC | PWR_UP */
    CE(1);
}

void NRF24L01_rx(void){
    CE(0);
    /* 设置接收地址 通道号：0 */
    __writeBuf( NRF24L01_WRITE_REG|RX_ADDR_P0 , (uint8_t*)NRF24L01_RX_Addr,sizeof(NRF24L01_RX_Addr)); // 可以为RX_ADDR_Px
    /* 使能自动应答 通道号：0 */
    __writeReg( NRF24L01_WRITE_REG|EN_AA      , 0x01);
    /* 使能接收地址 通道号：0 */
    __writeReg( NRF24L01_WRITE_REG|EN_RXADDR  , 0x01);
    /* 设置通信频率 */
    __writeReg( NRF24L01_WRITE_REG|RF_CH      , 40);
    /* 设置有效数据宽度 */
    __writeReg( NRF24L01_WRITE_REG|RX_PW_P0   , DATA_LEN);
    /* 设置射频参数 (0db增益,2Mbps,低噪声增益开启) */
    __writeReg( NRF24L01_WRITE_REG|RF_SETUP   , 0x0f);
    /* 基本配置 清除所有标志位，开启CRC效验，POWER UP，PRX模式*/
    __writeReg( NRF24L01_WRITE_REG|CONFIG     , 0x0f);
    CE(1);
}

uint8_t NRF24L01_send( uint8_t *data, uint8_t len ){
    uint16_t cnt = 0xffff;
    uint8_t  sta = 0;

    CE(0);
    __writeBuf( NRF24L01_WR_TX_PLOAD , data, len );
    CE(1);
    while(cnt--){
        sta = __readReg( NRF24L01_READ_REG|STATUS );
        __writeReg( NRF24L01_WRITE_REG|STATUS , sta );

        if( sta & TX_OK ){  
            return TX_OK;
        }

        if( sta & MAX_TX ){
            __writeReg( NRF24L01_FLUSH_TX         , NRF24L01_NOP );
            //...//
            return MAX_TX;
        }
        NRF24L01_delay_ms(1);
    }
    cache.mode = 1;
    return 0xff;
}

uint8_t NRF24L01_recv( uint8_t *buf, uint8_t len ){
    
    uint8_t sta = __readReg( STATUS );

    __writeReg( NRF24L01_WRITE_REG|STATUS , sta ); // 清除TX_DS或MAX_RT中断标志
    if( sta & RX_OK ){
        __readBuf  ( NRF24L01_RD_RX_PLOAD, buf, len );
        __writeReg ( NRF24L01_FLUSH_RX   , 0xff);
        return 0;
    }
    cache.mode = 0;
    return 1;
}

#include <string.h>
void NRF24L01_setTXAddr( uint8_t *addr, uint8_t len ){
    if( len == 5 ){
        memcpy( NRF24L01_TX_Addr, addr, len );
    }else{
        //...//
    }

    if( cache.mode == 1 ) { // send
        NRF24L01_tx();
    }else{
        NRF24L01_tx();
        NRF24L01_rx();
    }
    // CE(0);
    // __writeBuf( NRF24L01_WRITE_REG|TX_ADDR   , (uint8_t *)NRF24L01_TX_Addr, sizeof(NRF24L01_TX_Addr));  /* 设置主机地址 */
    // CE(1);
}

void NRF24L01_setRXAddr( uint8_t pipe, uint8_t *addr, uint8_t len ){
    if( len == 5 ){
        memcpy( NRF24L01_RX_Addr, addr, len );
    }else{
        //...//
    }

    if( pipe > 5 )
        pipe = 5;
    
    
    if( cache.mode == 0 ) { // recv
        NRF24L01_rx();
    }else{
        NRF24L01_rx();
        NRF24L01_tx();
    }


    // CE(0);
    // __writeBuf( NRF24L01_WRITE_REG|(RX_ADDR_P0+pipe) , (uint8_t *)NRF24L01_RX_Addr, sizeof(NRF24L01_RX_Addr));  /* 设置接收地址 */
    // CE(1);
}














