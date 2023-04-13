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




#endif /* MAIN_CONFIG_CONTROLLER_H_ */
