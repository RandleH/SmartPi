#ifndef _NRF24L01_H
#define _NRF24L01_H 

#include <stdbool.h>
#include <stdint.h>


/*==========================================
 * 硬件驱动API --- 基于STM32F0x
==========================================*/   
#include "stm32f10x_conf.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"

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

/*==========================================
 * C标准库API
==========================================*/     
#include "SMP_delay.h"
#include "RH_config.h"  
                       
#define   NRF24L01_DELAY_MS(ms)    SMP_delay_ms( ms ) // 假的延时函数, freeRTOS无法兼容正常延时, 因此每次切换模块模式需自行延时
#define   NRF24L01_DELAY_US(us)    SMP_delay_us( us ) 
#define   NRF24L01_ASSERT(expr)    RH_ASSERT(expr)
#define   NRF24L01_MALLOC(x)       RH_MALLOC(x)
#define   NRF24L01_CALLOC(x,s)     RH_CALLOC(x,s)
#define   NRF24L01_FREE(x)         RH_FREE(x)     

/*==========================================
 * 枚举通信速率
==========================================*/
typedef enum{
    NRF24L01_RATE_1Mbps = 0,
    NRF24L01_RATE_2Mbps = 1,
}E_NRF24L01_Rate_t;

/*==========================================
 * 枚举通信频率 2.4GHz ~ 2.525GHz
==========================================*/
typedef enum{
    NRF24L01_FREQ_2400MHz = 0,
    NRF24L01_FREQ_2401MHz,
    NRF24L01_FREQ_2402MHz,
    NRF24L01_FREQ_2403MHz,
    NRF24L01_FREQ_2404MHz,
    NRF24L01_FREQ_2405MHz,
    NRF24L01_FREQ_2406MHz,
    NRF24L01_FREQ_2407MHz,
    NRF24L01_FREQ_2408MHz,
    NRF24L01_FREQ_2409MHz,
    NRF24L01_FREQ_2410MHz,
    NRF24L01_FREQ_2411MHz,
    NRF24L01_FREQ_2412MHz,
    NRF24L01_FREQ_2413MHz,
    NRF24L01_FREQ_2414MHz,
    NRF24L01_FREQ_2415MHz,
    NRF24L01_FREQ_2416MHz,
    NRF24L01_FREQ_2417MHz,
    NRF24L01_FREQ_2418MHz,
    NRF24L01_FREQ_2419MHz,
    NRF24L01_FREQ_2420MHz,
    NRF24L01_FREQ_2421MHz,
    NRF24L01_FREQ_2422MHz,
    NRF24L01_FREQ_2423MHz,
    NRF24L01_FREQ_2424MHz,
    NRF24L01_FREQ_2425MHz,
    NRF24L01_FREQ_2426MHz,
    NRF24L01_FREQ_2427MHz,
    NRF24L01_FREQ_2428MHz,
    NRF24L01_FREQ_2429MHz,
    NRF24L01_FREQ_2430MHz,
    NRF24L01_FREQ_2431MHz,
    NRF24L01_FREQ_2432MHz,
    NRF24L01_FREQ_2433MHz,
    NRF24L01_FREQ_2434MHz,
    NRF24L01_FREQ_2435MHz,
    NRF24L01_FREQ_2436MHz,
    NRF24L01_FREQ_2437MHz,
    NRF24L01_FREQ_2438MHz,
    NRF24L01_FREQ_2439MHz,
    NRF24L01_FREQ_2440MHz,
    NRF24L01_FREQ_2441MHz,
    NRF24L01_FREQ_2442MHz,
    NRF24L01_FREQ_2443MHz,
    NRF24L01_FREQ_2444MHz,
    NRF24L01_FREQ_2445MHz,
    NRF24L01_FREQ_2446MHz,
    NRF24L01_FREQ_2447MHz,
    NRF24L01_FREQ_2448MHz,
    NRF24L01_FREQ_2449MHz,
    NRF24L01_FREQ_2450MHz,
    NRF24L01_FREQ_2451MHz,
    NRF24L01_FREQ_2452MHz,
    NRF24L01_FREQ_2453MHz,
    NRF24L01_FREQ_2454MHz,
    NRF24L01_FREQ_2455MHz,
    NRF24L01_FREQ_2456MHz,
    NRF24L01_FREQ_2457MHz,
    NRF24L01_FREQ_2458MHz,
    NRF24L01_FREQ_2459MHz,
    NRF24L01_FREQ_2460MHz,
    NRF24L01_FREQ_2461MHz,
    NRF24L01_FREQ_2462MHz,
    NRF24L01_FREQ_2463MHz,
    NRF24L01_FREQ_2464MHz,
    NRF24L01_FREQ_2465MHz,
    NRF24L01_FREQ_2466MHz,
    NRF24L01_FREQ_2467MHz,
    NRF24L01_FREQ_2468MHz,
    NRF24L01_FREQ_2469MHz,
    NRF24L01_FREQ_2470MHz,
    NRF24L01_FREQ_2471MHz,
    NRF24L01_FREQ_2472MHz,
    NRF24L01_FREQ_2473MHz,
    NRF24L01_FREQ_2474MHz,
    NRF24L01_FREQ_2475MHz,
    NRF24L01_FREQ_2476MHz,
    NRF24L01_FREQ_2477MHz,
    NRF24L01_FREQ_2478MHz,
    NRF24L01_FREQ_2479MHz,
    NRF24L01_FREQ_2480MHz,
    NRF24L01_FREQ_2481MHz,
    NRF24L01_FREQ_2482MHz,
    NRF24L01_FREQ_2483MHz,
    NRF24L01_FREQ_2484MHz,
    NRF24L01_FREQ_2485MHz,
    NRF24L01_FREQ_2486MHz,
    NRF24L01_FREQ_2487MHz,
    NRF24L01_FREQ_2488MHz,
    NRF24L01_FREQ_2489MHz,
    NRF24L01_FREQ_2490MHz,
    NRF24L01_FREQ_2491MHz,
    NRF24L01_FREQ_2492MHz,
    NRF24L01_FREQ_2493MHz,
    NRF24L01_FREQ_2494MHz,
    NRF24L01_FREQ_2495MHz,
    NRF24L01_FREQ_2496MHz,
    NRF24L01_FREQ_2497MHz,
    NRF24L01_FREQ_2498MHz,
    NRF24L01_FREQ_2499MHz,
    NRF24L01_FREQ_2500MHz,
    NRF24L01_FREQ_2501MHz,
    NRF24L01_FREQ_2502MHz,
    NRF24L01_FREQ_2503MHz,
    NRF24L01_FREQ_2504MHz,
    NRF24L01_FREQ_2505MHz,
    NRF24L01_FREQ_2506MHz,
    NRF24L01_FREQ_2507MHz,
    NRF24L01_FREQ_2508MHz,
    NRF24L01_FREQ_2509MHz,
    NRF24L01_FREQ_2510MHz,
    NRF24L01_FREQ_2511MHz,
    NRF24L01_FREQ_2512MHz,
    NRF24L01_FREQ_2513MHz,
    NRF24L01_FREQ_2514MHz,
    NRF24L01_FREQ_2515MHz,
    NRF24L01_FREQ_2516MHz,
    NRF24L01_FREQ_2517MHz,
    NRF24L01_FREQ_2518MHz,
    NRF24L01_FREQ_2519MHz,
    NRF24L01_FREQ_2520MHz,
    NRF24L01_FREQ_2521MHz,
    NRF24L01_FREQ_2522MHz,
    NRF24L01_FREQ_2523MHz,
    NRF24L01_FREQ_2524MHz,
    NRF24L01_FREQ_2525MHz,
}E_NRF24L01_Freq_t;

/*==========================================
 * 枚举通信通道
==========================================*/
typedef enum{
    NRF24L01_PIPE_P0 = 0,
    NRF24L01_PIPE_P1,
    NRF24L01_PIPE_P2,
    NRF24L01_PIPE_P3,
    NRF24L01_PIPE_P4,
    NRF24L01_PIPE_P5
}E_NRF24L01_Pipe_t;

/*==========================================
 * 枚举射频功率
==========================================*/
typedef enum{
    NRF24L01_PWR_0dBm  = 3,
    NRF24L01_PWR_6dBm  = 2,
    NRF24L01_PWR_12dBm = 1,
    NRF24L01_PWR_18dBm = 0,
}E_NRF24L01_Pwr_t;

/*==========================================
 * 枚举CRC校验字节
==========================================*/
typedef enum{
    NRF24L01_CRCO_1Byte = 0,
    NRF24L01_CRCO_2Byte = 1
}E_NRF24L01_CRCo_t;

/*==========================================
 * 通信收发地址
==========================================*/
typedef struct S_NRF24L01_Addr_t{
    uint8_t    tx[5];   
    uint8_t    rx_p0[5];
    uint8_t    rx_p1[5];
    uint8_t    rx_p2;
    uint8_t    rx_p3;
    uint8_t    rx_p4;
    uint8_t    rx_p5;
}S_NRF24L01_Addr_t;

/*==========================================
 * 通信数据包 最多32字节
==========================================*/
typedef struct S_NRF24L01_Data_t{
	uint8_t   *buf; // 数据流
	uint8_t    len; // 数据字节数 范围: 0~32
}S_NRF24L01_Data_t;

/*==========================================
 * NRF24L01配置参数汇总
==========================================*/
struct S_NRF24L01_CFG_t{
	bool       autoACK;
    bool       crc;
    bool       gainLNA;
    bool       lockPLL;

    E_NRF24L01_Pipe_t    pipe;
    E_NRF24L01_Rate_t    rate;
    E_NRF24L01_Freq_t    freq;
    E_NRF24L01_CRCo_t    crco;
    E_NRF24L01_Pwr_t     pwr;
    
    S_NRF24L01_Addr_t    addr;
    S_NRF24L01_Data_t    data;  
};
typedef struct S_NRF24L01_CFG_t  S_NRF24L01_CFG_t;
typedef struct S_NRF24L01_CFG_t* P_NRF24L01_CFG_t;


/*========================================================
 * NRF24L01 模式设定
==========================================================
 * 以下函数将更改模块状态
 * init  --- 初始化
 * check --- 检查
 * tx    --- 发送模式
 * rx    --- 接收模式
 * pd    --- 掉电模式

 * 特别注意:

 * 任何的收发配置不一致 都将导致传输失败. 建议使用结构体批量化配置.
 * NRF24L01 共有6个接收通道, P0/P1 分别占5字节, P[2:5]共享P1的
   前4字节. 更改P1通道地址将会影响P[2:5]. 同样, 更改任意P[2:5]地
   址将会影响P1.
 * 自动响应AutoAcknowledge开启后, 接收端ACK信号自动发送至发送端
   的P0接收通道. 因此如果开启, 则P0地址将会被同步成TX地址.
 * 自动响应AutoAcknowledge开启后, 接收端也应当开启, 否则发送端
   收不到ACK, 误认为发送失败.
 * 强制开启IRQ, 即本驱动永久开启中断电平信号.  
 * 使用结构体进行配置, 将会覆盖之前的模块配置. 如需保存, 建议先使用
   NRF24L01_rdCfg函数 将之前的配置导出, 再应用新配置.
 * 涉及可移植性, 不提供延时函数API, 以下函数执行后需延时2ms:
   > NRF24L01_tx
   > NRF24L01_rx
   > NRF24L01_pd
   > NRF24L01_setTXAddr
   > NRF24L01_setRXAddr
   > NRF24L01_setFreq
 
 * 默认的配置结构体:
   .autoACK = true  ,\
   .crc     = true  ,\
   .gainLNA = true  ,\
   .lockPLL = false ,\
   .pipe    = NRF24L01_PIPE_P0      ,\
   .rate    = NRF24L01_RATE_2Mbps   ,\
   .freq    = NRF24L01_FREQ_2440MHz ,\
   .crco    = NRF24L01_CRCO_2Byte   ,\
   .pwr     = NRF24L01_PWR_0dBm     ,\
   .addr.tx     = { 0xff, 0xff, 0xff, 0xff, 0x7f } ,\
   .addr.rx_p0  = { 0xff, 0xff, 0xff, 0xff, 0x7f } ,\
   .addr.rx_p1  = { 0xff, 0xff, 0xff, 0xff, 0x8f } ,\
   .data.buf = cache  ,\
   .data.len = sizeof(cache)  \

=========================================================*/
void      NRF24L01_init       ( void );
bool      NRF24L01_exist      ( void );
bool      NRF24L01_hasMessage ( void );
void      NRF24L01_tx         ( const S_NRF24L01_CFG_t  *p ); //  NULL: 使用默认配置; ptr: 使用自定义配置; 将配置写入模块并设置成发送模式 
void      NRF24L01_rx         ( const S_NRF24L01_CFG_t  *p ); //  NULL: 使用默认配置; ptr: 使用自定义配置; 将配置写入模块并设置成接收模式
void      NRF24L01_pd         ( const S_NRF24L01_CFG_t  *p ); //  NULL: 使用默认配置; ptr: 使用自定义配置; 将配置写入模块并设置成掉电模式 
void      NRF24L01_rdCfg      (       S_NRF24L01_CFG_t  *p ); //  NULL: 不允许     ; ptr: 目标写入     ; 从模块读取当前配置 
void      NRF24L01_wtCfg      ( const S_NRF24L01_CFG_t  *p ); //  NULL: 使用默认配置; ptr: 使用自定义配置; 将配置写入模块 

/*========================================================
 * NRF24L01 配置参数设定
==========================================================
 * 以下函数将更改模块参数
=========================================================*/
void               NRF24L01_autoACK    ( bool cmd );
void               NRF24L01_enCRC      ( bool cmd );

void               NRF24L01_setTXAddr  (                         const uint8_t addr[5] );
void               NRF24L01_setRXAddr  ( E_NRF24L01_Pipe_t pipe, const uint8_t addr[5] );

uint8_t            NRF24L01_getTXAddr  (                         uint8_t addr[5] );
uint8_t            NRF24L01_getRXAddr  ( E_NRF24L01_Pipe_t pipe, uint8_t addr[5] );


E_NRF24L01_Freq_t  NRF24L01_getFreq    ( void );
void               NRF24L01_setFreq    ( E_NRF24L01_Freq_t freq );


/*========================================================
 * NRF24L01 收发函数 
==========================================================
 * 返回参数为状态寄存器STATUS值
=========================================================*/
uint8_t   NRF24L01_send   ( uint8_t *data, uint8_t len );//
uint8_t   NRF24L01_recv   ( uint8_t *buf , uint8_t len );//

#define NRF24L01_DEMO
#ifdef  NRF24L01_DEMO
void NRF24L01_demo_tx(void);
void NRF24L01_demo_rx(void);
#endif

#endif




