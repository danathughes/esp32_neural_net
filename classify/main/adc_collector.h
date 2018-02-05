#ifndef __ADC_COLLECTOR_H__
#define __ADC_COLLECTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#define WINDOW_SIZE		200
#define NUM_ADC			4
#define NUM_ADC_BYTES	2
#define BUFFER_SIZE		2000
#define NUM_TERRAINS	4

#define V_REF			1100

#define WRITE_TO_DATABUFFER	false


#include "esp_adc_cal.h"

volatile uint16_t window[WINDOW_SIZE][NUM_ADC];

volatile int window_idx;
volatile bool window_full;
volatile bool tx_terrain_ready;

char* terrain_buffer;
float* terrain_pdf;
char* databuf;

volatile int buff_idx;
volatile bool buffer_full;

static intr_handle_t s_timer_handle;

volatile uint32_t timer_click_count;

esp_adc_cal_characteristics_t adc_characteristics;

void init_timer(int timer_period_us);
void init_window();
void init_buffer();
void init_adcs();
void measure_adcs();

#endif