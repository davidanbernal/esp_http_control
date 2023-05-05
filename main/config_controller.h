/*
 * config_controller.h
 *
 *  Created on: Mar 16, 2023
 *      Author: david
 */

#ifndef MAIN_CONFIG_CONTROLLER_H_
#define MAIN_CONFIG_CONTROLLER_H_

#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_log.h"
#include "string.h"
#include <stdlib.h>
#include "nvs_flash.h"
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>
#include "nvs_controller.h"
#include "serial_controller.h"
#include "json_controller.h"
#include "esp_spiffs.h"

static const char *TAG_STM_PRO = "spiffs";
static const char *TAGLOG = "EVENTLOG";
/*
char* concat2(const char *s1)
{
    char *s2="/n";
	char *result = malloc(strlen(s1) + 3); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
*/
void mac_address(void){
	uint8_t base_mac_addr [6] = {0};
	esp_err_t ret = ESP_OK;
	//Get base MAC address from EFUSE BLK0(default option)
		ret = esp_efuse_mac_get_default(base_mac_addr);
		if (ret != ESP_OK) {
			ESP_LOGE("MAC", "Failed to get base MAC address from EFUSE BLK0. (%s)", esp_err_to_name(ret));
			ESP_LOGE("MAC", "Aborting");
			abort();
		}
		//Set the base MAC address using the retrieved MAC address
	   //     ESP_LOGI("MAC", "Using \"0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\" as base MAC address",
		 //            base_mac_addr[0], base_mac_addr[1], base_mac_addr[2], base_mac_addr[3], base_mac_addr[4], base_mac_addr[5]);

		char s[10] = {0};
			int n = 0;
		for (int i = 0; i < 6; i++) {
				n += sprintf (&s[n], "%x", base_mac_addr[i]);
			}
	grabar_nvs("8_MACADRRWIFI",s);
}

void initSPIFFS(void)
{
    ESP_LOGI(TAG_STM_PRO, "%s", "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf =
        {
            .base_path = "/spiffs",
            .partition_label = NULL,
            .max_files = 5,
            .format_if_mount_failed = true};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG_STM_PRO, "%s", "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG_STM_PRO, "%s", "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG_STM_PRO, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

/*  // Formatting SPIFFS - Use only for debugging
    if (esp_spiffs_format(NULL) != ESP_OK)
    {
        ESP_LOGE(TAG_STM_PRO, "%s", "Failed to format SPIFFS");
        return;
    }
*/

    size_t total, used;
    if (esp_spiffs_info(NULL, &total, &used) == ESP_OK)
    {
        ESP_LOGI(TAG_STM_PRO, "Partition size: total: %d, used: %d", total, used);
    }
}

void readlog(void){

	FILE *file = fopen("/spiffs/log.txt", "r");
	  if(file ==NULL)
	  {
	    ESP_LOGE(TAGLOG,"File does not exist!");
	  }
	  else
	  {
		  ESP_LOGE(TAGLOG,"File  exist!");
	    char line[256];
	    while(fgets(line, sizeof(line), file) != NULL)
	    {
	    printf(line);
	    }
	    fclose(file);
	  }
}

void writelog(char *text){
	 /*Create file with name hello.txt */
	    ESP_LOGE(TAGLOG, "Append New register in: log.txt");
	    FILE *f = fopen("/spiffs/log.txt", "a");
	    if (f == NULL)
	    {
	        ESP_LOGE(TAGLOG, "Failed to open file for writing");
	        return;
	    }

	    fprintf(f, concat(text,"\n"));  // write data to hello.txt file
	    fclose(f);
	   // ESP_LOGI(TAG, "File written");
}

void clearlog(void){
	 /*Create file with name hello.txt */
	    ESP_LOGE(TAGLOG, "Cleaning file: log.txt");
	    FILE *f = fopen("/spiffs/log.txt", "w");
	    if (f == NULL)
	    {
	        ESP_LOGE(TAGLOG, "Failed to open file for writing");
	        return;
	    }

	    ESP_LOGE(TAGLOG, "Writing data to file: log.txt");
	    fprintf(f, "LOG DE EVENTOS ESP32\n");  // write data to hello.txt file
	    fclose(f);
	  //  ESP_LOGI(TAG, "File written");
}

#endif /* MAIN_CONFIG_CONTROLLER_H_ */
