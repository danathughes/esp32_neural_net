/* tcp_perf Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


/*
tcp_perf example

Using this example to test tcp throughput performance.
esp<->esp or esp<->ap

step1:
    init wifi as AP/STA using config SSID/PASSWORD.

step2:
    create a tcp server/client socket using config PORT/(IP).
    if server: wating for connect.
    if client connect to server.
step3:
    send/receive data to/from each other.
    if the tcp connect established. esp will send or receive data.
    you can see the info in serial output.
*/

#include <stdio.h>

#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "tcp_perf.h"
#include "adc_collector.h"
#include "neural_net.h"



//this task establish a TCP connection and receive data from TCP
static void tcp_conn(void *pvParameters)
{
    while (1) {

        g_rxtx_need_restart = false;

        ESP_LOGI(TAG, "task tcp_conn.");

        /*wating for connecting to AP*/
        xEventGroupWaitBits(tcp_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);

        ESP_LOGI(TAG, "sta has connected to ap.");

        int socket_ret = ESP_FAIL;

        TaskHandle_t tx_rx_task = NULL;

#if EXAMPLE_ESP_TCP_MODE_SERVER
        if (socket_ret == ESP_FAIL) {
            /*create tcp socket*/
            ESP_LOGI(TAG, "tcp_server will start after 3s...");
            vTaskDelay(3000 / portTICK_RATE_MS);
            ESP_LOGI(TAG, "create_tcp_server.");
            socket_ret = create_tcp_server();
        }
#else /*EXAMPLE_ESP_TCP_MODE_SERVER*/
        if (socket_ret == ESP_FAIL) {
            ESP_LOGI(TAG, "tcp_client will start after 20s...");
            vTaskDelay(20000 / portTICK_RATE_MS);
            ESP_LOGI(TAG, "create_tcp_client.");
            socket_ret = create_tcp_client();
        }
#endif
        if (socket_ret == ESP_FAIL) {
            ESP_LOGI(TAG, "create tcp socket error,stop.");
            continue;
        }

        /*create a task to tx/rx data*/

        if (tx_rx_task == NULL) {
            if (pdPASS != xTaskCreate(&send_data, "send_data", 4096, NULL, 4, &tx_rx_task)) {
                ESP_LOGE(TAG, "Send task create fail!");
            }
        }

        while (1) {
            g_total_data = 0;
            vTaskDelay(3000 / portTICK_RATE_MS);//every 3s

            if (g_rxtx_need_restart) {
                printf("send or receive task encoutner error, need to restart\n");
                break;
            }
        }

        close_socket();
    }

    vTaskDelete(NULL);
}

void buffer_monitor_task(void *pvParameter)
{
    while(1)
    {
        if(buffer_full)
        {
//            ESP_LOGI("buffer_monitor", "Buffer is full");
            neural_net_forward();
/*
            printf("Terrain PDF:\n");

            for(int i=0; i<NUM_TERRAINS; i++)
            {
                printf("  %d: %f\n", i, terrain_pdf[i]);
            }
*/
            window_idx = 0;
            buff_idx = 0;
            buffer_full = false;
            window_full = false;
            tx_terrain_ready = true;            
        }

        vTaskDelay(1);
    }
}

void app_main(void)
{
    
    timer_click_count = 0;
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    
#if EXAMPLE_ESP_WIFI_MODE_AP
    ESP_LOGI(TAG, "EXAMPLE_ESP_WIFI_MODE_AP");
    wifi_init_softap();
#else
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
#endif /*EXAMPLE_ESP_WIFI_MODE_AP*/
    

//    xTaskCreate(&buffer_monitor_task, "buffer_monitor", 2048, NULL, 5, NULL);
//    xTaskCreate(&tcp_conn, "tcp_conn", 4096, NULL, 5, NULL);

    init_adcs();
    init_buffer();
 //   init_timer(500);
    init_window();

    int xx = 0;

    for(int i=0; i<200; i++)
    {
        for(int j=0; j<4; j++)
        {
            window[i][j]=xx;
            xx++;
        }
    }

// TEST TO SEE HOW FAST A FORWARD PASS OF A NEURAL NET CAN GO
    printf("Forward pass on Neural Net...\n");
    TickType_t start = xTaskGetTickCount();
    neural_net_forward();
    printf("Done.\n");
    TickType_t end = xTaskGetTickCount();

    uint32_t total_ticks = (uint32_t) end - start;

    printf("Total Ticks: %d\n", total_ticks);

    printf("Restarting.\n\n\n");
/*
    for(int i=0; i<181; i++)
    {
        for(int j=0; j<12; j++)
        {
            printf("%f,", h_0[i][j]);
        }
        printf("\n");
    }
*/
}
