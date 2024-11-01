#include "gd32e23x.h"                   // Device header

#include "bluetooth.h"
#include "stdio.h"
#include "OLED.h"

#include <stdlib.h>
unsigned char Bluetooth_ConnectFlag = 0; //蓝牙连接状态 =0没有手机连接                =1有手机连接
char BLERX_BUFF[BLERX_LEN_MAX];     
unsigned char BLERX_FLAG = 0;
unsigned char BLERX_LEN = 0;

/************************************************************
 * 函数名称：Bluetooth_GPIO_Init
 * 函数说明：蓝牙RXTX引脚初始化
 * 型    参：bund=串口波特率
 * 返 回 值：无
 * 备    注：无
*************************************************************/
void Bluetooth_GPIO_Init(unsigned int bund)
{
        /* 使能引脚时钟 */
        rcu_periph_clock_enable(BLUETOOTH_TXD_RCU);
		rcu_periph_clock_enable(BLUETOOTH_RXD_RCU);
        /* 使能串口时钟 */
        rcu_periph_clock_enable(BLE_USART_RCU);
        
        /*        配置引脚为复用功能x */
        gpio_af_set(BLUETOOTH_RXD_PORT, BLE_USART_AF, BLUETOOTH_RXD_GPIO);
        gpio_af_set(BLUETOOTH_TXD_PORT, BLE_USART_AF, BLUETOOTH_TXD_GPIO);
        
        /*        配置引脚为复用上拉模式 */
        gpio_mode_set(BLUETOOTH_RXD_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, BLUETOOTH_RXD_GPIO);
        gpio_mode_set(BLUETOOTH_TXD_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, BLUETOOTH_TXD_GPIO);
        
        /*        配置引脚为为输出模式 */
        gpio_output_options_set(BLUETOOTH_RXD_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BLUETOOTH_RXD_GPIO);
        gpio_output_options_set(BLUETOOTH_TXD_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BLUETOOTH_TXD_GPIO);
        
        /*        设置USART的波特率 */
        usart_baudrate_set(BLE_USART, bund);
        
        /*        设置USART的校验位为无 */
        usart_parity_config(BLE_USART, USART_PM_NONE);
        
        /*        设置USART的数据位为8位 */
        usart_word_length_set(BLE_USART, USART_WL_8BIT);
        
        /*        设置USART的停止位为1位 */
        usart_stop_bit_set(BLE_USART, USART_STB_1BIT);
        
        /*        使能串口 */
        usart_enable(BLE_USART);
        
        /*        使能USART传输 */
        usart_transmit_config(BLE_USART, USART_TRANSMIT_ENABLE);
        
        /*        使能USART接收 */
        usart_receive_config(BLE_USART, USART_RECEIVE_ENABLE);
        
        /*        使能USART接收中断标志位 */
        usart_interrupt_enable(BLE_USART, USART_INT_RBNE);   
        
		/*        使能USART空闲中断标志位 */
        usart_interrupt_enable(BLE_USART, USART_INT_IDLE); // DLE 线检测中断

        /* 配置中断优先级 */
        nvic_irq_enable(BLE_USART_IRQ, 2); // 配置中断优先级
}

/******************************************************************
 * 函 数 名 称：BLE_Send_Bit
 * 函 数 说 明：向蓝牙发送单个字符
 * 函 数 形 参：ch=ASCII字符
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void BLE_Send_Bit(unsigned char ch)
{
        //发送字符
        usart_data_transmit(BLE_USART, ch);
        // 等待发送数据缓冲区标志自动置位
        while(RESET == usart_flag_get(BLE_USART, USART_FLAG_TBE) );
}  

/******************************************************************
 * 函 数 名 称：BLE_send_String
 * 函 数 说 明：向蓝牙发送字符串 
 * 函 数 形 参：str=发送的字符串
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void BLE_send_String(unsigned char *str)
{
        while( str && *str ) // 地址为空或者值为空跳出
        {        
                BLE_Send_Bit(*str++);
        }        
}

/******************************************************************
 * 函 数 名 称：Clear_BLERX_BUFF
 * 函 数 说 明：清除串口接收的数据
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void Clear_BLERX_BUFF(void)
{
        unsigned char i = BLERX_LEN_MAX-1;
        while(i)        
        {
                BLERX_BUFF[i--] = 0;
        }
        BLERX_LEN = 0;
}

/******************************************************************
 * 函 数 名 称：Bluetooth_Link_Gpio_Init
 * 函 数 说 明：蓝牙连接成功指示引脚state初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void Bluetooth_Link_Gpio_Init(void)        
{
        //打开4个LED的引脚时钟
        rcu_periph_clock_enable(BLUETOOTH_LINK_RCU);
        //设置引脚模式为上拉输入
        gpio_mode_set(BLUETOOTH_LINK_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, BLUETOOTH_LINK_GPIO);        
}

/******************************************************************
 * 函 数 名 称：Bluetooth_Init
 * 函 数 说 明：蓝牙初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：默认波特率为9600
******************************************************************/
void Bluetooth_Init(void)
{
    Bluetooth_GPIO_Init(9600);
    Bluetooth_Link_Gpio_Init();
    #if        DEBUG
         //在调试时，通过AT命令已经设置好模式
        //printf("Bluetooth_Init succeed!\r\n");

    #endif
}

/******************************************************************
 * 函 数 名 称：Get_Bluetooth_ConnectFlag
 * 函 数 说 明：获取手机连接状态 
 * 函 数 形 参：无
 * 函 数 返 回：返回1=已连接                返回0=未连接
 * 作       者：LC
 * 备       注：使用该函数前，必须先调用 Bluetooth_Mode 函数
******************************************************************/
unsigned char Get_Bluetooth_ConnectFlag(void)
{
        return Bluetooth_ConnectFlag;
}

/******************************************************************
 * 函 数 名 称：Bluetooth_Mode
 * 函 数 说 明：判断蓝牙模块的连接状态
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：未连接时STATE低电平   连接成功时STATE高电平
******************************************************************/
void Bluetooth_Mode(void)
{
        static char flag = 0;
        //如果没有手机连接
        if( DISCONNECT == BLUETOOTH_LINK ) 
        {            
        //连接状态为未连接
                Bluetooth_ConnectFlag = 0;
        //如果之前是连接状态
                if( flag == 1 )     
                {
                        flag = 0;//修改状态
                }
                return;
        }
        //如果手机已经连接
        if( CONNECT == BLUETOOTH_LINK )
        {
                Bluetooth_ConnectFlag = 1;
        //如果之前是断开状态
                if( flag == 0 )
                {
                      flag = 1;//修改状态
                }
        }
}

/******************************************************************
 * 函 数 名 称：Receive_Bluetooth_Data
 * 函 数 说 明：接收蓝牙数据
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
//void Receive_Bluetooth_Data(void)
//{
//    //获取蓝牙状态
//		//Bluetooth_Mode();
//        //如果手机已经连接
//        if( Bluetooth_ConnectFlag == 1 )
//        {
//                if( BLERX_FLAG == 1 )//接收到蓝牙数据
//                {
//                    BLERX_FLAG = 0;                
//            
//					//显示蓝牙发送过来的数据
//					//BLERX_BUFF为数据包
//					//printf("data = %s\r\n",BLERX_BUFF);
//					uint8_t actnumber=atoi(BLERX_BUFF);
//					//ActionCallBack(actnumber);
//					Clear_BLERX_BUFF();//清除接收缓存
//                }
//        }
//}

/******************************************************************
 * 函 数 名 称：Send_Bluetooth_Data
 * 函 数 说 明：向蓝牙模块发送数据
 * 函 数 形 参：dat=要发送的字符串
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：（如果手机连接了蓝牙，就是向手机发送数据）
******************************************************************/
void Send_Bluetooth_Data(char *dat)
{
    //获取蓝牙状态
    Bluetooth_Mode();
        //如果手机已经连接
        if( Bluetooth_ConnectFlag == 1 )
        {
        //发送数据
        BLE_send_String((unsigned char*)dat);
    }
}

/******************************************************************
 * 函 数 名 称：BLE_USART_IRQHandler
 * 函 数 说 明：连接蓝牙的串口中断服务函数
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void BLE_USART_IRQHandler(void)
{
        if(usart_interrupt_flag_get(BLE_USART,USART_INT_FLAG_RBNE) != RESET) // 接收缓冲区不为空
        {
                BLERX_BUFF[ BLERX_LEN ] = usart_data_receive(BLE_USART);
                #if        DEBUG
			//debug(BLERX_BUFF);
       // printf("%c",BLERX_BUFF[ BLERX_LEN ]);
        #endif
        BLERX_LEN = ( BLERX_LEN + 1 ) % BLERX_LEN_MAX;
        }
        if(usart_interrupt_flag_get(BLE_USART,USART_INT_FLAG_IDLE) == SET) // 检测到空闲中断
        {
				usart_interrupt_flag_clear(USART1, USART_INT_FLAG_IDLE);
                usart_data_receive(BLE_USART); // 必须要读，读出来的值不能要
                BLERX_BUFF[BLERX_LEN] = '\0';
                BLERX_FLAG = SET;// 接收完成
        }
}
