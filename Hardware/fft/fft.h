#ifndef __FFT_H__
#define __FFT_H__
#include "main.h"
#include "math.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#define TEST_LENGTH_SAMPLES 4096 
#define FFT_SAMPLE_FREQUENCY 15432

/* ������������ */
extern  float32_t testOutput_f32[TEST_LENGTH_SAMPLES*2]; 
extern  float32_t testOutputMag_f32[TEST_LENGTH_SAMPLES*2]; //0,511��ֵ��512,1023Ƶ��
extern  float32_t testInput_f32[TEST_LENGTH_SAMPLES*2];
extern  float32_t Phase_f32[TEST_LENGTH_SAMPLES*2];


//   �ⲿ��������
void PowerPhaseRadians_f32(float32_t *_ptr, float32_t *_phase, uint16_t _usFFTPoints, float32_t _uiCmpValue);
void fft_arm_rfft_f32_app(const uint16_t *data);
float fft_index_get_frequnce(int index);
int fft_get_max_amplitude_index(int filterFreq);
//void fft_get_max_amplitude_index(int filterFreq,uint16_t* maxValue);
#endif

