 /******************************************************************************
   * 测试硬件：立创开发板·GD32E230C8T6    使用主频72Mhz    晶振8Mhz
   * 版 本 号: V1.0
   * 修改作者: www.lckfb.com
   * 修改日期: 2023年11月02日
   * 功能介绍:      
   *****************************************************************************
   * 梁山派软硬件资料与相关扩展板软硬件资料官网全部开源  
   * 开发板官网：www.lckfb.com   
   * 技术支持常驻论坛，任何技术问题欢迎随时交流学习  
   * 立创论坛：club.szlcsc.com   
   * 其余模块移植手册：【立创·GD32E230C8T6开发板】模块移植手册
   * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
   * 不靠卖板赚钱，以培养中国工程师为己任
  ******************************************************************************/

#include "bsp_ultrasonic.h"
#include "systick.h"
#include "stdio.h"
#include "OLED.h"
#include "gd32e23x.h"                   // Device header


unsigned char msHcCount = 0;//ms计数
float distance = 0;
extern uint8_t lengthflag;
/******************************************************************
 * 函 数 名 称：bsp_ultrasonic
 * 函 数 说 明：超声波初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：TRIG引脚负责发送超声波脉冲串
******************************************************************/
void Ultrasonic_Init(void)
{
        // 定义定时器结构体
        timer_parameter_struct timer_initpara;          
        
        rcu_periph_clock_enable(RCU_TRIG);
		rcu_periph_clock_enable(RCU_ECHO);
        rcu_periph_clock_enable(RCU_TIMER5);	        /* 开启时钟 */
	
	    timer_deinit(TIMER5);
	
		timer_prescaler_config(0, 1, TIMER_PSC_RELOAD_NOW);
    
        //设置TRIG引脚模式为推挽输出
        gpio_mode_set(PORT_TRIG, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_TRIG);
        //设置引脚为推挽模式，翻转速度50MHz
        gpio_output_options_set(PORT_TRIG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_TRIG);        
        //设置引脚输出低电平
        gpio_bit_write(PORT_TRIG, GPIO_TRIG, RESET);
        
    
        //设置echo引脚模式为浮空输入
        gpio_mode_set(PORT_ECHO, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_ECHO);
        //设置引脚输出低电平
		
        gpio_bit_write(PORT_ECHO, GPIO_ECHO, RESET);

        /*        将定时器时钟的频率4倍频为200MHz        */
        //rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);        
        // 复位定时器

        timer_initpara.period           = 999;				//定时1ms
        timer_initpara.prescaler        = 71;
        timer_initpara.clockdivision    = TIMER_CKDIV_DIV1;    	// 分频因子
        timer_initpara.alignedmode      = TIMER_COUNTER_EDGE;  	// 边缘对齐
        timer_initpara.counterdirection = TIMER_COUNTER_UP;     // 向上计数
        
        /* 在输入捕获的时候使用 数字滤波器使用的采样频率之间的分频比例 */
        timer_initpara.clockdivision = TIMER_CKDIV_DIV1; // 分频因子
		
		
        /* 只有高级定时器才有 配置为 x，就重复 x+1 次进入中断 */
        timer_initpara.repetitioncounter = 0; // 重复计数器 0-255
		
        timer_init(TIMER5,&timer_initpara);         // 初始化定时器
		
				/* 配置中断优先级 */
        nvic_irq_enable(TIMER5_IRQn,0); // 设置中断优先级为 1,1
      
        /* 使能中断 */
        timer_interrupt_enable(TIMER5,TIMER_INT_UP); // 使能更新事件中断

		//timer_interrupt_flag_clear(TIMER5, TIMER_INT_FLAG_UP);
		
		/* TIMER0 primary output function enable */
		timer_primary_output_config(TIMER5, ENABLE);
		
		/* auto-reload preload enable */
		timer_auto_reload_shadow_enable(TIMER5);
		
        /* 使能定时器 */
        timer_enable(TIMER5);

}

/******************************************************************
 * 函 数 名 称：TIMER5_DAC_IRQHandler
 * 函 数 说 明：定时器5的中断服务函数
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：1ms中断一次
******************************************************************/
void TIMER5_IRQHandler(void)
{
        static int i = 0;
        if( timer_interrupt_flag_get(TIMER5, TIMER_INT_UP) != RESET )
        {
                msHcCount++;
                timer_interrupt_flag_clear(TIMER5,TIMER_INT_UP);
        }
		if(distance<25)lengthflag=1;
		else lengthflag=0;
		//printf("IRQ\r\n");
}

/******************************************************************
 * 函 数 名 称：OpenTimer
 * 函 数 说 明：打开定时器
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
static void OpenTimer(void) //
{
		/*清除计数器*/
        timer_counter_value_config(TIMER5,0);
        msHcCount = 0;
        timer_enable(TIMER5);//使能定时器
}
 /******************************************************************
  * 函 数 名 称：CloseTimer
  * 函 数 说 明：关闭定时器
  * 函 数 形 参：无
  * 函 数 返 回：无
  * 作       者：LC
  * 备       注：无
 ******************************************************************/
static void CloseTimer(void)
{
	/*关闭计数器使能*/
	timer_disable(TIMER5);
}
 
/******************************************************************
 * 函 数 名 称：GetEchoTimer
 * 函 数 说 明：获取定时器时间
 * 函 数 形 参：无
 * 函 数 返 回：返回获取到的时间
 * 作       者：LC
 * 备       注：无
******************************************************************/
unsigned int GetEchoTimer(void)
{
   unsigned int time = 0;
        /*//当回响信号很长是，计数值溢出后重复计数，overCount用中断来保存溢出次数*/
        time = msHcCount*1000;//overCount每++一次，代表overCount毫秒，time微妙
        time += timer_counter_read(TIMER5);//获取计TIM5数寄存器中的计数值，一边计算回响信号时间
        timer_counter_value_config(TIMER5,0);//将TIM2计数寄存器的计数值清零
        delay_1ms(10);
        return time;
}

/******************************************************************
 * 函 数 名 称：Hcsr04GetLength
 * 函 数 说 明：获取测量距离
 * 函 数 形 参：无
 * 函 数 返 回：测量距离
 * 作       者：LC
 * 备       注：无
******************************************************************/
float Hcsr04GetLength(void)
{
        /*测5次数据计算一次平均值*/
        float length = 0;
        float t = 0;
        float sum = 0;
        unsigned int  i = 0;
		while(i <= 2){
                        gpio_bit_write(PORT_TRIG, GPIO_TRIG, SET);//trig拉高信号，发出高电平

                delay_1us(20);//持续时间超过10us
                gpio_bit_write(PORT_TRIG, GPIO_TRIG, RESET);//trig拉低信号，发出低电平
                /*Echo发出信号 等待回响信号*/
                /*输入方波后，模块会自动发射8个40KHz的声波，与此同时回波引脚（echo）端的电平会由0变为1；
                （此时应该启动定时器计时）；当超声波返回被模块接收到时，回波引 脚端的电平会由1变为0；
                （此时应该停止定时器计数），定时器记下的这个时间即为
                        超声波由发射到返回的总时长；*/
                
                while(gpio_input_bit_get(PORT_ECHO, GPIO_ECHO) == 0);//echo等待回响
                /*开启定时器*/
                OpenTimer();
                i = i+1; //每收到一次回响信号+1，收到5次就计算均值
                while(gpio_input_bit_get(PORT_ECHO, GPIO_ECHO) == 1);
                /*关闭定时器*/
                CloseTimer();
                /*获取Echo高电平时间时间*/
                t = GetEchoTimer();
                length = (float)t/58;//单位时cm
                sum += length;                
        }
        length = sum/2;//五次平均值
        distance = length;
		
		//printf("%f\r\n",length);
		
        return length;
}

/******************************************************************
 * 函 数 名 称：Get_distance
 * 函 数 说 明：返回测量后的距离；必须先采集有超声波数据
 * 函 数 形 参：无
 * 函 数 返 回：上一次测量过的距离
 * 作       者：LC
 * 备       注：无
******************************************************************/
float Get_distance(void)
{
        return distance;
}

