#include "fft.h"





/* ���� */
uint32_t ifftFlag = 0; 
uint32_t fftSize = 0;

/* ������������ */
static float32_t testOutput_f32[TEST_LENGTH_SAMPLES*2]; 
 float32_t testOutputMag_f32[TEST_LENGTH_SAMPLES*2]; //0,511��ֵ��512,1023Ƶ��
static float32_t testInput_f32[TEST_LENGTH_SAMPLES*2];
static float32_t Phase_f32[TEST_LENGTH_SAMPLES*2];


/*
*********************************************************************************************************
*	�� �� ��: PowerPhaseRadians_f32
*	����˵��: ����λ
*	��    �Σ�_ptr  ��λ��ַ����ʵ�����鲿
*             _phase �����λ����λ�Ƕ��ƣ���Χ(-180, 180]
*             _usFFTPoints  ����������ÿ������������float32_t��ֵ
*             _uiCmpValue  �Ƚ�ֵ����Ҫ�����λ����ֵ
*	�� �� ֵ: ��
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
		lX= _ptr[2*i];  	  /* ʵ�� */
		lY= _ptr[2*i + 1];    /* �鲿 */ 
		
 		phase = atan2f(lY, lX);    		  				 /* atan2���Ľ����Χ��(-pi, pi], ������ */
		arm_sqrt_f32((float32_t)(lX*lX+ lY*lY), &mag);   /* ��ģ */
		
		if(_uiCmpValue > mag)
		{
			Phase_f32[i] = 0;			
		}
		else
		{
			Phase_f32[i] = phase* 180.0f/3.1415926f;   /* �����Ľ���ɻ���ת��Ϊ�Ƕ� */
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: arm_rfft_f32_app
*	����˵��: ���ú���arm_rfft_fast_f32�����Ƶ����Ƶ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void fft_arm_rfft_f32_app(const uint16_t *data)
{
	uint16_t i;
	arm_rfft_fast_instance_f32 S;
	
	
	/* ���任 */
    ifftFlag = 0; 
	
	/* ��ʼ���ṹ��S�еĲ��� */
 	arm_rfft_fast_init_f32(&S, TEST_LENGTH_SAMPLES);

	for(i=0; i<TEST_LENGTH_SAMPLES; i++)
	{
		/* ��������ֱ��������50Hz���Ҳ���ɣ����β�����4096����ʼ��λ60�� */
		testInput_f32[i] = data[i];//1 + cos(2*3.1415926f*50*i/4096 + 3.1415926f/3);
	}

	
	/* 4096��ʵ���п���FFT */ 
	arm_rfft_fast_f32(&S, testInput_f32, testOutput_f32, ifftFlag);
	
	/* Ϊ�˷��������arm_cfft_f32����Ľ�����Աȣ����������4096��ģֵ��ʵ�ʺ���arm_rfft_fast_f32
	   ֻ������512��  
	*/ 
 	arm_cmplx_mag_f32(testOutput_f32, testOutputMag_f32, TEST_LENGTH_SAMPLES);
	
	
	/* ����Ƶ */
	PowerPhaseRadians_f32(testOutput_f32, Phase_f32, TEST_LENGTH_SAMPLES, 0.5f);
	
	
	for (size_t i = 0; i < TEST_LENGTH_SAMPLES / 2; i++)
	{
		testOutputMag_f32[i] = testOutputMag_f32[i] / TEST_LENGTH_SAMPLES * 2;
		testOutputMag_f32[i+(TEST_LENGTH_SAMPLES/2)] = FFT_SAMPLE_FREQUENCY * i / TEST_LENGTH_SAMPLES; // Ƶ�ʴ洢����4096/2����index + 4096/2 ����
	}
	
	
}
/**
 * @brief ���������õ�Ƶ��
 * @param index 
 * @retval float 
 * @date 2023-08-04
 */
float fft_index_get_frequnce(int index)
{
	//����λת��Ƶ��f = Fs*(0:(L/2)-1)/L;
	//LOG_DBG(TASKTAG, "fft max index:%d A:%f\n", index, testOutputMag_f32[index]);
	return (FFT_SAMPLE_FREQUENCY * index)/ TEST_LENGTH_SAMPLES;
}
/**
 * @brief �õ�����ֵ������
 * @param filterFreq �����ĵ���
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
// 			if (testOutputMag_f32[max] < testOutputMag_f32[i] && fft_index_get_frequnce(i) > 2000)//1000���µĲ�Ҫ
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
 * @brief �õ�����ֵ������
 * @param filterFreq �����ĵ���
 * @retval int 
 * @date 2023-08-04
 */
int fft_get_max_amplitude_index(int filterFreq)
{
	size_t i = filterFreq;
	int max = i++;
	
	for (; i < TEST_LENGTH_SAMPLES/2; i++)
	{
		if (testOutputMag_f32[max] < testOutputMag_f32[i] && fft_index_get_frequnce(i) > 2000)//2000���µĲ�Ҫ
			max = i;
	}
	return max;
}

