#include <stddef.h>
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "driver/timer.h"
#include "esp_log.h"
#include "esp_err.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"

#include "esp_adc_cal.h"

#include "driver/gpio.h"
#include "driver/adc.h"

#include "adc_collector.h"


// The data buffer will consist of 8 bytes of data for each entry in the buffer (4 sensors, 16-bit values)
// The data is stored ADC0, ADC1, ADC2, ADC3
#define ADC_0	(ADC1_CHANNEL_0)
#define ADC_1 	(ADC1_CHANNEL_1)
#define ADC_2	(ADC1_CHANNEL_2)
#define ADC_3	(ADC1_CHANNEL_3)


static void timer_isr(void* arg)
{
	TIMERG0.int_clr_timers.t0 = 1;
	TIMERG0.hw_timer[0].config.alarm_en = 1;
	
	// code which runs in the interrupt	
	if(!buffer_full)
	{
		measure_adcs();
	}
}

/*
* init_timer(int timer_period_us)
*
* Initialize the interrupt timer.
*
* timer_period_us - Number of microseconds between interrupts
*
* See https://esp32.com/viewtopic.php?f=13&t=1735
*/

void init_timer(int timer_period_us)
{
	timer_config_t config = {
		.alarm_en = true,
		.counter_en = false,
		.intr_type = TIMER_INTR_LEVEL,
		.counter_dir = TIMER_COUNT_UP,
		.auto_reload = true,
		.divider = 80			/* 1 us per tic */
	};

	timer_init(TIMER_GROUP_0, TIMER_0, &config);
	timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
	timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, timer_period_us);
	timer_enable_intr(TIMER_GROUP_0, TIMER_0);
	timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer_isr, NULL, 0, &s_timer_handle);

	timer_start(TIMER_GROUP_0, TIMER_0);
}


void init_buffer()
{
	databuf = (char*) malloc (BUFFER_SIZE * NUM_ADC * NUM_ADC_BYTES * sizeof(char));
	buff_idx = 0;
	buffer_full = false;
}

void init_window()
{
	terrain_buffer = (char*) malloc(NUM_TERRAINS * sizeof(float));
	terrain_pdf = (float*) terrain_buffer;

	for(int i=0; i<WINDOW_SIZE; i++)
	{
		for(int j=0; j<NUM_ADC; j++)
		{
			window[i][j] = 0;
		}
	}
	window_idx = 0;
	window_full = false;
	tx_terrain_ready = false;
}

void init_adcs()
{
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC_0, ADC_ATTEN_DB_11);
	adc1_config_channel_atten(ADC_1, ADC_ATTEN_DB_11);
	adc1_config_channel_atten(ADC_2, ADC_ATTEN_DB_11);
	adc1_config_channel_atten(ADC_3, ADC_ATTEN_DB_11);

	esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, &adc_characteristics);
}


void write_to_buffer(uint32_t adc0_val, uint32_t adc1_val, uint32_t adc2_val, uint32_t adc3_val)
{
	// Convert to individual bytes and write to the databuffer
	databuf[buff_idx] = (char) adc0_val >> 8;
	databuf[buff_idx+1] = (char) adc0_val;
	databuf[buff_idx+2] = (char) adc1_val >> 8;
	databuf[buff_idx+3] = (char) adc1_val;
	databuf[buff_idx+4] = (char) adc2_val >> 8;
	databuf[buff_idx+5] = (char) adc2_val;
	databuf[buff_idx+6] = (char) adc3_val >> 8;
	databuf[buff_idx+7] = (char) adc3_val;

	buff_idx += NUM_ADC * NUM_ADC_BYTES;

	// Has the buffer been filled?
	if(buff_idx >= BUFFER_SIZE * NUM_ADC * NUM_ADC_BYTES)
	{
		buffer_full = true;
		//printf("Buffer Full!\n");
	}	
}


void measure_adcs()
{
	uint32_t adc0_val, adc1_val, adc2_val, adc3_val;

	// Measure the ADCs
	adc0_val = adc1_to_voltage(ADC_0, &adc_characteristics);
	adc1_val = adc1_to_voltage(ADC_1, &adc_characteristics);
	adc2_val = adc1_to_voltage(ADC_2, &adc_characteristics);
	adc3_val = adc1_to_voltage(ADC_3, &adc_characteristics);

	if(WRITE_TO_DATABUFFER)
	{
		write_to_buffer(adc0_val, adc1_val, adc2_val, adc3_val);
	}

	// Write to the meaurement window
	window[window_idx][0] = (uint16_t) adc0_val;
	window[window_idx][1] = (uint16_t) adc1_val;
	window[window_idx][2] = (uint16_t) adc2_val;
	window[window_idx][3] = (uint16_t) adc3_val;

	window_idx += 1;
	if(window_idx >= WINDOW_SIZE)
	{
		window_full = true;
		buffer_full = true;
	}
	if(window_idx == WINDOW_SIZE/2)
	{
		window_full = true;
		buffer_full = true;
	}
/*	if(window_idx == WINDOW_SIZE/4)
	{
		window_full = true;
		buffer_full = true;
	}
	if(window_idx == 3*WINDOW_SIZE/4)
	{
		window_full = true;
		buffer_full = true;
	}
*/
}


