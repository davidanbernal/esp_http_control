/*
 * http_client_controller.h
 *
 *  Created on: Mar 8, 2023
 *      Author: david
 */

#ifndef MAIN_HTTP_CLIENT_CONTROLLER_H_
#define MAIN_HTTP_CLIENT_CONTROLLER_H_


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"
#include "esp_tls.h"
#include "cJSON.h"
#include "serial_controller.h"
#include "nvs_controller.h"
#include "local_http_server.h"
#include <esp_http_server.h>
#include "esp_http_client.h"

#define PANIC_PIN GPIO_NUM_19
#define ALARM_PIN GPIO_NUM_21
#define MAX_HTTP_RECV_BUFFER 512*2
#define MAX_HTTP_OUTPUT_BUFFER 2048
static const char *TAG = "HTTP_CLIENT";

int estado=0;
const char *token=NULL;
char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
char local_response_buffer2[MAX_HTTP_OUTPUT_BUFFER] = {0};


esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
    }
    return ESP_OK;
}

void httptrama(int tipo){
	//ESP_LOGI(TAG, "Entre a trama");

	estado=0;
	//int timetemp=atoi(leer_nvs("4_TIMEOUT-HTTP"));
	esp_http_client_config_t config = {

			.url= leer_nvs("4_URL-TRAMA"),
			//.url= "https://192.168.1.106:1880/trama",
			.method = HTTP_METHOD_POST,
			.event_handler = _http_event_handler,
			.user_data = local_response_buffer2,        // Pass address of local buffer to get response
			.disable_auto_redirect = false,
			.timeout_ms=atoi(leer_nvs("4_TIMEOUT-HTTP"))
		};
		esp_http_client_handle_t client = esp_http_client_init(&config);



		// POST
	   const char  *post_data=NULL;
		if(tipo==0){ post_data =trama_sensors();}
		else if(tipo==1){ post_data =trama_panic();}
		else if(tipo==2){ post_data =trama_alarma();}
		else post_data ="NO DATA";

	   printf("json: %s\n", post_data);
	   esp_http_client_set_header(client, "Content-Type", "application/json");
	   const char *datosnvs=leer_nvs("4_TOKEN-TRAMA");
	   esp_http_client_set_header(client, "x-access-token",datosnvs );



	  esp_http_client_set_post_field(client, post_data, strlen(post_data));
	  esp_err_t err = esp_http_client_perform(client);
	  if (err == ESP_OK) {
			ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
					esp_http_client_get_status_code(client),
					esp_http_client_get_content_length(client));
		} else {
			ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
		}

		printf("res envio trama: %s\n", local_response_buffer2);
	 esp_http_client_cleanup(client);
}
static void http_rest_with_url(void)
{

	//ESP_LOGI(TAG, "entre a login");

    /**
     * NOTE: All the configuration parameters for http_client must be spefied either in URL or as host and path parameters.
     * If host and path parameters are not set, query parameter will be ignored. In such cases,
     * query parameter should be specified in URL.
     *
     * If URL as well as host and path parameters are specified, values of host and path will be considered.
     */
    esp_http_client_config_t config = {

		.url= leer_nvs("4_URL-LOGIN"),
		.method = HTTP_METHOD_POST,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
        .disable_auto_redirect = false,
		.timeout_ms=atoi(leer_nvs("4_TIMEOUT-HTTP"))
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // POST
    const char *post_data = "{\"field1\":\"value1\"}";
   //const char *post_data = "{\"Authorization\":\"Basic SUFtaW5pLTAxOjAxMEEwMDAwODExMmNmYTQzNjMwMzQzNzMzYTU1MTQ4IA\"}";
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Authorization", leer_nvs("4_TOKEN-LOGIN"));
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

     esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }


	cJSON *root2 = cJSON_Parse(local_response_buffer);
	int status = cJSON_GetObjectItem(root2,"status")->valueint;
	char *message = cJSON_GetObjectItem(root2,"msg")->valuestring;
	//ESP_LOGI(TAG, "mensajes= %s",message);
	if(status==200)//login correcto
	  {
		cJSON *body_response = cJSON_GetObjectItem(root2, "body");
		token = cJSON_GetObjectItem(body_response, "token")->valuestring;
		grabar_nvs("4_TOKEN-TRAMA", token);
		//ESP_LOGI(TAG, "token= %s",token);
		//ESP_LOGI(TAG, "termine respuesta de login ok");
		estado=1;

	  }
	if(status==400 || status==401)
	   {
		char message = cJSON_GetObjectItem(root2,"msg")->valueint;
		ESP_LOGI(TAG, "message= %d",message);
	   estado=0;
	   }

	cJSON_Delete(root2);


    esp_http_client_cleanup(client);
   // free(local_response_buffer);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
 if(estado==1)
 {httptrama(0);}


}

static void http_client_task(void *pvParameters)
{
	while(1){
    http_rest_with_url();
    vTaskDelay(10000 / portTICK_PERIOD_MS);
	}

    ESP_LOGI(TAG, "Finish http example");
    vTaskDelete(NULL);
}

static void panic_button_task(void *pvParameters)
{
	while(1){
	uint8_t level = gpio_get_level(PANIC_PIN);
	if (level == 0){
	    	ESP_LOGW("PANIC", "El boton se ha activado");
	    	httptrama(1);
	    	vTaskDelay(2000 / portTICK_PERIOD_MS);

	    	}
	vTaskDelay(100 / portTICK_PERIOD_MS);

	}
    ESP_LOGI(TAG, "Finish task panic");
    vTaskDelete(NULL);
}
static void alarm_button_task(void *pvParameters)
{
	while(1){
	uint8_t level = gpio_get_level(ALARM_PIN);
	if (level == 0){
	    	ESP_LOGW("ALARMA", "La alarma se ha activado");
	    	httptrama(2);
	    	vTaskDelay(2000 / portTICK_PERIOD_MS);

	    	}
	vTaskDelay(100 / portTICK_PERIOD_MS);

	}
    ESP_LOGI(TAG, "Finish task alarm");
    vTaskDelete(NULL);
}

#endif /* MAIN_HTTP_CLIENT_CONTROLLER_H_ */
