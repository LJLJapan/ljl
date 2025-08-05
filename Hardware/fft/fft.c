#include "fft.h"





/* 变量 */
uint32_t ifftFlag = 0; 
uint32_t fftSize = 0;

/* 输入和输出缓冲 */
static float32_t testOutput_f32[TEST_LENGTH_SAMPLES*2]; 
 float32_t testOutputMag_f32[TEST_LENGTH_SAMPLES*2]; //0,511幅值，512,1023频率
static float32_t testInput_f32[TEST_LENGTH_SAMPLES*2];
static float32_t Phase_f32[TEST_LENGTH_SAMPLES*2];


/*
*********************************************************************************************************
*	函 数 名: PowerPhaseRadians_f32
*	功能说明: 求相位
*	形    参：_ptr  复位地址，含实部和虚部
*             _phase 求出相位，单位角度制，范围(-180, 180]
*             _usFFTPoints  复数个数，每个复数是两个float32_t数值
*             _uiCmpValue  比较值，需要求出相位的数值
*	返 回 值: 无
*********************************************************************************************************
*/
void PowerPhaseRadians_f32(float32_t *_ptr, float32_t *_phase, uint16_t _usFFTPoints, float32_t _uiCmpValue)		
{
	float32_t lX, lY;
	uint16_t i;
	float32_t phase;
	float32_t mag;
	
	
	for (i=0; i <_usFFTPoints; i++)
	{
		lX= _ptr[2*i];  	  /* 实部 */
		lY= _ptr[2*i + 1];    /* 虚部 */ 
		
 		phase = atan2f(lY, lX);    		  				 /* atan2求解的结果范围是(-pi, pi], 弧度制 */
		arm_sqrt_f32((float32_t)(lX*lX+ lY*lY), &mag);   /* 求模 */
		
		if(_uiCmpValue > mag)
		{
			Phase_f32[i] = 0;			
		}
		else
		{
			Phase_f32[i] = phase* 180.0f/3.1415926f;   /* 将求解的结果由弧度转换为角度 */
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: arm_rfft_f32_app
*	功能说明: 调用函数arm_rfft_fast_f32计算幅频和相频
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void fft_arm_rfft_f32_app(const uint16_t *data)
{
	uint16_t i;
	arm_rfft_fast_instance_f32 S;
	
	
	/* 正变换 */
    ifftFlag = 0; 
	
	/* 初始化结构体S中的参数 */
 	arm_rfft_fast_init_f32(&S, TEST_LENGTH_SAMPLES);

	for(i=0; i<TEST_LENGTH_SAMPLES; i++)
	{
		/* 波形是由直流分量，50Hz正弦波组成，波形采样率4096，初始相位60° */
		testInput_f32[i] = data[i];//1 + cos(2*3.1415926f*50*i/4096 + 3.1415926f/3);
	}

	
	/* 4096点实序列快速FFT */ 
	arm_rfft_fast_f32(&S, testInput_f32, testOutput_f32, ifftFlag);
	
	/* 为了方便跟函数arm_cfft_f32计算的结果做对比，这里求解了4096组模值，实际函数arm_rfft_fast_f32
	   只求解出了512组  
	*/ 
 	arm_cmplx_mag_f32(testOutput_f32, testOutputMag_f32, TEST_LENGTH_SAMPLES);
	
	
	/* 求相频 */
	PowerPhaseRadians_f32(testOutput_f32, Phase_f32, TEST_LENGTH_SAMPLES, 0.5f);
	
	
	for (size_t i = 0; i < TEST_LENGTH_SAMPLES / 2; i++)
	{
		testOutputMag_f32[i] = testOutputMag_f32[i] / TEST_LENGTH_SAMPLES * 2;
		testOutputMag_f32[i+(TEST_LENGTH_SAMPLES/2)] = FFT_SAMPLE_FREQUENCY * i / TEST_LENGTH_SAMPLES; // 频率存储到后4096/2个，index + 4096/2 访问
	}
	
	
}
/**
 * @brief 输入引索得到频率
 * @param index 
 * @retval float 
 * @date 2023-08-04
 */
float fft_index_get_frequnce(int index)
{
	//将单位转换频率f = Fs*(0:(L/2)-1)/L;
	//LOG_DBG(TASKTAG, "fft max index:%d A:%f\n", index, testOutputMag_f32[index]);
	return (FFT_SAMPLE_FREQUENCY * index)/ TEST_LENGTH_SAMPLES;
}
/**
 * @brief 得到最大幅值的引索
 * @param filterFreq 跳过的点数
 * @retval int 
 * @date 2023-08-04
 */
// void fft_get_max_amplitude_index(int filterFreq,uint16_t* maxValue)
// {
// 	size_t i = 0,j = 0,z = 0;
// 	size_t size_Value = sizeof(maxValue)/sizeof(uint16_t);
// 	int max = i++;
// 	for(j = 0 ; j < size_Value ; j++);
// 	{
// 		maxValue[j] = 0;
// 	}
// 	for(j = 0 ; j < size_Value ; j++);
// 	{
// 		for (i = filterFreq; i < TEST_LENGTH_SAMPLES/2; i++)
// 		{
// 			if (testOutputMag_f32[max] < testOutputMag_f32[i] && fft_index_get_frequnce(i) > 2000)//1000以下的不要
// 			for(z = 0;z < j; z++)
// 				if(maxValue[z] == max )
// 				{
// 					z = 0xff;
// 					break;
// 				}
// 			if(z != 0xff)max = i;
				
// 		}
// 		maxValue[j] = max;
// 	}
// }
/**
 * @brief 得到最大幅值的引索
 * @param filterFreq 跳过的点数
 * @retval int 
 * @date 2023-08-04
 */
int fft_get_max_amplitude_index(int filterFreq)
{
	size_t i = filterFreq;
	int max = i++;
	
	for (; i < TEST_LENGTH_SAMPLES/2; i++)
	{
		if (testOutputMag_f32[max] < testOutputMag_f32[i] && fft_index_get_frequnce(i) > 2000)//2000以下的不要
			max = i;
	}
	return max;
}

