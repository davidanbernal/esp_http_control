/*
 * matricular_keys.h
 *
 *  Created on: Mar 28, 2023
 *      Author: david
 */

#ifndef MAIN_CONFIG_INITIAL_H_
#define MAIN_CONFIG_INITIAL_H_

#include "local_http_server.h"
#include "nvs_controller.h"
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"

#define REQUEST_STM_UID 2
#define SIZE_OF_AHSN 34
#define BASE64_OUTPUT_SIZE 65

void valores_iniciales(void){
	grabar_nvs("4_SERVER-TIME","10");
	grabar_nvs("1_TRAMA-TIME","500");
	grabar_nvs("2_WIFI-AP-PASS","ivam45678");
	grabar_nvs("2_WIFI-AP-IP","124.213.16.29");
	grabar_nvs("2_WIFI-ST-SSID","WIFI_PRUEBAS");
	grabar_nvs("2_WIFI-ST-PASS","12345678");
	grabar_nvs("2_WIFI-ST-INTE","3");
	grabar_nvs("4_URL-TRAMA","https://atsdev.actiontracker.es/ivtramas/api-tramas/api/trama/saveFromESP32");
	grabar_nvs("4_URL-LOGIN","https://atsdev.actiontracker.es/api-ats/authDevice/login");
	grabar_nvs("4_URL-REG","https://atsdev.actiontracker.es/api-ats/device2/create");
	grabar_nvs("4_TOKEN-LOGIN","Basic SUFtaW5pLTAxOjAxMEEwMDAwODExMmNmYTQzNjMwMzQzNzMzYTU1MTQ4IA");
	grabar_nvs("4_TIMEOUT-HTTP","8000");
	grabar_nvs("4_TOKEN-TIME","12");
	grabar_nvs("5_TOK-HTTPSER","dslfkjasdkhf732489032l;ksd");
}

void encrypt_any_length_string(const char *input, uint8_t *key, uint8_t *iv,
                               char *base64_output, size_t *base64_len) {
    int padded_input_len = 0;
    int input_len = strlen(input) + 1;
    int modulo16 = input_len % 16;

    if (input_len < 16)
        padded_input_len = 16;
    else
        padded_input_len = (strlen(input) / 16 + 1) * 16;

    char *padded_input = (char *)malloc(padded_input_len);
    if (!padded_input) {
        printf("Failed to allocate memory\n");
        return;
    }
    memcpy(padded_input, input, strlen(input));
    uint8_t pkc5_value = (17 - modulo16);
    for (int i = strlen(input); i < padded_input_len; i++) {
        padded_input[i] = pkc5_value;
    }

    unsigned char *encrypt_output = (unsigned char *)malloc(padded_input_len);

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, key, 256);
    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, padded_input_len, iv,
                          (unsigned char *)padded_input, encrypt_output);

    // encode the encrypted output in base64
    mbedtls_base64_encode((unsigned char *)base64_output, *base64_len,
                          base64_len, encrypt_output, padded_input_len);

//    ESP_LOG_BUFFER_HEX("cbc_encrypt_any_length", encrypt_output, padded_input_len);
//    printf("base64 output: %s\n", base64_output);

    free(padded_input);
    free(encrypt_output);
}

static esp_err_t request_and_store_ahsn(){
    static const char *AHSN_TAG = "REQUEST_AND_STORE_AHSN";
    esp_log_level_set(AHSN_TAG, ESP_LOG_INFO);
    const uint8_t request_id[7] = {REQUEST_STM_UID, 0, 0, 0, 0, 0, 0};
    //La primera solicitud UART siempre responde con datos invalidos
    int txBytes = uart_write_bytes(UART_NUM_1, request_id, 7);
    ESP_LOGI("Request_ID", "Wrote %d bytes", txBytes);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 3000 / portTICK_RATE_MS);
    //La segunda solicitud UART es la valida
    txBytes = uart_write_bytes(UART_NUM_1, request_id, 7);
    ESP_LOGI("Request_ID", "Wrote %d bytes", txBytes);
    rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 3000 / portTICK_RATE_MS);
    if (rxBytes > 0) {
    	data[rxBytes] = 0;
    	ESP_LOGI("REQUEST_AND_STORE_AHSN", "This is data received from STM32: %s", (char *) data);
        char data_as_string[60];
        char * stm32_id = malloc(SIZE_OF_AHSN);
        sprintf(data_as_string, "%s", data);

        if (strstr(data_as_string, "p") == NULL){
        	ESP_LOGE(AHSN_TAG, "Invalid data format: could not find 'p' delimiter");
            goto cleanup;  // jump to the cleanup section of the code
        }
        if (strstr(data_as_string, "-") == NULL){
        	ESP_LOGE(AHSN_TAG, "Invalid data format: could not find '-' delimiter");
            goto cleanup;  // jump to the cleanup section of the code
        }
        if (strstr(data_as_string, "d") == NULL){
        	ESP_LOGE(AHSN_TAG, "Invalid data format: could not find 'g' delimiter");
            goto cleanup;  // jump to the cleanup section of the code
        }
        if (strstr(data_as_string, "m") == NULL){
        	ESP_LOGE(AHSN_TAG, "Invalid data format: could not find 'm' delimiter");
            goto cleanup;  // jump to the cleanup section of the code
        }

        stm32_id = strtok(data_as_string, "d");

        stm32_id = strtok(NULL, "m");

        if(strlen(stm32_id) != 16){
        	ESP_LOGE(AHSN_TAG, "Invalid data format: there aren't 8 bytes of data");
        	goto cleanup;
        }

    	ESP_LOGI(AHSN_TAG, "This is the number %s", stm32_id);

        unsigned char mac[6] = {0};
        char mac_esp[24];

        // Get MAC address
        esp_efuse_mac_get_default(mac);
        esp_read_mac(mac, ESP_MAC_WIFI_STA);

        // Convert MAC address to string
        sprintf(mac_esp, "%02hhx%02hhx%02hhx%02hhx", mac[2], mac[3], mac[4], mac[5]);
        ESP_LOGI(AHSN_TAG, "MAC str is %s", mac_esp);

        strcat(mac_esp, stm32_id);

        char * ahsn = malloc(33*sizeof(char));

        strcpy(ahsn, ":020a0000"); //Informacion por defecto de ActionTracker
        strcat(ahsn, mac_esp);

        writex_nvs("7_SERIAL_ID", ahsn);
        ESP_LOGI(AHSN_TAG, "This is the number after mac esp %s", ahsn);
        char enc_key[32] = "@AjH39zYz!by3Y#Aw4v%n8KZssaMpETT";
        char enc_iv[16] = "Bx3K6U3@^955ZN5G";

        char * encrypted_ahsn = (char *) malloc(BASE64_OUTPUT_SIZE);
        size_t base64_len = BASE64_OUTPUT_SIZE;

        encrypt_any_length_string(ahsn, (uint8_t *)enc_key, (uint8_t *)enc_iv, encrypted_ahsn, &base64_len);
        ESP_LOGI(AHSN_TAG, "base64 output: %s\n", encrypted_ahsn);

        char * ahsn_base64 = (char *) malloc(BASE64_OUTPUT_SIZE);
        size_t *olen = (size_t *) malloc(BASE64_OUTPUT_SIZE);
        mbedtls_base64_encode((unsigned char *)ahsn_base64, base64_len, olen, (const unsigned char *) ahsn, strlen(ahsn));
        ESP_LOGI(AHSN_TAG, "AHSN BASE64 output: %s\n", ahsn_base64);

        writex_nvs("7_USER_ID", encrypted_ahsn);
        writex_nvs("7_AHSN_BASE64", ahsn_base64);
        free(encrypted_ahsn);
        free(olen);
        free(ahsn_base64);
       // writex_nvs("7_STATUS-MATR", "1");
        return ESP_OK;

        cleanup:
			writex_nvs("7_STATUS-MATR", "0");
			return ESP_FAIL;
    }
    else{
    	ESP_LOGI(AHSN_TAG, "No response from UART");
    	writex_nvs("7_STATUS-MATR", "0");
    	return ESP_FAIL;
    }
}


#endif /* MAIN_CONFIG_INITIAL_H_ */
