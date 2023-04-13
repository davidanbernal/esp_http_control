/*
 * wifi_controller.h
 *
 *  Created on: Mar 8, 2023
 *      Author: david
 */

#ifndef MAIN_WIFI_CONTROLLER_H_
#define MAIN_WIFI_CONTROLLER_H_

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
//#include "wifi_controller.h"



static const char *TAGW = "WIFI";

static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
int count=0;
#define CONFIG_AP_WIFI_SSID      "prueba_esp32"
#define CONFIG_AP_WIFI_PASSWORD      "12345678"
#define CONFIG_AP_WIFI_CHANNEL   1
#define CONFIG_AP_MAX_STA_CONN 3

#define CONFIG_STA_WIFI_SSID      "wifipruebas"
#define CONFIG_STA_WIFI_PASSWORD      "xyz45678"

#define CONFIG_STA_CONNECT_TIMEOUT  3

int counter = 0;
/*
static void writex_nvs(const char* registro, int valor)
	{
	// Initialize NVS
							    esp_err_t err = nvs_flash_init();
							    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
							        // NVS partition was truncated and needs to be erased
							        // Retry nvs_flash_init
							        ESP_ERROR_CHECK(nvs_flash_erase());
							        err = nvs_flash_init();
							    }
							    ESP_ERROR_CHECK( err );

							nvs_handle_t my_handle;
							 err = nvs_open("storage", NVS_READWRITE, &my_handle);
							    if (err != ESP_OK) {
							        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
							        //ESP_LOGI(TAGW, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
							    } else {
							    	//ESP_LOGI(TAGW, "Done\n");
							        printf("Done\n");
							    }
						// Write
							   // ESP_LOGI(TAGW, "Updating restart counter in NVS ... ");
							    printf("Updating restart counter in NVS ... ");
						        counter=valor;
						        err = nvs_set_i32(my_handle, registro, counter);
						        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

						        // Commit written value.
						        // After setting any values, nvs_commit() must be called to ensure changes are written
						        // to flash storage. Implementations may write to storage at other times,
						        // but this is not guaranteed.
						       // ESP_LOGI(TAGW, "Committing updates in NVS ... \n");
						        printf("Committing updates in NVS ... \n");
						        err = nvs_commit(my_handle);
						        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

						        // Close
						        nvs_close(my_handle);
						       // printf("Restarting now.\n");
						           fflush(stdout);
						        //   esp_restart();
	}
static void event_handler(void* arg, esp_event_base_t event_base,
								int32_t event_id, void* event_data)
{

	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		ESP_LOGI(TAGW, "WIFI_EVENT_STA_DISCONNECTED");
		esp_wifi_connect();
		xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
		count++;
		ESP_LOGI(TAGW, "contadoer");//ESP_LOGI(TAGW, count);
		if(count==5)
		{
			count=0;
			ESP_LOGW(TAGW, "cambiando AP Mode");
			writex_nvs("modo",1);
			 esp_restart();
		}

	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ESP_LOGI(TAGW, "IP_EVENT_STA_GOT_IP");
		xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
	}
}

static void initialise_wifi(void)
{
	esp_log_level_set("wifi", ESP_LOG_WARN);
	static bool initialized = false;
	if (initialized) {
		return;
	}
	ESP_ERROR_CHECK(esp_netif_init());
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
	assert(ap_netif);
	esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
	assert(sta_netif);
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_LOGW(TAGW, "inciando wifi");
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &event_handler, NULL) );
	ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );

	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
	ESP_ERROR_CHECK( esp_wifi_start() );

	initialized = true;
}


static void wifi_ap(void)
{
	wifi_config_t wifi_config = { 0 };
	strcpy((char *)wifi_config.ap.ssid,CONFIG_AP_WIFI_SSID);
	strcpy((char *)wifi_config.ap.password, CONFIG_AP_WIFI_PASSWORD);
	wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
	wifi_config.ap.ssid_len = strlen(CONFIG_AP_WIFI_SSID);
	wifi_config.ap.max_connection = CONFIG_AP_MAX_STA_CONN;
	wifi_config.ap.channel = CONFIG_AP_WIFI_CHANNEL;

	if (strlen(CONFIG_AP_WIFI_PASSWORD) == 0) {
		wifi_config.ap.authmode = WIFI_AUTH_OPEN;
	}


	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );
	ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config) );
	ESP_ERROR_CHECK( esp_wifi_start() );
	ESP_LOGI(TAGW, "WIFI_MODE_AP started. SSID:%s password:%s channel:%d",
			 CONFIG_AP_WIFI_SSID, CONFIG_AP_WIFI_PASSWORD, CONFIG_AP_WIFI_CHANNEL);
	return ESP_OK;
}

static bool wifi_sta(int timeout_ms)
{
	wifi_config_t wifi_config = { 0 };
	strcpy((char *)wifi_config.sta.ssid, CONFIG_STA_WIFI_SSID);
	strcpy((char *)wifi_config.sta.password, CONFIG_STA_WIFI_PASSWORD);
	ESP_LOGW(TAGW, "sta funcion");
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_LOGW(TAGW, "sta funcion1");
	ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
	ESP_LOGW(TAGW, "sta funcion2");
	ESP_ERROR_CHECK( esp_wifi_connect() );
	ESP_LOGW(TAGW, "sta funcion3");
	int bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
								   pdFALSE, pdTRUE, timeout_ms / portTICK_PERIOD_MS);
	ESP_LOGI(TAGW, "bits=%x", bits);
	if (bits) {
		ESP_LOGI(TAGW, "WIFI_MODE_STA connected. SSID:%s password:%s",
			 CONFIG_STA_WIFI_SSID, CONFIG_STA_WIFI_PASSWORD);
		writex_nvs("modo",0);
	} else {
		ESP_LOGI(TAGW, "WIFI_MODE_STA can't connected. SSID:%s password:%s",
			 CONFIG_STA_WIFI_SSID, CONFIG_STA_WIFI_PASSWORD);
	}
	return (bits & CONNECTED_BIT) != 0;
}
static void readx_nvs(const char* registro)
	{
	// Initialize NVS
					    esp_err_t err = nvs_flash_init();
					    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
					        // NVS partition was truncated and needs to be erased
					        // Retry nvs_flash_init
					        ESP_ERROR_CHECK(nvs_flash_erase());
					        err = nvs_flash_init();
					    }
					    ESP_ERROR_CHECK( err );

					    // Open
					  	    printf("\n");
					  	 // ESP_LOGI(TAGW, "Opening Non-Volatile Storage (NVS) handle...");
					  	    printf("Opening Non-Volatile Storage (NVS) handle... ");

					  	    nvs_handle_t my_handle;
					  	    err = nvs_open("storage", NVS_READWRITE, &my_handle);
					  	    if (err != ESP_OK) {
					  	        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
					  	    //  ESP_LOGI(TAGW, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
					  	    } else {
					  	        printf("Done\n");
					  	   //   ESP_LOGI(TAGW, "Done\n");

					  	        // Read
					  	  //  ESP_LOGI(TAGW, "Reading restart counter from NVS ... ");
					  	        printf("Reading restart counter from NVS ... ");
					  	        	counter=0;//       int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
					  	        err = nvs_get_i32(my_handle, registro, &counter);
					  	        switch (err) {
					  	            case ESP_OK:
					  	            	//ESP_LOGI(TAGW, "Done\n");
					  	            	//ESP_LOGI(TAGW, "valor counter = %d\n", counter);
					  	                printf("Done\n");
					  	                printf("valor counter = %d\n", counter);
					  	                break;
					  	            case ESP_ERR_NVS_NOT_FOUND:
					  	                printf("The value is not initialized yet!\n");
					  	            //  ESP_LOGI(TAGW, "The value is not initialized yet!\n");
					  	                break;
					  	            default :
					  	               printf("Error (%s) reading!\n", esp_err_to_name(err));
					  	          //    ESP_LOGI(TAGW, "Error (%s) reading!\n", esp_err_to_name(err));
					  	        }



					  	        // Close
					  	        nvs_close(my_handle);
					  	    }

	}


static bool wifi_apsta(int timeout_ms)
{
	wifi_config_t ap_config = { 0 };
	strcpy((char *)ap_config.ap.ssid,CONFIG_AP_WIFI_SSID);
	strcpy((char *)ap_config.ap.password, CONFIG_AP_WIFI_PASSWORD);
	ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
	ap_config.ap.ssid_len = strlen(CONFIG_AP_WIFI_SSID);
	ap_config.ap.max_connection = CONFIG_AP_MAX_STA_CONN;
	ap_config.ap.channel = CONFIG_AP_WIFI_CHANNEL;

	if (strlen(CONFIG_AP_WIFI_PASSWORD) == 0) {
		ap_config.ap.authmode = WIFI_AUTH_OPEN;
	}

	wifi_config_t sta_config = { 0 };
	strcpy((char *)sta_config.sta.ssid, CONFIG_STA_WIFI_SSID);
	strcpy((char *)sta_config.sta.password, CONFIG_STA_WIFI_PASSWORD);


	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA) );
	ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config) );
	ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config) );
	ESP_ERROR_CHECK( esp_wifi_start() );
	ESP_LOGI(TAGW, "WIFI_MODE_AP started. SSID:%s password:%s channel:%d",
			 CONFIG_AP_WIFI_SSID, CONFIG_AP_WIFI_PASSWORD, CONFIG_AP_WIFI_CHANNEL);

	ESP_ERROR_CHECK( esp_wifi_connect() );
	int bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
								   pdFALSE, pdTRUE, timeout_ms / portTICK_PERIOD_MS);
	ESP_LOGI(TAGW, "bits=%x", bits);
	if (bits) {
		ESP_LOGI(TAGW, "WIFI_MODE_STA connected. SSID:%s password:%s",
			 CONFIG_STA_WIFI_SSID, CONFIG_STA_WIFI_PASSWORD);
	} else {
		ESP_LOGI(TAGW, "WIFI_MODE_STA can't connected. SSID:%s password:%s",
			 CONFIG_STA_WIFI_SSID, CONFIG_STA_WIFI_PASSWORD);
	}
	return (bits & CONNECTED_BIT) != 0;
}




*/

#endif /* MAIN_WIFI_CONTROLLER_H_ */
