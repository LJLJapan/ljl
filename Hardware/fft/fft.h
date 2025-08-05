#ifndef __FFT_H__
#define __FFT_H__
#include "main.h"
#include "math.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#define TEST_LENGTH_SAMPLES 4096 
#define FFT_SAMPLE_FREQUENCY 15432

/* 输入和输出缓冲 */
extern  float32_t testOutput_f32[TEST_LENGTH_SAMPLES*2]; 
extern  float32_t testOutputMag_f32[TEST_LENGTH_SAMPLES*2]; //0,511幅值，512,1023频率
extern  float32_t testInput_f32[TEST_LENGTH_SAMPLES*2];
extern  float32_t Phase_f32[TEST_LENGTH_SAMPLES*2];


//   外部变量声明
void PowerPhaseRadians_f32(float32_t *_ptr, float32_t *_phase, uint16_t _usFFTPoints, float32_t _uiCmpValue);
void fft_arm_rfft_f32_app(const uint16_t *data);
float fft_index_get_frequnce(int index);
int fft_get_max_amplitude_index(int filterFreq);
//void fft_get_max_amplitude_index(int filterFreq,uint16_t* maxValue);
#endif

