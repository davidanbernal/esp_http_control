/* ESP HTTP Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"
#include "esp_tls.h"
#include "cJSON.h"
#include "serial_controller.h"
#include "config_initial.h"
#include "nvs_controller.h"
#include "config_controller.h"
#include "wifi_controller.h"
#include "local_http_server.h"
#include "http_client_controller.h"
#include "file_server_controller.h"
#include <esp_http_server.h>
#include "esp_http_client.h"
//#include "STM_FLASH_CONTROLLER/stm_flash/include/stm_flash.h"
//#include "STM_FLASH_CONTROLLER/stm_pro_mode/include/stm_pro_mode.h"



void init_serial_coms(void) {
	const uart_config_t uart_config_for_flash = {
	   .baud_rate = 115200,
	   .data_bits = UART_DATA_8_BITS,
	   .parity = UART_PARITY_EVEN,
	   .stop_bits = UART_STOP_BITS_1,
	   .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config_for_flash);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}



void app_main(void)
{
	vTaskDelay(5000 / portTICK_PERIOD_MS);// PARA SOFTWARE RESET ESP

	static httpd_handle_t server = NULL;
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    initSPIFFS();

     gpio_set_direction(PANIC_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PANIC_PIN, GPIO_PULLUP_ONLY);
    gpio_set_direction(ALARM_PIN, GPIO_MODE_INPUT);//ELIMINAR CUANDO SE REALICE LOGICA DE ALARMAS
    gpio_set_pull_mode(ALARM_PIN, GPIO_PULLUP_ONLY);//ELIMINAR CUANDO SE REALICE LOGICA DE ALARMAS
    init_serial_coms();
    uart_param_config(UART_NUM_1, &uart_config_for_protocol);

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    /*
--------------------MODO DE COMUNICACION------------
   	initialise_wifi();
	readx_nvs("modo");

//	ESP_LOGW(TAG, "COLOCA STA Mode");
//	xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
//	wifi_sta(CONFIG_STA_CONNECT_TIMEOUT*1000);

if(counter==0)
	{
	ESP_LOGW(TAG, "Start STA Mode");
//	xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
	wifi_sta(CONFIG_STA_CONNECT_TIMEOUT*5000);
	}
else//(counter==1)
	{

	ESP_LOGW(TAG, "Start AP Mode");
	//xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
		wifi_ap();
	}
	*/
    ESP_ERROR_CHECK(example_connect());
    ESP_LOGI(TAG, "Connected to AP, begin http example");
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));



    	esp_err_t result_ahsn;
    	char * 	first_boot = readx_nvs("7_STATUS-MATR");
    	if(strcmp(first_boot, "1") != 0){
    	   ESP_LOGW("ERASE_FLASH_MEMORY", "Erasing flash memory");
    	   ESP_ERROR_CHECK(nvs_flash_erase());

    	   ESP_LOGW("ERASE_FLASH_MEMORY", "Requesting and storing AHSN");
    	   vTaskDelay(500 / portTICK_PERIOD_MS);

    	   result_ahsn = request_and_store_ahsn();

    	   if(result_ahsn != ESP_OK){
    		   ESP_LOGW("ERASE_FLASH_MEMORY", "Request and storing of AHSN has failed");
    	   }
    	   else{
    		   valores_iniciales();
    		   server = start_webserver();//Server http
    	   }
    	}
    	else{
    	   ESP_LOGW("INCIO POSTERIOR", "Cargando tareas");
    	   leer_nvs("7_USER_ID");
    	   leer_nvs("7_AHSN_BASE64");
    		load_keys_with_port_assigned_to_RAM();

    	    synchronize_serial_coms_group = xEventGroupCreate();
    	    uart_queue = xQueueCreate(1, sizeof(uint8_t*));
    	    server = start_webserver();//Server http
    	    xTaskCreatePinnedToCore(UART_rx_task, "uart_rx_task", 1024*3, NULL, configMAX_PRIORITIES, NULL, 1); //Task for receiving data
    	    xTaskCreatePinnedToCore(UART_tx_task, "uart_tx_task", 1024*3, NULL, configMAX_PRIORITIES, NULL, 1); //Task for transmitting data7
    	    xTaskCreatePinnedToCore(&panic_button_task, "panic_button_task", 2048*2, NULL, 9, NULL,1); //Task panic button
    	    xTaskCreatePinnedToCore(&alarm_button_task, "alarm_button_task", 2048*2, NULL, 9, NULL,1);//Task alarms
    	    vTaskDelay(10000 / portTICK_PERIOD_MS);
    	    xTaskCreatePinnedToCore(&http_client_task, "http_client_task", 8192*6, NULL, 10, NULL,0);//Task  http client token login
    	//    xTaskCreatePinnedToCore(&webbuff_task, "webbuff_task", 8192*4, NULL, 10, NULL,0);//Task  http client token login
    	   // xTaskCreatePinnedToCore(&httpcl_request_task, "httpcl_request_task", 8192*2, NULL, 10, NULL,0);//Task http client request data

    	     }







}
