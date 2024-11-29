/********************************************************************************
  * 文 件 名: bsp_mq2.c
  * 版 本 号: 初版
  * 修改作者: LC
  * 修改日期: 2023年04月06日
  * 功能介绍:          
  ******************************************************************************
  * 注意事项:
*********************************************************************************/


#include "bsp_mq2.h"
#include "systick.h"


 //DMA缓冲区
uint16_t gt_adc_val[ SAMPLES ][ CHANNEL_NUM ]; 


/******************************************************************
 * 函 数 名 称：ADC_DMA_Init
 * 函 数 说 明：初始化ADC+DMA功能
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void ADC_DMA_Init(void)
{
    /* DMA初始化功能结构体定义 */
    dma_parameter_struct dma_single_data_parameter;
        
        /* 使能引脚时钟 */
    rcu_periph_clock_enable(RCU_MQ2_GPIO_AO);                
    rcu_periph_clock_enable(RCU_MQ2_GPIO_DO);                
        
        /* 使能ADC时钟 */
	rcu_periph_clock_enable(RCU_ADC);                
  
        /* 使能DMA时钟 */
    rcu_periph_clock_enable(RCU_DMA);
 
        /*        配置ADC时钟        */ 
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);        
        
//   /*        配置PA1(DO)为输入模式        */
//    gpio_mode_set(PORT_ILLUME_DO, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_ILLUME_DO); 
//        /*        配置PC1(AO)为浮空模拟输入模式        */
//    gpio_mode_set(PORT_ILLUME_AO, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_ILLUME_AO);  
	
	gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_MQ2_AO);
	gpio_mode_set(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_MQ2_DO);
	
	/*        清除 DMA通道0 之前配置         */
    dma_deinit(CHANNEL_DMA);
   
        /*        DMA初始化配置         */
        dma_single_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA);       //设置DMA传输的外设地址为ADC0基地址
        dma_single_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;   //关闭外设地址自增
        dma_single_data_parameter.memory_addr = (uint32_t)(gt_adc_val);       //设置DMA传输的内存地址为 gt_adc_val数组
        dma_single_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;    //开启内存地址自增（因为不止一个通道）
        dma_single_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;  //传输的数据位 为 16位
        dma_single_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
		dma_single_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;       //DMA传输方向为 外设往内存
        dma_single_data_parameter.number = SAMPLES * CHANNEL_NUM;             //传输的数据长度为：每个通道采集30次 * 1个通道
        dma_single_data_parameter.priority = DMA_PRIORITY_HIGH;               //设置高优先级
        dma_init(CHANNEL_DMA, &dma_single_data_parameter);//将配置保存至DMA1的通道0
        
        /*        DMA通道外设选择                */
        /*        数据手册的195页根据PERIEN[2:0]值确定第三个参数，例是100 则为DMA_SUBPERI4          例是010 则为DMA_SUBPERI2         */
        /*        我们是ADC0功能，PERIEN[2:0]值为000，故为DMA_SUBPERI0                */
        //dma_channel_subperipheral_select(PORT_DMA, CHANNEL_DMA, DMA_SUBPERI0);
 
        /*        使能DMA1通道0循环模式                */
        dma_circulation_enable(CHANNEL_DMA);
 
        /*        启动DMA1的通道0功能                */
        dma_channel_enable(CHANNEL_DMA); 
 
        /*        配置ADC为独立模式        */
     //adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
        
    /*        使能连续转换模式        */
    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);
         
    /*        使能扫描模式        */
    adc_special_function_config(ADC_SCAN_MODE, ENABLE);
   
        /*        数据右对齐        */        
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    
    /*        ADC0设置为规则组  一共使用 CHANNEL_NUM 个通道                */  
    adc_channel_length_config(ADC_REGULAR_CHANNEL, CHANNEL_NUM);
                
    /*        ADC规则通道配置：ADC0的通道11的扫描顺序为0；采样时间：15个周期                */  
        /*        DMA开启之后 gt_adc_val[x][0] = PC1的数据   */
    adc_regular_channel_config(0, CHANNEL_ADC, ADC_SAMPLETIME_13POINT5);
 
        /*        ADC0设置为12位分辨率                */  
        adc_resolution_config(ADC_RESOLUTION_12B); 
        
        /*        ADC外部触发禁用, 即只能使用软件触发                */  
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE); 
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
    
    /*        使能规则组通道每转换完成一个就发送一次DMA请求                */  
    // adc_dma_request_after_last_enable(PORT_ADC);  
	
	        /*        使能DMA                */          
        adc_enable();
		
		        /*        等待ADC稳定                */  
        delay_1ms(1U);
		
		        /*        开启ADC自校准                */
		adc_calibration_enable(); 
    
        /*        使能DMA请求                */  
        adc_dma_mode_enable();

        
        /*        开启软件触发ADC转换                */
        adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}
 

/******************************************************************
 * 函 数 名 称：Get_Adc_Dma_Value
 * 函 数 说 明：对DMA保存的数据进行平均值计算后输出
 * 函 数 形 参：CHx 第几个扫描的数据
 * 函 数 返 回：对应扫描的ADC值
 * 作       者：LC
 * 备       注：无
******************************************************************/
unsigned int Get_Adc_Dma_Value(char CHx)
{
        unsigned char i = 0;
        unsigned int AdcValue = 0;
    
    /* 因为采集 SAMPLES 次，故循环 SAMPLES 次 */
        for(i=0; i< SAMPLES; i++)
        {
        /*    累加    */
                AdcValue+=gt_adc_val[i][CHx];
        }
    /* 求平均值 */
        AdcValue=AdcValue / SAMPLES;
    
        return AdcValue;
}

/******************************************************************
 * 函 数 名 称：Get_MQ2_Percentage_value
 * 函 数 说 明：读取MQ2值，并且返回百分比
 * 函 数 形 参：无
 * 函 数 返 回：返回百分比
 * 作       者：LC
 * 备       注：无
******************************************************************/
unsigned int Get_MQ2_Percentage_value(void)
{
    int adc_max = 4095;
    int adc_new = 0;
    int Percentage_value = 0;
    
    adc_new = Get_Adc_Dma_Value(0);
    
    Percentage_value = ((float)adc_new/adc_max) * 100;
    return Percentage_value;
}