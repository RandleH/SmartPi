

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

#pragma anon_unions

typedef union {
    struct{
        uint8_t _PRIM_RX      : 1;  // 1: PRX, 0: PTX
        uint8_t _PWR_UP       : 1;  // 1: POWER UP , 0:POWER DOWN
        uint8_t _CRCO         : 1;  // 1: 2byte, 0: 1byte   CRC encoding scheme
        uint8_t _EN_CRC       : 1;  // Enable CRC. Forced high if one of the bits in the EN_AA is high
        uint8_t _MASK_MAX_RT  : 1;  // 1: Interrupt not reflected on the IRQ pin 0: Reflect MAX_RT as active low interrupt on the IRQ pin
        uint8_t _MASK_TX_DS   : 1;  // 1: Interrupt not reflected on the IRQ pin 0: Reflect TX_DS as active low interrupt on the IRQ pin
        uint8_t _MASK_RX_DR   : 1;  // 1: Interrupt not reflected on the IRQ pin 0: Reflect RX_DR as active low interrupt on the IRQ pin
        uint8_t               : 1;
    };
    uint8_t val;
}REG_CONFIG_t;

typedef union{
    struct{
        uint8_t _RF_CH : 6;
        uint8_t        : 1;
    };
    uint8_t val;
}REG_RFCH_t;

typedef union{
    struct{
        uint8_t _LNA_HCURR  : 1;
        uint8_t _RF_PWR     : 2;
        uint8_t _RF_DR      : 1;
        uint8_t _PLL_LOCK   : 1;
        uint8_t             : 3;
    };
    uint8_t val;
}REG_RFSETUP_t;


static uint8_t cache[32] = {0};

S_NRF24L01_CFG_t G_NRF24L01_Config;


static void __configGPIO(void){
    GPIO_InitTypeDef GPIO_InitStructure;
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
static void    __configIRQ     (void){
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

static void    __configSPI     (void){
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

static uint8_t __transferByte  (uint8_t data ){
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

static uint8_t __readReg       (uint8_t reg){
    CS(0);
    __transferByte(reg);
    uint8_t status = __transferByte(0xff);
    CS(1);
    return status;
}

static uint8_t __writeReg      (uint8_t reg,uint8_t data){
    CS(0);
    
    uint8_t status = __transferByte(reg);
    __transferByte(data); 
    
    CS(1);
    return status;
}

static uint8_t __writeBuf      (uint8_t reg,const uint8_t *p,uint8_t len){
    CS(0);
    
    uint8_t status = __transferByte( reg );
    for(int i=0; i<len; i++)
        __transferByte(*p++); //Write data
    
    CS(1);
    return status;
}

static uint8_t __readBuf       (uint8_t reg, uint8_t *pBuf,uint8_t len){
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
    if( !NRF24L01_exist() ){
        //...//
        NRF24L01_ASSERT(0);
        return;
    }

    G_NRF24L01_Config.autoACK = true;
    G_NRF24L01_Config.crc     = true;
    G_NRF24L01_Config.gainLNA = true;  
    G_NRF24L01_Config.lockPLL = false; 
    G_NRF24L01_Config.pipe    = NRF24L01_PIPE_P0;
    G_NRF24L01_Config.rate    = NRF24L01_RATE_2Mbps;
    G_NRF24L01_Config.freq    = NRF24L01_FREQ_2440MHz;
    G_NRF24L01_Config.crco    = NRF24L01_CRCO_2Byte;
    G_NRF24L01_Config.pwr     = NRF24L01_PWR_0dBm;
    
    G_NRF24L01_Config.addr.tx[0] = G_NRF24L01_Config.addr.rx_p0[0] = G_NRF24L01_Config.addr.rx_p1[0] = 0xff;
    G_NRF24L01_Config.addr.tx[1] = G_NRF24L01_Config.addr.rx_p0[1] = G_NRF24L01_Config.addr.rx_p1[1] = 0xff;
    G_NRF24L01_Config.addr.tx[2] = G_NRF24L01_Config.addr.rx_p0[2] = G_NRF24L01_Config.addr.rx_p1[2] = 0xff;
    G_NRF24L01_Config.addr.tx[3] = G_NRF24L01_Config.addr.rx_p0[3] = G_NRF24L01_Config.addr.rx_p1[3] = 0xff;
    G_NRF24L01_Config.addr.tx[4] = G_NRF24L01_Config.addr.rx_p0[4] = G_NRF24L01_Config.addr.rx_p1[4] = 0x7e;
    
    G_NRF24L01_Config.data.buf = cache;
    G_NRF24L01_Config.data.len = sizeof(cache);

    NRF24L01_wtCfg( &G_NRF24L01_Config );
 
}

bool NRF24L01_exist(void){
    uint8_t buf[5]={0};
    uint8_t tmp[5]={0};
    
    CE(0);
    __readBuf( NRF24L01_READ_REG | TX_ADDR, buf    , 5 );


    __writeBuf  ( NRF24L01_WRITE_REG | TX_ADDR, buf, 5 );
    __readBuf   ( NRF24L01_READ_REG  | TX_ADDR, tmp, 5 );
    
    for(int i=0; i<5; i++){
        if(buf[i] != buf[i])
            return false;
    }
    CE(1);
    return true;
}

bool NRF24L01_hasMessage(void){
    return (bool)( RX_OK & __readReg( STATUS ) );
}

void NRF24L01_tx( const S_NRF24L01_CFG_t  *p ){
    CE(0);
#if 0 
    __writeBuf( NRF24L01_WRITE_REG|TX_ADDR   , (uint8_t *)NRF24L01_TX_Addr, sizeof(NRF24L01_TX_Addr));  /* 设置主机地址 */
    __writeBuf( NRF24L01_WRITE_REG|RX_ADDR_P0, (uint8_t *)NRF24L01_RX_Addr, sizeof(NRF24L01_RX_Addr));  /* 设置目标地址 发送通道号：0 */
    
    /* 配置发送属性 详见说明书 Page21 */
    __writeReg( NRF24L01_WRITE_REG|EN_AA     , 0x01);   /* Enable ‘Auto Acknowledgment’ Function */
    __writeReg( NRF24L01_WRITE_REG|EN_RXADDR , 0x01);   /* Enabled RX Addresses */

    __writeReg(NRF24L01_WRITE_REG|SETUP_AW, 0x02);      /* Setup of Address Widths */

    __writeReg( NRF24L01_WRITE_REG|SETUP_RETR, 0x1a);   /* Setup of Automatic Retransmission */    
    __writeReg( NRF24L01_WRITE_REG|RF_CH     , 40  );   /* RF Channel */
    __writeReg( NRF24L01_WRITE_REG|RF_SETUP  , 0x0f);   /* RF Setup Register */
    __writeReg( NRF24L01_WRITE_REG|CONFIG    , 0x0e);   /* TxMode | EN_CRC | 16BIT_CRC | PWR_UP */
#endif
    if( p==NULL ){
        p = &G_NRF24L01_Config; 
    }

    NRF24L01_wtCfg( p );

    if( p->autoACK ){
        __writeBuf( NRF24L01_WRITE_REG|RX_ADDR_P0, p->addr.tx, sizeof(p->addr.rx_p0) ); 
        __writeReg( NRF24L01_WRITE_REG|RX_PW_P0  , sizeof(p->addr.rx_p0)   );
    }

    REG_CONFIG_t config = {0};
    
    config.val = __readReg( NRF24L01_READ_REG|CONFIG );
    
    config._PRIM_RX = 0;
    config._PWR_UP  = 1;

    __writeReg( NRF24L01_WRITE_REG|CONFIG, config.val );

    NRF24L01_DELAY_MS(2);
    CE(1);
    NRF24L01_DELAY_US(130);
}

void NRF24L01_rx( const S_NRF24L01_CFG_t  *p ){
    CE(0);

    if( p==NULL ){
        p = &G_NRF24L01_Config; 
    }

    NRF24L01_wtCfg( p );

    REG_CONFIG_t config = {0};
    
    config.val = __readReg( NRF24L01_READ_REG|CONFIG );
    
    config._PRIM_RX = 1;
    config._PWR_UP  = 1;

    __writeReg( NRF24L01_WRITE_REG|CONFIG     , config.val );
    
    NRF24L01_DELAY_MS(2);
    CE(1);
    NRF24L01_DELAY_US(130);
}

void NRF24L01_pd( const S_NRF24L01_CFG_t  *p ){
    CE(0);

    if( p==NULL ){
        p = &G_NRF24L01_Config;
    }

    NRF24L01_wtCfg( p );

    REG_CONFIG_t config = {0};
    
    config.val = __readReg( NRF24L01_READ_REG|CONFIG );
    
    config._PRIM_RX = 1;
    config._PWR_UP  = 0;

    __writeReg( NRF24L01_WRITE_REG|CONFIG     , config.val );
    
    NRF24L01_DELAY_MS(2);

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
        NRF24L01_DELAY_MS(1);
    }
    //...//
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
    //...//
    return 1;
}

#include <string.h>
void NRF24L01_setTXAddr( uint8_t *addr, uint8_t len ){
    NRF24L01_ASSERT( len <= 5 );
    NRF24L01_ASSERT( addr );

    memcpy( G_NRF24L01_Config.addr.tx, addr, len );

    CE(0);
    __writeBuf( NRF24L01_WRITE_REG|TX_ADDR, G_NRF24L01_Config.addr.tx, sizeof(G_NRF24L01_Config.addr.tx) ); 
    CE(1);
}

void NRF24L01_setRXAddr( uint8_t pipe, uint8_t *addr, uint8_t len ){
    
    NRF24L01_ASSERT( len <= 5 );
    NRF24L01_ASSERT( addr );
    NRF24L01_ASSERT( pipe <= NRF24L01_PIPE_P5 );
    NRF24L01_ASSERT( pipe >= NRF24L01_PIPE_P2 && len!=1 );
    
    CE(0);
    switch( pipe ){
        case NRF24L01_PIPE_P0: memcpy( G_NRF24L01_Config.addr.rx_p0 , addr, len );
                               __writeBuf( NRF24L01_WRITE_REG|RX_ADDR_P0, G_NRF24L01_Config.addr.rx_p0, sizeof(G_NRF24L01_Config.addr.rx_p0) ); 
                               __writeReg( NRF24L01_WRITE_REG|RX_PW_P0  , G_NRF24L01_Config.data.len   ); 
                               break;
        case NRF24L01_PIPE_P1: memcpy( G_NRF24L01_Config.addr.rx_p1 , addr, len );
                               __writeBuf( NRF24L01_WRITE_REG|RX_ADDR_P1, G_NRF24L01_Config.addr.rx_p1, sizeof(G_NRF24L01_Config.addr.rx_p1) ); 
                               __writeReg( NRF24L01_WRITE_REG|RX_PW_P1  , G_NRF24L01_Config.data.len   ); 
                               break;
        case NRF24L01_PIPE_P2: memcpy( &G_NRF24L01_Config.addr.rx_p2 , addr, len );
                               __writeReg( NRF24L01_WRITE_REG|RX_ADDR_P2, G_NRF24L01_Config.addr.rx_p2 ); 
                               __writeReg( NRF24L01_WRITE_REG|RX_PW_P2  , G_NRF24L01_Config.data.len   ); 
                               break;
        case NRF24L01_PIPE_P3: memcpy( &G_NRF24L01_Config.addr.rx_p3 , addr, len );
                               __writeReg( NRF24L01_WRITE_REG|RX_ADDR_P3, G_NRF24L01_Config.addr.rx_p3 ); 
                               __writeReg( NRF24L01_WRITE_REG|RX_PW_P3  , G_NRF24L01_Config.data.len   ); 
                               break;
        case NRF24L01_PIPE_P4: memcpy( &G_NRF24L01_Config.addr.rx_p4 , addr, len );
                               __writeReg( NRF24L01_WRITE_REG|RX_ADDR_P4, G_NRF24L01_Config.addr.rx_p4 ); 
                               __writeReg( NRF24L01_WRITE_REG|RX_PW_P4  , G_NRF24L01_Config.data.len   ); 
                               break;
        case NRF24L01_PIPE_P5: memcpy( &G_NRF24L01_Config.addr.rx_p5 , addr, len );
                               __writeReg( NRF24L01_WRITE_REG|RX_ADDR_P5, G_NRF24L01_Config.addr.rx_p5 ); 
                               __writeReg( NRF24L01_WRITE_REG|RX_PW_P5  , G_NRF24L01_Config.data.len   ); 
                               break;    
    }                               
    CE(1);
}

const uint8_t* NRF24L01_getTXAddr(uint8_t* byteCnt){
    if( byteCnt!=NULL )
        *byteCnt = sizeof(G_NRF24L01_Config.addr.tx);
    return G_NRF24L01_Config.addr.tx;
}

const uint8_t* NRF24L01_getRXAddr(uint8_t* byteCnt){
    if( byteCnt!=NULL )
        *byteCnt = sizeof(G_NRF24L01_Config.addr.rx_p0);
    return G_NRF24L01_Config.addr.rx_p0;
}

void NRF24L01_autoACK( bool cmd ){
    G_NRF24L01_Config.autoACK = cmd;
    CE(0);
    __writeReg( NRF24L01_WRITE_REG|EN_AA, (cmd==true)?0x3f:0x00 );
    CE(1);
    NRF24L01_DELAY_US(130);
}

void NRF24L01_enCRC( bool cmd ){
    CE(0);
    REG_CONFIG_t config = { .val = __readReg( NRF24L01_WRITE_REG|CONFIG ) };
    config._EN_CRC = G_NRF24L01_Config.crc = cmd;
    __writeReg( NRF24L01_WRITE_REG|CONFIG, config.val );
    CE(1);
    NRF24L01_DELAY_US(130);
}

void NRF24L01_wtCfg( const S_NRF24L01_CFG_t *p ){
    CE(0);
    /* 基本配置 */
    {
        REG_CONFIG_t config = {0};
        
        config._PWR_UP      = false;
        config._CRCO        = (uint8_t)p->crco;
        config._EN_CRC      = (uint8_t)p->crc;
        config._MASK_MAX_RT = (uint8_t)0;
        config._MASK_TX_DS  = (uint8_t)0;
        config._MASK_RX_DR  = (uint8_t)0; 

        __writeReg( NRF24L01_WRITE_REG|CONFIG, config.val );
    }

    /* 自动应答 */
    {
        if( p->autoACK ){
            __writeReg( NRF24L01_WRITE_REG|EN_AA, 0x3f );
        }else{
            __writeReg( NRF24L01_WRITE_REG|EN_AA, 0x00 );
        }
    }
    
    /* 开启通道 */
    {
        __writeReg( NRF24L01_WRITE_REG|EN_RXADDR  , (1<<(uint8_t)p->pipe) );
    }

    /* 设置地址宽度 */
    //...//

    /* 设置自动重发延时 */
    //...//

    /* 射频通信频率 */
    {
        REG_RFCH_t rf_ch = {0};

        rf_ch._RF_CH = (uint8_t)p->freq;

        __writeReg( NRF24L01_WRITE_REG|RF_CH , rf_ch.val );
    }

    /* 射频参数 */
    {
        REG_RFSETUP_t rf_setup = {0};

        rf_setup._LNA_HCURR = (uint8_t)p->gainLNA;
        rf_setup._PLL_LOCK  = (uint8_t)p->lockPLL;
        rf_setup._RF_DR     = (uint8_t)p->rate;
        rf_setup._RF_PWR    = (uint8_t)p->pwr;

        __writeReg( NRF24L01_WRITE_REG|RF_SETUP  , rf_setup.val );
    }

    /* 设置收发地址 & 配置数据宽度 */
    {
        switch( p->pipe ){
            case NRF24L01_PIPE_P0: __writeBuf( NRF24L01_WRITE_REG|RX_ADDR_P0, p->addr.rx_p0, sizeof(p->addr.rx_p0) ); 
                                   __writeReg( NRF24L01_WRITE_REG|RX_PW_P0  , p->data.len   ); 
                                   break;
            case NRF24L01_PIPE_P1: __writeBuf( NRF24L01_WRITE_REG|RX_ADDR_P1, p->addr.rx_p1, sizeof(p->addr.rx_p1) ); 
                                   __writeReg( NRF24L01_WRITE_REG|RX_PW_P1  , p->data.len   ); 
                                   break;
            case NRF24L01_PIPE_P2: __writeReg( NRF24L01_WRITE_REG|RX_ADDR_P2, p->addr.rx_p2 ); 
                                   __writeReg( NRF24L01_WRITE_REG|RX_PW_P2  , p->data.len   ); 
                                   break;
            case NRF24L01_PIPE_P3: __writeReg( NRF24L01_WRITE_REG|RX_ADDR_P3, p->addr.rx_p3 ); 
                                   __writeReg( NRF24L01_WRITE_REG|RX_PW_P3  , p->data.len   ); 
                                   break;
            case NRF24L01_PIPE_P4: __writeReg( NRF24L01_WRITE_REG|RX_ADDR_P4, p->addr.rx_p4 ); 
                                   __writeReg( NRF24L01_WRITE_REG|RX_PW_P4  , p->data.len   ); 
                                   break;
            case NRF24L01_PIPE_P5: __writeReg( NRF24L01_WRITE_REG|RX_ADDR_P5, p->addr.rx_p5 ); 
                                   __writeReg( NRF24L01_WRITE_REG|RX_PW_P5  , p->data.len   ); 
                                   break;                       
        }
        __writeBuf( NRF24L01_WRITE_REG|TX_ADDR, p->addr.tx, sizeof(p->addr.tx) );
    }

    CE(1);
}

void NRF24L01_rdCfg(       S_NRF24L01_CFG_t *p ){

    NRF24L01_ASSERT( p );

    CE(0);
    {
        REG_CONFIG_t config = { .val = __readReg( NRF24L01_READ_REG|CONFIG ) };
        p->crco   = (E_NRF24L01_CRCo_t)config._CRCO;
        p->crc    = (bool)config._EN_CRC;
    }

    {
        p->autoACK = (bool)(0x3f&__readReg( NRF24L01_WRITE_REG|EN_AA ));
    }

    {
        REG_RFCH_t   rf_ch  = { .val = __readReg( NRF24L01_READ_REG|RF_CH ) };
        p->freq   = (E_NRF24L01_Freq_t)rf_ch._RF_CH;
    }

    {
        REG_RFSETUP_t rf_setup = { .val = __readReg( NRF24L01_READ_REG|RF_SETUP ) };
        p->gainLNA = (bool)rf_setup._LNA_HCURR;
        p->lockPLL = (bool)rf_setup._PLL_LOCK;
        p->rate    = (E_NRF24L01_Rate_t)rf_setup._RF_DR;
        p->pwr     = (E_NRF24L01_Pwr_t )rf_setup._RF_PWR;
    }

    {
        __readBuf( NRF24L01_READ_REG|TX_ADDR   , p->addr.tx   , sizeof(p->addr.tx   ) );
        __readBuf( NRF24L01_READ_REG|RX_ADDR_P0, p->addr.rx_p0, sizeof(p->addr.rx_p0) );
        __readBuf( NRF24L01_READ_REG|RX_ADDR_P1, p->addr.rx_p1, sizeof(p->addr.rx_p1) );  
        p->addr.rx_p2 = __readReg( NRF24L01_READ_REG|RX_ADDR_P2 );
        p->addr.rx_p3 = __readReg( NRF24L01_READ_REG|RX_ADDR_P3 );
        p->addr.rx_p4 = __readReg( NRF24L01_READ_REG|RX_ADDR_P4 );
        p->addr.rx_p5 = __readReg( NRF24L01_READ_REG|RX_ADDR_P5 );
    }

    CE(1);
}


#ifdef NRF24L01_DEMO

#define M_TX_addr    {0xcc,0xcc,0xcc,0xcc,0xcc}
#define M_RXP0_addr  {0xdd,0xdd,0xdd,0xdd,0xdd}
#define M_RXP1_addr  {0xee,0xee,0xee,0xee,0xee}

#define M_DATA       {'R','a','n','d','l','e','H'}

S_NRF24L01_CFG_t cfg = {
    .autoACK = true ,
    .crc     = true ,
    .gainLNA = true ,
    .lockPLL = false,
    .pipe       = NRF24L01_PIPE_P0      ,
    .rate       = NRF24L01_RATE_1Mbps   ,
    .freq       = NRF24L01_FREQ_2452MHz ,
    .crco       = NRF24L01_CRCO_1Byte   ,
    .pwr        = NRF24L01_PWR_6dBm     ,
    .addr.tx    = M_TX_addr   ,
    .addr.rx_p0 = M_RXP0_addr ,
    .addr.rx_p1 = M_RXP1_addr ,
    .data.len   = 32
};

void NRF24L01_demo_rx(void){
    NRF24L01_init();
    NRF24L01_rx( &cfg );
    
    while(1){
        if( NRF24L01_hasMessage() ){
            uint8_t buf[32] = {0};
            NRF24L01_recv( buf, sizeof(buf) );
            //...//
        }
    }
}

void NRF24L01_demo_tx(void){
    NRF24L01_init();
    NRF24L01_tx( &cfg );

    while(1){
        uint8_t buf[32] = M_DATA;
        NRF24L01_send( buf, sizeof(buf));
        NRF24L01_DELAY_MS(2000);
        //...//
    }
}

#endif








