#ifndef __ADC_COLLECTOR_H__
#define __ADC_COLLECTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#define WINDOW_SIZE		4000
#define NUM_ADC			4
#define BUFFER_SIZE		WINDOW_SIZE*NUM_ADC

#define V_REF			1100
#define V_REF_CHANNEL	GPIO_NUM_35

#include "esp_adc_cal.h"

volatile int window_idx;
volatile bool window_full;

char* databuf;
uint16_t* window;

static intr_handle_t s_timer_handle;

volatile uint32_t timer_click_count;

esp_adc_cal_characteristics_t adc_characteristics;

void init_timer(int timer_period_us);
void init_buffer();
void init_adcs();
void measure_adcs();

#endif