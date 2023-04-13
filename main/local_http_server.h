/*
 * local_http_server.h
 *
 *  Created on: Mar 8, 2023
 *      Author: david
 */

#ifndef MAIN_LOCAL_HTTP_SERVER_H_
#define MAIN_LOCAL_HTTP_SERVER_H_

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
#include "config_initial.h"
#include "nvs_controller.h"
#include "serial_controller.h"
#include "json_controller.h"


static const char *TAGL = "HTTP_SERVER";
const char *token_httser="";
char *modelav="020A0000";



char* concat2(const char *s1, const char *s2)
	{
	    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
	    // in real code you would check for errors in malloc here
	    strcpy(result, s1);
	    strcat(result, s2);
	    return result;
	}


/* An HTTP POST handler */
static esp_err_t echo_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;
  ///  ESP_LOGI(TAGL, "qty %d\n", remaining);
    if(remaining <= 0){
    	ESP_LOGI(TAGL, "=========== handler ==========");
    }

    else{

    while (remaining >= 1) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            ESP_LOGI(TAGL, "=========== NO RECIBI ==========");
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAGL, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAGL, "%.*s", ret, buf);
        ESP_LOGI(TAGL, "trama: %s\n", buf);
        ESP_LOGI(TAGL, "====================================");


    /*    cJSON *root2 = cJSON_Parse(local_response_buffer);
        int status = cJSON_GetObjectItem(root2,"status")->valueint;
        ESP_LOGI(TAG, "status= %d",status);     	        	           // char *string = cJSON_Print(output_buffer);
		char *message = cJSON_GetObjectItem(root2,"msg")->valuestring;
		ESP_LOGI(TAG, "mensajes= %s",message);
		cJSON_Delete(root2);    */
    }
    }
    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t echo = {
    .uri       = "/echo",
    .method    = HTTP_POST,
    .handler   = echo_post_handler,
    .user_ctx  = NULL
};

static esp_err_t version_handler(httpd_req_t *req)
{

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
	        //concat2(modelav,s);
	          //  printf ("\n s = %s\n\n", s);
	      //  esp_base_mac_addr_set(base_mac_addr);

	            //Get the derived MAC address for each network interface
	          //  uint8_t derived_mac_addr[6] = {0};
	            //Get MAC address for WiFi Station interface
	      //      ESP_ERROR_CHECK(esp_read_mac(derived_mac_addr, ESP_MAC_WIFI_STA));
	       //     ESP_LOGI("WIFI_STA MAC", "0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
	       //              derived_mac_addr[0], derived_mac_addr[1], derived_mac_addr[2],
	       //              derived_mac_addr[3], derived_mac_addr[4], derived_mac_addr[5]);
	char *trama_version(void)
	{
	    char *string = NULL;

	    cJSON *monitor = cJSON_CreateObject();
	    if (cJSON_AddStringToObject(monitor, "endpoint", "VERSION") == NULL)
	        {goto end;	}
	    if (cJSON_AddStringToObject(monitor, "Serial_ESP", concat2(modelav,s)) == NULL)
	   	    {goto end;}
	    if (cJSON_AddStringToObject(monitor, "MAC_ESP", s) == NULL)//pendiente extraer nombre del archivo
	    	{goto end;}
	    if (cJSON_AddStringToObject(monitor, "Version_ESP", "dsf34545g.bin") == NULL)//pendiente extraer nombre del archivo
	    	{ goto end; }
	    if (cJSON_AddStringToObject(monitor, "Version_STM", "dfdsfsdf.bin") == NULL)//pendiente extraer nombre del archivo
	        {goto end; }

	    string = cJSON_Print(monitor);
	    if (string == NULL)
	    {
	        fprintf(stderr, "Failed to print monitor.\n");
	    }

	end:
	    cJSON_Delete(monitor);
	    return string;
	}
	httpd_resp_set_status(req,HTTPD_200);
	httpd_resp_sendstr(req, trama_version());
	grabar_nvs("8_MACADRRWIFI",s);
	grabar_nvs("2_WiFi-AP-SSID",concat("ivAdventureM_", leer_nvs("8_MACADRRWIFI")));
    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t version_uri = {
    .uri       = "/VERSION",
    .method    = HTTP_POST,
    .handler   = version_handler,
    .user_ctx  = NULL
};

static esp_err_t trama_handler(httpd_req_t *req)
{
	  char *responhttp=NULL;
      char buf[100];
       int ret, remaining = req->content_len;

       if(remaining <= 0){

       	char *json_armado(void)
       		{
       		    char *string = NULL;

       		    cJSON *monitor = cJSON_CreateObject();
       		    if (cJSON_AddStringToObject(monitor, "endpoint", "TRAMA") == NULL)
       		 		{goto end;	}
       		    if (cJSON_AddStringToObject(monitor, "TIME", leer_nvs("1_TRAMA-TIME")) == NULL)
       		   	    {
       		   	        goto end;
       		   	    }


       		    string = cJSON_Print(monitor);
       		    if (string == NULL)
       		    {
       		        fprintf(stderr, "Failed to print monitor.\n");
       		    }

       		end:
       		    cJSON_Delete(monitor);
       		    return string;
       		}
       	responhttp= json_armado();
       }

       else{

       while (remaining >= 1) {
           /* Read the data for the request */
           if ((ret = httpd_req_recv(req, buf,
                           MIN(remaining, sizeof(buf)))) <= 0) {
               if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                   /* Retry receiving if timeout occurred */
                   continue;
               }

               return ESP_FAIL;
           }

           /* Send back the same data */
          // httpd_resp_send_chunk(req, buf, ret);
           remaining -= ret;

           /* Log data received
           ESP_LOGI(TAGL, "=========== RECEIVED DATA ==========");
           ESP_LOGI(TAGL, "%.*s", ret, buf);
           ESP_LOGI(TAGL, "trama: %s\n", buf);
           ESP_LOGI(TAGL, "====================================");*/
           int status = 0;

			   cJSON *root2 =cJSON_ParseWithOpts(buf, NULL, false);
			   if (root2 == NULL)
				   {
					   const char *error_ptr = cJSON_GetErrorPtr();
					   if (error_ptr != NULL)
					   {
						   fprintf(stderr, "Error before: %s\n", error_ptr);
						   responhttp=trama_endp( "TRAMA", "msg","Error in JSON data");

					   }
					   status = 0;
					   goto end;
				   }
           		char *token = cJSON_GetObjectItem(root2,"token")->valuestring;
           		//ESP_LOGI(TAGL, "token= %s",token);
           		char *message = cJSON_GetObjectItem(root2,"time")->valuestring;
				//ESP_LOGI(TAGL, "tiempo= %s",message);
				int result = strcmp(token_httser, token);
				if(result==0)//login correcto
				  {
					grabar_nvs("1_TRAMA-TIME",message);
					ESP_LOGW(TAGL, "tiempo almacenado");
					responhttp=trama_endp( "TRAMA", "msg","Data saved..");
				  	}
				else{
					ESP_LOGE(TAGL, "ERROR DE LOGIN");
					responhttp=trama_endp( "TRAMA", "msg","Error token..");
				}
				cJSON_Delete(root2);
          }
       }
end:
       httpd_resp_set_status(req,HTTPD_200);
	   httpd_resp_sendstr(req, responhttp);
   // End response
	  httpd_resp_send_chunk(req, NULL, 0);
	  return ESP_OK;
}

static const httpd_uri_t trama_uri = {
    .uri       = "/TRAMA",
    .method    = HTTP_POST,
    .handler   = trama_handler,
    .user_ctx  = NULL
};

static esp_err_t server_handler(httpd_req_t *req)
{

	char *responhttp=NULL;
	char buf[100];
	       int ret, remaining = req->content_len;

	       if(remaining <= 0){

	       	char *json_armado(void)
	       		{
	       		    char *string = NULL;

	       		    cJSON *monitor = cJSON_CreateObject();
	       		 if (cJSON_AddStringToObject(monitor, "endpoint", "SERVER") == NULL)
					{goto end;	}
	       		    if (cJSON_AddStringToObject(monitor, "Time_Server_ms", leer_nvs("4_SERVERTIME")) == NULL)
	       		   	    {   goto end;   }


	       		    string = cJSON_Print(monitor);
	       		    if (string == NULL)
	       		    {
	       		        fprintf(stderr, "Failed to print monitor.\n");
	       		    }

	       		end:
	       		    cJSON_Delete(monitor);
	       		    return string;
	       		}
	       		httpd_resp_set_status(req,HTTPD_200);
	       		httpd_resp_sendstr(req, json_armado());
	       }

	       else{

	       while (remaining >= 1) {
	           /* Read the data for the request */
	           if ((ret = httpd_req_recv(req, buf,
	                           MIN(remaining, sizeof(buf)))) <= 0) {
	               if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
	                   /* Retry receiving if timeout occurred */
	                   continue;
	               }

	               return ESP_FAIL;
	           }

	           /* Send back the same data */
	          // httpd_resp_send_chunk(req, buf, ret);
	           remaining -= ret;

	           /* Log data received
	           ESP_LOGI(TAGL, "=========== RECEIVED DATA ==========");
	           ESP_LOGI(TAGL, "%.*s", ret, buf);
	           ESP_LOGI(TAGL, "trama: %s\n", buf);
	           ESP_LOGI(TAGL, "====================================");*/
	           cJSON *root2 = cJSON_Parse(buf);

	           		char *token = cJSON_GetObjectItem(root2,"token")->valuestring;
	           		//ESP_LOGI(TAGL, "token= %s",token);
	           		char *message = cJSON_GetObjectItem(root2,"time")->valuestring;
					//ESP_LOGI(TAGL, "tiempo= %s",message);
					int result = strcmp(token_httser, token);
					if(result==0)//login correcto
					  {
						grabar_nvs("4_SERVERTIME",message);
						ESP_LOGW(TAGL, "tiempo almacenado");
						responhttp=trama_endp( "SERVER", "msg","Data saved..");
					  	}
					else{
						ESP_LOGE(TAGL, "ERROR DE LOGIN");
						responhttp=trama_endp( "SERVER", "msg","Error token..");
					}
					cJSON_Delete(root2);
	          }
	       }
    httpd_resp_set_status(req,HTTPD_200);
    httpd_resp_sendstr(req, responhttp);
    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t server_uri = {
    .uri       = "/SERVER",
    .method    = HTTP_POST,
    .handler   = server_handler,
    .user_ctx  = NULL
};

static esp_err_t resetesp_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	char buf[100];
	       int ret, remaining = req->content_len;

	       if(remaining <= 0){

				   responhttp=trama_endp( "SETPORTS", "msg","No read Data");
			   }

		   else{

			   while (remaining >= 1) {
	           /* Read the data for the request */
	           if ((ret = httpd_req_recv(req, buf,
	                           MIN(remaining, sizeof(buf)))) <= 0) {
	               if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
	                   /* Retry receiving if timeout occurred */
	                   continue;
	               }

	               return ESP_FAIL;
	           }

	           /* Send back the same data */
	         //  httpd_resp_send_chunk(req, buf, ret);
	           remaining -= ret;

	           /* Log data received
	           ESP_LOGI(TAGL, "=========== RECEIVED DATA ==========");
	           ESP_LOGI(TAGL, "%.*s", ret, buf);
	           ESP_LOGI(TAGL, "trama: %s\n", buf);
	           ESP_LOGI(TAGL, "====================================");*/
	           int status = 0;

				   cJSON *root2 =cJSON_ParseWithOpts(buf, NULL, false);
				   if (root2 == NULL)
					   {
						   const char *error_ptr = cJSON_GetErrorPtr();
						   if (error_ptr != NULL)
						   {
							   fprintf(stderr, "Error before: %s\n", error_ptr);
							   responhttp=trama_endp( "RESETESP", "msg","Error in JSON data");

						   }
						   status = 0;
						   goto end;
					   }

	           		char *token = cJSON_GetObjectItem(root2,"token")->valuestring;
	           		//ESP_LOGI(TAGL, "token= %s",token);
	           		char *message = cJSON_GetObjectItem(root2,"RESET")->valuestring;
					//ESP_LOGI(TAGL, "tiempo= %s",message);
					int result = strcmp(token_httser, token);
					if(result==0)//login correcto
					  {

						if (strcmp("true", message) == 0) {
							//ESP_LOGW(TAGL, "reset in progress..4");

							responhttp=trama_endp( "RESETESP", "msg","Reset in progress..");
							httpd_resp_set_status(req,HTTPD_200);
							httpd_resp_sendstr(req, responhttp);
							httpd_resp_send_chunk(req, NULL, 0);
							vTaskDelay(5000 / portTICK_PERIOD_MS);
						    //esp_restart();
							}
						else {//ESP_LOGW(TAGL, "command error");

							responhttp=trama_endp( "RESETESP", "msg","Error command");


						}
					  }
						//grabar_nvs("time_tramas",message);

					else{
						ESP_LOGE(TAGL, "ERROR DE LOGIN");
						responhttp=trama_endp( "RESETESP", "msg","Error token..");
					}
					cJSON_Delete(root2);
	          }
		   }//ELSE
end:
	       httpd_resp_set_status(req,HTTPD_200);
		httpd_resp_sendstr(req, responhttp);


	           // End response
	           httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static const httpd_uri_t resetesp_uri = {
    .uri       = "/RESETESP",
    .method    = HTTP_POST,
    .handler   = resetesp_handler,
    .user_ctx  = NULL
};

static esp_err_t defaultesp_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	char buf[100];
	       int ret, remaining = req->content_len;

	       if(remaining <= 0){

				   responhttp=trama_endp( "SETPORTS", "msg","No read Data");
			   }

			else{

			   while (remaining >= 1) {
	           /* Read the data for the request */
	           if ((ret = httpd_req_recv(req, buf,
	                           MIN(remaining, sizeof(buf)))) <= 0) {
	               if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
	                   /* Retry receiving if timeout occurred */
	                   continue;
	               }

	               return ESP_FAIL;
	           }

	           /* Send back the same data */

	           remaining -= ret;

	           int status = 0;

			   cJSON *root2 =cJSON_ParseWithOpts(buf, NULL, false);
			   if (root2 == NULL)
				   {
					   const char *error_ptr = cJSON_GetErrorPtr();
					   if (error_ptr != NULL)
					   {
						   fprintf(stderr, "Error before: %s\n", error_ptr);
						   responhttp=trama_endp( "DEFAULTESP", "msg","Error in JSON data");

					   }
					   status = 0;
					   goto end;
				   }

	           		char *token = cJSON_GetObjectItem(root2,"token")->valuestring;
	           		//ESP_LOGI(TAGL, "token= %s",token);
	           		char *message = cJSON_GetObjectItem(root2,"RESET")->valuestring;
					//ESP_LOGI(TAGL, "tiempo= %s",message);
					int result = strcmp(token_httser, token);
					if(result==0)//login correcto
					  {

						if (strcmp("true", message) == 0) {
							//ESP_LOGW(TAGL, "reset in progress..4");

							responhttp=trama_endp( "DEFAULTESP", "msg","Default reset in progress..");
							grabar_nvs("4_SERVER-TIME","30");
							grabar_nvs("1_TRAMA-TIME","2000");
							grabar_nvs("2_WiFi-AP-PASS","ivam45678");
							grabar_nvs("2_WiFi-AP-IP","124.213.16.29");
							grabar_nvs("2_WiFi-ST-SSID","WIFI_PRUEBAS");
							grabar_nvs("2_WiFi-ST-PASS","12345678");
							grabar_nvs("2_WiFi-ST-INTE","3");
							grabar_nvs("4_URL-TRAMA","https://atsdev.actiontracker.es/ivtramas/api-tramas/api/trama/saveFromESP32");
							grabar_nvs("4_URL-LOGIN","https://atsdev.actiontracker.es/api-ats/authDevice/login");
							grabar_nvs("4_TOKEN-LOGIN","Basic SUFtaW5pLTAxOjAxMEEwMDAwODExMmNmYTQzNjMwMzQzNzMzYTU1MTQ4IA");
							grabar_nvs("4_TIMEOUT-HTTP","8000");
							grabar_nvs("4_TOKEN-TIME","12");
							grabar_nvs("5_TOK-HTTPSER","dslfkjasdkhf732489032l;ksd");
							grabar_nvs("7_MODO-CONEX","0");
							grabar_nvs("7_STATUS-MATR","0");
							grabar_nvs("1_PRESCOM","255");
							grabar_nvs("1_PRESION_DISP","255");
							grabar_nvs("1_PRESDELAN","255");
							grabar_nvs("1_PRESTRAS","255");
							grabar_nvs("1_PRESACEI","255");
							grabar_nvs("1_TEMPTRANS","255");
							grabar_nvs("1_TEMPTURBO","255");
							grabar_nvs("1_TEMPESCAP","255");
							grabar_nvs("1_TEMPREFRI","255");
							grabar_nvs("1_TEMPACE","255");
							grabar_nvs("1_VADO_A_T","255");
							grabar_nvs("1_VDO_B_T","255");
							grabar_nvs("1_VEO_C_T","255");
							grabar_nvs("1_CLCIO","255");
							grabar_nvs("1_NEVERA","255");
							grabar_nvs("1_TEMPAMBINT","255");
							grabar_nvs("1_TEMPAMBEXT","255");
							grabar_nvs("1_RESHELADAGUA","255");
							grabar_nvs("1_TEMP_TRANSM","255");
							grabar_nvs("1_TEMPAMB_DISP","255");
							grabar_nvs("1_TEMPCABIN_DIS","255");
							grabar_nvs("1_VENTILADOR_A","255");
							grabar_nvs("1_VENTILADOR_B","255");
							grabar_nvs("1_VENTILADOR_C","255");
							grabar_nvs("1_INTERRUPTOR_C","255");
							grabar_nvs("1_INTERRUPTOR_V","255");
							grabar_nvs("1_INTERRUPTOR_N","255");
							grabar_nvs("1_TODO_TRANSM","255");
							grabar_nvs("1_FRONTAL_TRA","255");
							grabar_nvs("1_CENTRAL_TRANS","255");
							grabar_nvs("1_TRASERO_TRANS","255");
							grabar_nvs("1_DESBLOQUEADO","255");
							grabar_nvs("1_FGD_TRANSM","255");
							grabar_nvs("1_RGD_TRANSM","255");
							grabar_nvs("1_AGD_TRANSM","255");
							grabar_nvs("1_WINCHE_A_ES","255");
							grabar_nvs("1_WINCHE_A_RE","255");
							grabar_nvs("1_WINCHE_B_ES","255");
							grabar_nvs("1_WINCHE_B_RE","255");
							grabar_nvs("1_WINCHE_C_ES","255");
							grabar_nvs("1_WINCHE_C_RE","255");
							grabar_nvs("1_FIN_WINC_A_ES","255");
							grabar_nvs("1_FIN_WINC_A_RE","255");
							grabar_nvs("1_FIN_WINC_B_ES","255");
							grabar_nvs("1_FIN_WINC_B_RE","255");
							grabar_nvs("1_FIN_WINC_C_ES","255");
							grabar_nvs("1_FIN_WINC_C_RE","255");
							grabar_nvs("1_WINCHE_A","255");
							grabar_nvs("1_WINCHE_B","255");
							grabar_nvs("1_WINCHE_C","255");
							grabar_nvs("1_NIEBLA","255");
							grabar_nvs("1_COMPRESOR","255");
							grabar_nvs("1_TOLDO_SUPERIOR","255");
							grabar_nvs("1_TOLDO_LATERAL","255");
							grabar_nvs("1_MALETERO","255");
							grabar_nvs("1_COMPRESOR_NE","255");
							grabar_nvs("1_BATERIA_A_VO","255");
							grabar_nvs("1_BATERIA_A_CU","255");
							grabar_nvs("1_BATERIA_B_VO","255");
							grabar_nvs("1_BATERIA_B_CU","255");
							grabar_nvs("1_BATERIA_C_VO","255");
							grabar_nvs("1_BATERIA_C_CU","255");
							grabar_nvs("1_BATERIA_INT1","255");
							grabar_nvs("1_ACELERADOR_1","255");
							grabar_nvs("1_ACELERADOR_2","255");
							grabar_nvs("1_ACELERADOR_3","255");
							grabar_nvs("1_ACELERADOR_4","255");
							grabar_nvs("1_TEMPORIZADOR1","255");
							grabar_nvs("1_TEMPORIZADOR2","255");
							grabar_nvs("1_TEMPORIZADOR3","255");
							grabar_nvs("1_TEMPORIZADOR4","255");
							grabar_nvs("1_TEMPORIZADOR5","255");
							grabar_nvs("1_TEMPORIZADOR6","255");
							grabar_nvs("1_TEMPORIZADOR7","255");
							grabar_nvs("1_TEMPORIZADOR8","255");
							grabar_nvs("1_TEMPORIZADOR9","255");
							grabar_nvs("1_TEMPORIZADO10","255");
							grabar_nvs("1_FRESHWATER","255");
							grabar_nvs("1_BLACKWATER","255");
							grabar_nvs("1_GREYWATER","255");
							grabar_nvs("1_OILOTHER","255");
							grabar_nvs("1_BALLAST","255");
							grabar_nvs("1_SLUDGE","255");
							grabar_nvs("1_BILGE","255");
							grabar_nvs("1_SEAWATER","255");
							grabar_nvs("1_GASOLINA_DISP","255");
							grabar_nvs("1_SYSGO_1","255");
							grabar_nvs("1_SYSGO_2","255");
							grabar_nvs("1_SYSGO_3","255");
							grabar_nvs("1_SYSGO_4","255");
							grabar_nvs("1_SYSGO_5","255");
							grabar_nvs("1_RIGDELAN","255");
							grabar_nvs("1_RIGTRAS","255");
							grabar_nvs("1_RIGTODO","255");
							grabar_nvs("1_ALTDELAN","255");
							grabar_nvs("1_ALTTRAS","255");
							grabar_nvs("1_ALTTODO","255");
							grabar_nvs("1_RIBDELAN","255");
							grabar_nvs("1_RIBTRAS","255");
							grabar_nvs("1_RIBTODO","255");
							grabar_nvs("1_GNSS_GGA","255");
							grabar_nvs("1_GNSS_VEL","255");
							grabar_nvs("1_GNSS_DIST","255");
							grabar_nvs("1_VENTILADO_A_R","255");
							grabar_nvs("1_VENTILADO_B_R","255");
							grabar_nvs("1_VENTILADO_C_R","255");
							grabar_nvs("1_RPM_DISP","255");
							grabar_nvs("1_NEUMA_1","255");
							grabar_nvs("1_NEUMA_2","255");
							grabar_nvs("1_NEUMA_3","255");
							grabar_nvs("1_NEUMA_4","255");
							grabar_nvs("1_NEUMA_5","255");
							grabar_nvs("1_NEUMA_6","255");
							grabar_nvs("1_NEUMA_7","255");
							grabar_nvs("1_NEUMA_8","255");
							grabar_nvs("1_NEUMA_9","255");
							grabar_nvs("1_NEUMA_10","255");
							grabar_nvs("1_NEUMA_11","255");
							grabar_nvs("1_NEUMA_12","255");
							grabar_nvs("1_NEUMA_13","255");
							grabar_nvs("1_NEUMA_14","255");
							grabar_nvs("1_NEUMA_15","255");
							grabar_nvs("1_NEUMA_16","255");
							grabar_nvs("1_GIROS_DISP","255");
							grabar_nvs("1_INCLINACION_D","255");
							grabar_nvs("1_ALARMA_BOTON","255");
							grabar_nvs("1_INPUT_DIG1","255");
							grabar_nvs("1_MARCHA_TRANSM","255");
							grabar_nvs("1_TORQUE_DISP","255");
							grabar_nvs("1_POTEN_DISP","255");
							grabar_nvs("1_VELOCIDAD_DIS","255");
							grabar_nvs("1_MSNM_DISP","255");
							grabar_nvs("1_SHIFT_DISP","255");
							httpd_resp_set_status(req,HTTPD_200);
							httpd_resp_sendstr(req, responhttp);
							httpd_resp_send_chunk(req, NULL, 0);
							vTaskDelay(5000 / portTICK_PERIOD_MS);
						    //esp_restart(); // FALTA VERIFICAR COMO RESETEAR LA NVS
							}
						else {//ESP_LOGW(TAGL, "command error");

							responhttp=trama_endp( "DEFAULTESP", "msg","Error command");


						}
					  }

					else{
						ESP_LOGE(TAGL, "ERROR DE LOGIN");
						responhttp=trama_endp( "DEFAULTESP", "msg","Error token..");
					}
					cJSON_Delete(root2);
	          }
	        }//else
end:
	       httpd_resp_set_status(req,HTTPD_200);
		httpd_resp_sendstr(req, responhttp);


	           // End response
	           httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static const httpd_uri_t defaultesp_uri = {
    .uri       = "/DEFAULTESP",
    .method    = HTTP_POST,
    .handler   = defaultesp_handler,
    .user_ctx  = NULL
};

static esp_err_t wifiap_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	char buf[300];

	       int ret, remaining = req->content_len;

	       if(remaining <= 0){

	       	char *json_armado(void)
	       		{
	       		    char *string = NULL;

	       		    cJSON *monitor = cJSON_CreateObject();
	       		const char *temp1= leer_nvs("2_WiFi-AP-SSID");
	       		const char *temp2= leer_nvs("2_WiFi-AP-PASS");
	       		const char *temp3= leer_nvs("2_WiFi-AP-IP");

	       		if (cJSON_AddStringToObject(monitor, "endpoint", "WIFIAP") == NULL)
	       			{goto end; }
	       		if (cJSON_AddStringToObject(monitor, "WIFI_AP_SSID", temp1) == NULL)
	       		 	{goto end; }

	       		if (cJSON_AddStringToObject(monitor, "WIFI_AP_PASS", temp2) == NULL)
	       			 { goto end; }

	       		if (cJSON_AddStringToObject(monitor, "WIFI_AP_IP", temp3) == NULL)
					{goto end; }

	       		    string = cJSON_Print(monitor);
	       		    if (string == NULL)
	       		    {fprintf(stderr, "Failed to print monitor.\n"); }

	       		end:
	       		    cJSON_Delete(monitor);
	       		 //ESP_LOGW(TAGL, "WIFI AP 4 %s\n", string);
	       		    return string;
	       		}
	       		//httpd_resp_set_status(req,HTTPD_200);
	       //	ESP_LOGW(TAGL, "WIFI AP 1",json_armado());
	        	responhttp= json_armado();
	       }

	       else{

	       while (remaining >= 1) {
	           /* Read the data for the request */
	           if ((ret = httpd_req_recv(req, buf,
	                           MIN(remaining, sizeof(buf)))) <= 0) {
	               if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
	                   /* Retry receiving if timeout occurred */
	                   continue;
	               }

	               return ESP_FAIL;
	           }

	           /* Send back the same data */

	           remaining -= ret;

	           int status = 0;

			   cJSON *root2 =cJSON_ParseWithOpts(buf, NULL, false);
			   if (root2 == NULL)
				   {
					   const char *error_ptr = cJSON_GetErrorPtr();
					   if (error_ptr != NULL)
					   {
						   fprintf(stderr, "Error before: %s\n", error_ptr);
						   responhttp=trama_endp( "WIFIAP", "msg","Error in JSON data");

					   }
					   status = 0;
					   goto end;
				   }

	           char *token  = cJSON_GetObjectItem(root2,"token")->valuestring;
				char *ssidap = cJSON_GetObjectItem(root2,"WIFI_AP_SSID")->valuestring;
				char *passap = cJSON_GetObjectItem(root2,"WIFI_AP_PASS")->valuestring;
				char *ipap   = cJSON_GetObjectItem(root2,"WIFI_AP_IP")->valuestring;

				int result = strcmp(token_httser, token);
				if(result==0)//login correcto
				  {
					char *ssidcom=concat(ssidap, "_");
					grabar_nvs("2_WiFi-AP-SSID",concat(ssidcom, leer_nvs("8_MACADRRWIFI")));
					//grabar_nvs("2_WiFi-AP-SSID",ssidap);
					grabar_nvs("2_WIFI-AP-PASS",passap);
					grabar_nvs("2_WIFI-AP-IP",ipap);
					ESP_LOGW(TAGL, "Data wifi ap saved");
					responhttp=trama_endp( "WIFIAP", "msg","Data saved..");
					}
				else{
					ESP_LOGE(TAGL, "ERROR DE LOGIN");
					responhttp=trama_endp( "WIFIAP", "msg","Error token..");
				}
				cJSON_Delete(root2);
	          }
	       }
end:
   httpd_resp_set_status(req,HTTPD_200);
   httpd_resp_sendstr(req, responhttp);
   // End response
   httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static const httpd_uri_t wifiap_uri = {
    .uri       = "/WIFIAP",
    .method    = HTTP_POST,
    .handler   = wifiap_handler,
    .user_ctx  = NULL
};

static esp_err_t wifist_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	char buf[300];


	       int ret, remaining = req->content_len;

	       if(remaining <= 0){

	       	char *json_armado(void)
	       		{
	       		    char *string = NULL;

	       		    cJSON *monitor = cJSON_CreateObject();
	       		const char *temp1= leer_nvs("2_WiFi-ST-SSID");
				const char *temp2= leer_nvs("2_WiFi-ST-PASS");
				const char *temp3= leer_nvs("2_WiFi-ST-INTE");

				if (cJSON_AddStringToObject(monitor, "endpoint", "WIFIST") == NULL)
					{goto end; }
	       		if (cJSON_AddStringToObject(monitor, "WIFI_ST_SSID", temp1) == NULL)
	       		 	{goto end; }
	       		if (cJSON_AddStringToObject(monitor, "WIFI_ST_PASS", temp2) == NULL)// ELIMINAR PARA VERSION FINAL
	       			 { goto end; }
	       		if (cJSON_AddStringToObject(monitor, "WIFI_ST_INTENTOS", temp3) == NULL)
					{goto end; }

	       		    string = cJSON_Print(monitor);
	       		    if (string == NULL)
	       		    {
	       		        fprintf(stderr, "Failed to print monitor.\n");
	       		    }

	       		end:
	       		    cJSON_Delete(monitor);
	       		    return string;
	       		}
	       	responhttp= json_armado();
	       }

	       else{

	       while (remaining >= 1) {
	           /* Read the data for the request */
	           if ((ret = httpd_req_recv(req, buf,
	                           MIN(remaining, sizeof(buf)))) <= 0) {
	               if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
	                   /* Retry receiving if timeout occurred */
	                   continue;
	               }

	               return ESP_FAIL;
	           }

	           /* Send back the same data */

	           remaining -= ret;

	           int status = 0;

			   cJSON *root2 =cJSON_ParseWithOpts(buf, NULL, false);
			   if (root2 == NULL)
				   {
					   const char *error_ptr = cJSON_GetErrorPtr();
					   if (error_ptr != NULL)
					   {
						   fprintf(stderr, "Error before: %s\n", error_ptr);
						   responhttp=trama_endp( "WIFIST", "msg","Error in JSON data");

					   }
					   status = 0;
					   goto end;
				   }
	           	    char *token = cJSON_GetObjectItem(root2,"token")->valuestring;
	           		char *ssidst = cJSON_GetObjectItem(root2,"WIFI_ST_SSID")->valuestring;
	           		char *passst = cJSON_GetObjectItem(root2,"WIFI_ST_PASS")->valuestring;
	           		char *intentos = cJSON_GetObjectItem(root2,"WIFI_ST_INTENTOS")->valuestring;

					int result = strcmp(token_httser, token);
					if(result==0)//login correcto
					  {
						grabar_nvs("2_WiFi-ST-SSID",ssidst);
						grabar_nvs("2_WIFI-ST-PASS",passst);
						grabar_nvs("2_WIFI-ST-INTE",intentos);
						ESP_LOGW(TAGL, "Data wifi ap saved");
						responhttp=trama_endp( "WIFIST", "msg","Data saved..");
					  	}
					else{
						ESP_LOGE(TAGL, "ERROR DE LOGIN");
						responhttp=trama_endp( "WIFIST", "msg","Error token..");
					}
					cJSON_Delete(root2);
	          }
	       }
end:
   httpd_resp_set_status(req,HTTPD_200);
   httpd_resp_sendstr(req, responhttp);
   // End response
   httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static const httpd_uri_t wifist_uri = {
    .uri       = "/WIFIST",
    .method    = HTTP_POST,
    .handler   = wifist_handler,
    .user_ctx  = NULL
};

static esp_err_t sethttp_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
		char buf[500];


		       int ret, remaining = req->content_len;

		       if(remaining <= 0){

		       	char *json_armado(void)
		       		{
		       		    char *string = NULL;


		       		const char *temp1= leer_nvs("4_URL-TRAMA");
					const char *temp2= leer_nvs("4_URL-LOGIN");
					const char *temp3= leer_nvs("4_TOKEN-LOGIN");// ELIMINAR PARA VERSION FINAL
					const char *temp4= leer_nvs("4_TIMEOUT-HTTP");
					const char *temp5= leer_nvs("4_TOKEN-TIME");
					const char *temp6= leer_nvs("4_SERVER-TIME");
					const char *temp7= leer_nvs("5_TOK-HTTPSER");// ELIMINAR PARA VERSION FINAL
					cJSON *monitor = cJSON_CreateObject();
					if (cJSON_AddStringToObject(monitor, "endpoint", "SETHTTP") == NULL)
						{goto end; }
		       		if (cJSON_AddStringToObject(monitor, "URL_TRAMA", temp1) == NULL)
		       		 	{goto end; }
		       		if (cJSON_AddStringToObject(monitor, "URL_LOGIN", temp2) == NULL)
		       			 { goto end; }
		       		if (cJSON_AddStringToObject(monitor, "TOKEN_LOGIN", temp3) == NULL) // ELIMINAR PARA VERSION FINAL
						{goto end; }
		       		if (cJSON_AddStringToObject(monitor, "TOKEN_SERVER", temp7) == NULL) // ELIMINAR PARA VERSION FINAL
		       			{goto end; }
		       		if (cJSON_AddStringToObject(monitor, "TIMEOUT_HTTPCLIENT", temp4) == NULL)
						{goto end; }
					if (cJSON_AddStringToObject(monitor, "TOKEN_TIME", temp5) == NULL)
						 { goto end; }
					if (cJSON_AddStringToObject(monitor, "SERVER_TIME", temp6) == NULL)
						{goto end; }

		       		    string = cJSON_Print(monitor);
		       		    if (string == NULL)
		       		    {
		       		        fprintf(stderr, "Failed to print monitor.\n");
		       		    }

		       		end:
		       		    cJSON_Delete(monitor);
		       		    return string;
		       		}
		       	responhttp= json_armado();
		       }

		       else{

		       while (remaining >= 1) {
		           /* Read the data for the request */
		           if ((ret = httpd_req_recv(req, buf,
		                           MIN(remaining, sizeof(buf)))) <= 0) {
		               if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
		                   /* Retry receiving if timeout occurred */
		                   continue;
		               }

		               return ESP_FAIL;
		           }

		           /* Send back the same data */
		        //   httpd_resp_send_chunk(req, buf, ret);
		           remaining -= ret;

		           /* Log data received
		           ESP_LOGI(TAGL, "=========== RECEIVED DATA ==========");
		           ESP_LOGI(TAGL, "%.*s", ret, buf);
		           ESP_LOGI(TAGL, "trama: %s\n", buf);
		           ESP_LOGI(TAGL, "====================================");*/
		           int status = 0;

					   cJSON *root2 =cJSON_ParseWithOpts(buf, NULL, false);
					   if (root2 == NULL)
						   {
							   const char *error_ptr = cJSON_GetErrorPtr();
							   if (error_ptr != NULL)
							   {
								   fprintf(stderr, "Error before: %s\n", error_ptr);
								   responhttp=trama_endp( "SETHTTP", "msg","Error in JSON data");

							   }
							   status = 0;
							   goto end;
						   }
		           	    char *token = cJSON_GetObjectItem(root2,"token")->valuestring;
		           		char *urltra = cJSON_GetObjectItem(root2,"URL_TRAMA")->valuestring;
		           		char *urllog = cJSON_GetObjectItem(root2,"URL_LOGIN")->valuestring;
		           		char *toklog = cJSON_GetObjectItem(root2,"TOKEN_LOGIN")->valuestring;
		           		char *tokser = cJSON_GetObjectItem(root2,"TOKEN_SERVER")->valuestring;
		           		char *timout = cJSON_GetObjectItem(root2,"TIMEOUT_HTTPCLIENT")->valuestring;
						char *toktim = cJSON_GetObjectItem(root2,"TOKEN_TIME")->valuestring;
						char *sertim = cJSON_GetObjectItem(root2,"SERVER_TIME")->valuestring;

						int result = strcmp(token_httser, token);
						if(result==0)//login correcto
						  {
							grabar_nvs("4_URL-TRAMA",urltra);
					  		grabar_nvs("4_URL-LOGIN",urllog);
							grabar_nvs("4_TOKEN-LOGIN",toklog);
							grabar_nvs("4_TIMEOUT-HTTPCLIENT",timout);
							grabar_nvs("4_TOKEN-TIME",toktim);
							grabar_nvs("5_TOK-HTTPSER",tokser);
							grabar_nvs("4_SERVER-TIME",sertim);

							ESP_LOGW(TAGL, "Data HTTP saved");
							responhttp=trama_endp( "SETHTTP", "msg","Data saved..");
						  	}
						else{
							ESP_LOGE(TAGL, "ERROR DE LOGIN");
							responhttp=trama_endp( "SETHTTP", "msg","Error token..");
						}
						cJSON_Delete(root2);
		          }
		       }
end:
	   httpd_resp_set_status(req,HTTPD_200);
	   httpd_resp_sendstr(req, responhttp);
	   // End response
	   httpd_resp_send_chunk(req, NULL, 0);

	    return ESP_OK;
}

static const httpd_uri_t sethttp_uri = {
    .uri       = "/SETHTTP",
    .method    = HTTP_POST,
    .handler   = sethttp_handler,
    .user_ctx  = NULL
};

static esp_err_t setports_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	char buf[4100];
	int grabar=0;

		       int ret, remaining = req->content_len;

		       if(remaining <= 0){


		    	   responhttp=trama_endp( "SETPORTS", "msg","No read Data");
		       }

		       else{

		       while (remaining >= 1) {
		           /* Read the data for the request */
		           if ((ret = httpd_req_recv(req, buf,
		                           MIN(remaining, sizeof(buf)))) <= 0) {
		               if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
		                   /* Retry receiving if timeout occurred */
		                   continue;
		               }

		               return ESP_FAIL;
		           }

		           /* Send back the same data */
		        //   httpd_resp_send_chunk(req, buf, ret);
		           remaining -= ret;

		           /* Log data received
		           ESP_LOGI(TAGL, "=========== RECEIVED DATA ==========");
		           ESP_LOGI(TAGL, "%.*s", ret, buf);
		          // ESP_LOGI(TAGL, "trama: %s\n", buf);
		           ESP_LOGI(TAGL, "====================================");*/
		           int status = 0;

		           cJSON *root2 =cJSON_ParseWithOpts(buf, NULL, false);
		           if (root2 == NULL)
		               {
		                   const char *error_ptr = cJSON_GetErrorPtr();
		                   if (error_ptr != NULL)
		                   {
		                       fprintf(stderr, "Error before: %s\n", error_ptr);
		                       responhttp=trama_endp( "SETPORTS", "msg","Error in JSON data");

		                   }
		                   status = 0;
		                   goto end;
		               }
		               char *token = cJSON_GetObjectItem(root2,"token")->valuestring;
		        	   ESP_LOGI(TAGL, "%s", token);

		        		cJSON *press = cJSON_GetObjectItem(root2,"PRESION");
		        		char *item1 = cJSON_GetObjectItem(press,"PRESCOM")->valuestring;
						char *item2 = cJSON_GetObjectItem(press,"PRESION_DISP")->valuestring;
						char *item3 = cJSON_GetObjectItem(press,"PRESDELAN")->valuestring;
						char *item4 = cJSON_GetObjectItem(press,"PRESTRAS")->valuestring;
						char *item5 = cJSON_GetObjectItem(press,"PRESACEI")->valuestring;

						cJSON *temp = cJSON_GetObjectItem(root2,"TEMPERATURA");
						char *item6 = cJSON_GetObjectItem(temp,"TEMPTRANS")->valuestring;
						char *item7 = cJSON_GetObjectItem(temp,"TEMPTURBO")->valuestring;
						char *item8 = cJSON_GetObjectItem(temp,"TEMPESCAP")->valuestring;
						char *item9 = cJSON_GetObjectItem(temp,"TEMPREFRI")->valuestring;
						char *item10 = cJSON_GetObjectItem(temp,"TEMPACE")->valuestring;
						char *item11 = cJSON_GetObjectItem(temp,"VENTILADOR_A_TEMP")->valuestring;
						char *item12 = cJSON_GetObjectItem(temp,"VENTILADOR_B_TEMP")->valuestring;
						char *item13 = cJSON_GetObjectItem(temp,"VENTILADOR_C_TEMP")->valuestring;
						char *item14 = cJSON_GetObjectItem(temp,"CLIMATIZACION")->valuestring;
						char *item15 = cJSON_GetObjectItem(temp,"NEVERA")->valuestring;
						char *item16 = cJSON_GetObjectItem(temp,"TEMPAMBINT")->valuestring;
						char *item17 = cJSON_GetObjectItem(temp,"TEMPAMBEXT")->valuestring;
						char *item18 = cJSON_GetObjectItem(temp,"RESHELADAGUA")->valuestring;
						char *item19 = cJSON_GetObjectItem(temp,"TEMP_TRANSM")->valuestring;
						char *item20 = cJSON_GetObjectItem(temp,"TEMPAMB_DISP")->valuestring;
						char *item21 = cJSON_GetObjectItem(temp,"TEMPCABIN_DISP")->valuestring;

						cJSON *interr = cJSON_GetObjectItem(root2,"INTERRUPTOR");
						char *item22 = cJSON_GetObjectItem(interr,"VENTILADOR_A")->valuestring;
						char *item23 = cJSON_GetObjectItem(interr,"VENTILADOR_B")->valuestring;
						char *item24 = cJSON_GetObjectItem(interr,"VENTILADOR_C")->valuestring;
						char *item25 = cJSON_GetObjectItem(interr,"INTERRUPTOR_CLIMATIZADOR")->valuestring;
						char *item26 = cJSON_GetObjectItem(interr,"INTERRUPTOR_VENTILACION")->valuestring;
						char *item27 = cJSON_GetObjectItem(interr,"INTERRUPTOR_NEVERA")->valuestring;
						char *item28 = cJSON_GetObjectItem(interr,"TODO_TRANSM_DIFERENCIAL")->valuestring;
						char *item29 = cJSON_GetObjectItem(interr,"FRONTAL_TRANSM_DIFERENCIAL")->valuestring;
						char *item30 = cJSON_GetObjectItem(interr,"CENTRAL_TRANSM_DIFERENCIAL")->valuestring;
						char *item31 = cJSON_GetObjectItem(interr,"TRASERO_TRANSM_DIFERENCIAL")->valuestring;
						char *item32 = cJSON_GetObjectItem(interr,"DESBLOQUEADO_TRANSM_DIFERENCIAL")->valuestring;
						char *item33 = cJSON_GetObjectItem(interr,"FGD_TRANSM_TRACCION")->valuestring;
						char *item34 = cJSON_GetObjectItem(interr,"RGD_TRANSM_TRACCION")->valuestring;
						char *item35 = cJSON_GetObjectItem(interr,"AGD_TRANSM_TRACCION")->valuestring;
						char *item36 = cJSON_GetObjectItem(interr,"WINCHE_A_ESTIRAR")->valuestring;
						char *item37 = cJSON_GetObjectItem(interr,"WINCHE_A_RECOGER")->valuestring;
						char *item38 = cJSON_GetObjectItem(interr,"WINCHE_B_ESTIRAR")->valuestring;
						char *item39 = cJSON_GetObjectItem(interr,"WINCHE_B_RECOGER")->valuestring;
						char *item40 = cJSON_GetObjectItem(interr,"WINCHE_C_ESTIRAR")->valuestring;
						char *item41 = cJSON_GetObjectItem(interr,"WINCHE_C_RECOGER")->valuestring;
						char *item42 = cJSON_GetObjectItem(interr,"FINAL_WINCHE_A_ESTIRAR")->valuestring;
						char *item43 = cJSON_GetObjectItem(interr,"FINAL_WINCHE_A_RECOGER")->valuestring;
						char *item44 = cJSON_GetObjectItem(interr,"FINAL_WINCHE_B_ESTIRAR")->valuestring;
						char *item45 = cJSON_GetObjectItem(interr,"FINAL_WINCHE_B_RECOGER")->valuestring;
						char *item46 = cJSON_GetObjectItem(interr,"FINAL_WINCHE_C_ESTIRAR")->valuestring;
						char *item47 = cJSON_GetObjectItem(interr,"FINAL_WINCHE_C_RECOGER")->valuestring;
						char *item48 = cJSON_GetObjectItem(interr,"WINCHE_A")->valuestring;
						char *item49 = cJSON_GetObjectItem(interr,"WINCHE_B")->valuestring;
						char *item50 = cJSON_GetObjectItem(interr,"WINCHE_C")->valuestring;
						char *item51 = cJSON_GetObjectItem(interr,"NIEBLA")->valuestring;
						char *item52 = cJSON_GetObjectItem(interr,"COMPRESOR")->valuestring;
						char *item53 = cJSON_GetObjectItem(interr,"TOLDO_SUPERIOR")->valuestring;
						char *item54 = cJSON_GetObjectItem(interr,"TOLDO_LATERAL")->valuestring;
						char *item55 = cJSON_GetObjectItem(interr,"MALETERO")->valuestring;
						char *item56 = cJSON_GetObjectItem(interr,"COMPRESOR_NEUMA")->valuestring;

						cJSON *analog = cJSON_GetObjectItem(root2,"ANALOGICOS");
						char *item57 = cJSON_GetObjectItem(analog,"BATERIA_A_VOL")->valuestring;
						char *item58 = cJSON_GetObjectItem(analog,"BATERIA_A_CUR")->valuestring;
						char *item59 = cJSON_GetObjectItem(analog,"BATERIA_B_VOL")->valuestring;
						char *item60 = cJSON_GetObjectItem(analog,"BATERIA_B_CUR")->valuestring;
						char *item61 = cJSON_GetObjectItem(analog,"BATERIA_C_VOL")->valuestring;
						char *item62 = cJSON_GetObjectItem(analog,"BATERIA_C_CUR")->valuestring;
						char *item63 = cJSON_GetObjectItem(analog,"BATERIA_INTERNA_1")->valuestring;
						char *item64 = cJSON_GetObjectItem(analog,"ACELERADOR_1")->valuestring;
						char *item65 = cJSON_GetObjectItem(analog,"ACELERADOR_2")->valuestring;
						char *item66 = cJSON_GetObjectItem(analog,"ACELERADOR_3")->valuestring;
						char *item67 = cJSON_GetObjectItem(analog,"ACELERADOR_4")->valuestring;

						cJSON *tempo = cJSON_GetObjectItem(root2,"TEMPORIZADOR");
						char *item68 = cJSON_GetObjectItem(tempo,"TEMPORIZADOR_1")->valuestring;
						char *item69 = cJSON_GetObjectItem(tempo,"TEMPORIZADOR_2")->valuestring;
						char *item70 = cJSON_GetObjectItem(tempo,"TEMPORIZADOR_3")->valuestring;
						char *item71 = cJSON_GetObjectItem(tempo,"TEMPORIZADOR_4")->valuestring;
						char *item72 = cJSON_GetObjectItem(tempo,"TEMPORIZADOR_5")->valuestring;
						char *item73 = cJSON_GetObjectItem(tempo,"TEMPORIZADOR_6")->valuestring;
						char *item74 = cJSON_GetObjectItem(tempo,"TEMPORIZADOR_7")->valuestring;
						char *item75 = cJSON_GetObjectItem(tempo,"TEMPORIZADOR_8")->valuestring;
						char *item76 = cJSON_GetObjectItem(tempo,"TEMPORIZADOR_9")->valuestring;
						char *item77 = cJSON_GetObjectItem(tempo,"TEMPORIZADOR_10")->valuestring;

						cJSON *tanq = cJSON_GetObjectItem(root2,"TANQUES");
						char *item78 = cJSON_GetObjectItem(tanq,"FRESHWATER")->valuestring;
						char *item79 = cJSON_GetObjectItem(tanq,"BLACKWATER")->valuestring;
						char *item80 = cJSON_GetObjectItem(tanq,"GREYWATER")->valuestring;
						char *item81 = cJSON_GetObjectItem(tanq,"OILOTHER")->valuestring;
						char *item82 = cJSON_GetObjectItem(tanq,"BALLAST")->valuestring;
						char *item83 = cJSON_GetObjectItem(tanq,"SLUDGE")->valuestring;
						char *item84 = cJSON_GetObjectItem(tanq,"BILGE")->valuestring;
						char *item85 = cJSON_GetObjectItem(tanq,"SEAWATER")->valuestring;
						char *item86 = cJSON_GetObjectItem(tanq,"GASOLINA_DISP")->valuestring;

						cJSON *sysgo = cJSON_GetObjectItem(root2,"SYSGO");
						char *item87 = cJSON_GetObjectItem(sysgo,"SYSGO_1")->valuestring;
						char *item88 = cJSON_GetObjectItem(sysgo,"SYSGO_2")->valuestring;
						char *item89 = cJSON_GetObjectItem(sysgo,"SYSGO_3")->valuestring;
						char *item90 = cJSON_GetObjectItem(sysgo,"SYSGO_4")->valuestring;
						char *item91 = cJSON_GetObjectItem(sysgo,"SYSGO_5")->valuestring;

						cJSON *suspe = cJSON_GetObjectItem(root2,"SUSPENSIONES");
						char *item92 = cJSON_GetObjectItem(suspe,"RIGDELAN")->valuestring;
						char *item93 = cJSON_GetObjectItem(suspe,"RIGTRAS")->valuestring;
						char *item94 = cJSON_GetObjectItem(suspe,"RIGTODO")->valuestring;
						char *item95 = cJSON_GetObjectItem(suspe,"ALTDELAN")->valuestring;
						char *item96 = cJSON_GetObjectItem(suspe,"ALTTRAS")->valuestring;
						char *item97 = cJSON_GetObjectItem(suspe,"ALTTODO")->valuestring;
						char *item98 = cJSON_GetObjectItem(suspe,"RIBDELAN")->valuestring;
						char *item99 = cJSON_GetObjectItem(suspe,"RIBTRAS")->valuestring;
						char *item100 = cJSON_GetObjectItem(suspe,"RIBTODO")->valuestring;

						cJSON *gnss = cJSON_GetObjectItem(root2,"GNSS");
						char *item101 = cJSON_GetObjectItem(gnss,"GNSS_GGA")->valuestring;
						char *item102 = cJSON_GetObjectItem(gnss,"GNSS_VEL")->valuestring;
						char *item103 = cJSON_GetObjectItem(gnss,"GNSS_DIST")->valuestring;

						cJSON *rpms = cJSON_GetObjectItem(root2,"RPMS");
						char *item104 = cJSON_GetObjectItem(rpms,"VENTILADOR_A_RPM")->valuestring;
						char *item105 = cJSON_GetObjectItem(rpms,"VENTILADOR_B_RPM")->valuestring;
						char *item106 = cJSON_GetObjectItem(rpms,"VENTILADOR_C_RPM")->valuestring;
						char *item107 = cJSON_GetObjectItem(rpms,"RPM_DISP")->valuestring;

						cJSON *neuma = cJSON_GetObjectItem(root2,"NEUMA");
						char *item108 = cJSON_GetObjectItem(neuma,"NEUMA_1")->valuestring;
						char *item109 = cJSON_GetObjectItem(neuma,"NEUMA_2")->valuestring;
						char *item110 = cJSON_GetObjectItem(neuma,"NEUMA_3")->valuestring;
						char *item111 = cJSON_GetObjectItem(neuma,"NEUMA_4")->valuestring;
						char *item112 = cJSON_GetObjectItem(neuma,"NEUMA_5")->valuestring;
						char *item113 = cJSON_GetObjectItem(neuma,"NEUMA_6")->valuestring;
						char *item114 = cJSON_GetObjectItem(neuma,"NEUMA_7")->valuestring;
						char *item115 = cJSON_GetObjectItem(neuma,"NEUMA_8")->valuestring;
						char *item116 = cJSON_GetObjectItem(neuma,"NEUMA_9")->valuestring;
						char *item117 = cJSON_GetObjectItem(neuma,"NEUMA_10")->valuestring;
						char *item118 = cJSON_GetObjectItem(neuma,"NEUMA_11")->valuestring;
						char *item119 = cJSON_GetObjectItem(neuma,"NEUMA_12")->valuestring;
						char *item120 = cJSON_GetObjectItem(neuma,"NEUMA_13")->valuestring;
						char *item121 = cJSON_GetObjectItem(neuma,"NEUMA_14")->valuestring;
						char *item122 = cJSON_GetObjectItem(neuma,"NEUMA_15")->valuestring;
						char *item123 = cJSON_GetObjectItem(neuma,"NEUMA_16")->valuestring;

						cJSON *incli = cJSON_GetObjectItem(root2,"INCLINACIONES");
						char *item124 = cJSON_GetObjectItem(incli,"GIROS_DISP")->valuestring;
						char *item125 = cJSON_GetObjectItem(incli,"INCLINACION_D_R_P_Y")->valuestring;

						cJSON *indig = cJSON_GetObjectItem(root2,"INDIGITALES");
						char *item126 = cJSON_GetObjectItem(indig,"ALARMA_BOTON_PANICO")->valuestring;
						char *item127 = cJSON_GetObjectItem(indig,"INPUT_DIGITAL_1")->valuestring;

						cJSON *otros = cJSON_GetObjectItem(root2,"OTROS");
						char *item128 = cJSON_GetObjectItem(otros,"MARCHA_TRANSM")->valuestring;
						char *item129 = cJSON_GetObjectItem(otros,"TORQUE_DISP")->valuestring;
						char *item130 = cJSON_GetObjectItem(otros,"POTEN_DISP")->valuestring;
						char *item131 = cJSON_GetObjectItem(otros,"VELOCIDAD_DISP")->valuestring;
						char *item132 = cJSON_GetObjectItem(otros,"MSNM_DISP")->valuestring;
						char *item133 = cJSON_GetObjectItem(otros,"SHIFT_DISP")->valuestring;

					int result = strcmp(token_httser, token);
					if(result==0)//login correcto
					  {

						grabar_nvs("1_PRESCOM",item1);
						grabar_nvs("1_PRESION_DISP",item2);
						grabar_nvs("1_PRESDELAN",item3);
						grabar_nvs("1_PRESTRAS",item4);
						grabar_nvs("1_PRESACEI",item5);
						grabar_nvs("1_TEMPTRANS",item6);
						grabar_nvs("1_TEMPTURBO",item7);
						grabar_nvs("1_TEMPESCAP",item8);
						grabar_nvs("1_TEMPREFRI",item9);
						grabar_nvs("1_TEMPACE",item10);
						grabar_nvs("1_VADO_A_T",item11);
						grabar_nvs("1_VDO_B_T",item12);
						grabar_nvs("1_VEO_C_T",item13);
						grabar_nvs("1_CLCIO",item14);
						grabar_nvs("1_NEVERA",item15);
						grabar_nvs("1_TEMPAMBINT",item16);
						grabar_nvs("1_TEMPAMBEXT",item17);
						grabar_nvs("1_RESHELADAGUA",item18);
						grabar_nvs("1_TEMP_TRANSM",item19);
						grabar_nvs("1_TEMPAMB_DISP",item20);
						grabar_nvs("1_TEMPCABIN_DIS",item21);
						grabar_nvs("1_VENTILADOR_A",item22);
						grabar_nvs("1_VENTILADOR_B",item23);
						grabar_nvs("1_VENTILADOR_C",item24);
						grabar_nvs("1_INTERRUPTOR_C",item25);
						grabar_nvs("1_INTERRUPTOR_V",item26);
						grabar_nvs("1_INTERRUPTOR_N",item27);
						grabar_nvs("1_TODO_TRANSM",item28);
						grabar_nvs("1_FRONTAL_TRA",item29);
						grabar_nvs("1_CENTRAL_TRANS",item30);
						grabar_nvs("1_TRASERO_TRANS",item31);
						grabar_nvs("1_DESBLOQUEADO",item32);
						grabar_nvs("1_FGD_TRANSM",item33);
						grabar_nvs("1_RGD_TRANSM",item34);
						grabar_nvs("1_AGD_TRANSM",item35);
						grabar_nvs("1_WINCHE_A_ES",item36);
						grabar_nvs("1_WINCHE_A_RE",item37);
						grabar_nvs("1_WINCHE_B_ES",item38);
						grabar_nvs("1_WINCHE_B_RE",item39);
						grabar_nvs("1_WINCHE_C_ES",item40);
						grabar_nvs("1_WINCHE_C_RE",item41);
						grabar_nvs("1_FIN_WINC_A_ES",item42);
						grabar_nvs("1_FIN_WINC_A_RE",item43);
						grabar_nvs("1_FIN_WINC_B_ES",item44);
						grabar_nvs("1_FIN_WINC_B_RE",item45);
						grabar_nvs("1_FIN_WINC_C_ES",item46);
						grabar_nvs("1_FIN_WINC_C_RE",item47);
						grabar_nvs("1_WINCHE_A",item48);
						grabar_nvs("1_WINCHE_B",item49);
						grabar_nvs("1_WINCHE_C",item50);
						grabar_nvs("1_NIEBLA",item51);
						grabar_nvs("1_COMPRESOR",item52);
						grabar_nvs("1_TOLDO_SUPERIOR",item53);
						grabar_nvs("1_TOLDO_LATERAL",item54);
						grabar_nvs("1_MALETERO",item55);
						grabar_nvs("1_COMPRESOR_NE",item56);
						grabar_nvs("1_BATERIA_A_VO",item57);
						grabar_nvs("1_BATERIA_A_CU",item58);
						grabar_nvs("1_BATERIA_B_VO",item59);
						grabar_nvs("1_BATERIA_B_CU",item60);
						grabar_nvs("1_BATERIA_C_VO",item61);
						grabar_nvs("1_BATERIA_C_CU",item62);
						grabar_nvs("1_BATERIA_INT1",item63);
						grabar_nvs("1_ACELERADOR_1",item64);
						grabar_nvs("1_ACELERADOR_2",item65);
						grabar_nvs("1_ACELERADOR_3",item66);
						grabar_nvs("1_ACELERADOR_4",item67);
						grabar_nvs("1_TEMPORIZADOR1",item68);
						grabar_nvs("1_TEMPORIZADOR2",item69);
						grabar_nvs("1_TEMPORIZADOR3",item70);
						grabar_nvs("1_TEMPORIZADOR4",item71);
						grabar_nvs("1_TEMPORIZADOR5",item72);
						grabar_nvs("1_TEMPORIZADOR6",item73);
						grabar_nvs("1_TEMPORIZADOR7",item74);
						grabar_nvs("1_TEMPORIZADOR8",item75);
						grabar_nvs("1_TEMPORIZADOR9",item76);
						grabar_nvs("1_TEMPORIZADO10",item77);
						grabar_nvs("1_FRESHWATER",item78);
						grabar_nvs("1_BLACKWATER",item79);
						grabar_nvs("1_GREYWATER",item80);
						grabar_nvs("1_OILOTHER",item81);
						grabar_nvs("1_BALLAST",item82);
						grabar_nvs("1_SLUDGE",item83);
						grabar_nvs("1_BILGE",item84);
						grabar_nvs("1_SEAWATER",item85);
						grabar_nvs("1_GASOLINA_DISP",item86);
						grabar_nvs("1_SYSGO_1",item87);
						grabar_nvs("1_SYSGO_2",item88);
						grabar_nvs("1_SYSGO_3",item89);
						grabar_nvs("1_SYSGO_4",item90);
						grabar_nvs("1_SYSGO_5",item91);
						grabar_nvs("1_RIGDELAN",item92);
						grabar_nvs("1_RIGTRAS",item93);
						grabar_nvs("1_RIGTODO",item94);
						grabar_nvs("1_ALTDELAN",item95);
						grabar_nvs("1_ALTTRAS",item96);
						grabar_nvs("1_ALTTODO",item97);
						grabar_nvs("1_RIBDELAN",item98);
						grabar_nvs("1_RIBTRAS",item99);
						grabar_nvs("1_RIBTODO",item100);
						grabar_nvs("1_GNSS_GGA",item101);
						grabar_nvs("1_GNSS_VEL",item102);
						grabar_nvs("1_GNSS_DIST",item103);
						grabar_nvs("1_VENTILADO_A_R",item104);
						grabar_nvs("1_VENTILADO_B_R",item105);
						grabar_nvs("1_VENTILADO_C_R",item106);
						grabar_nvs("1_RPM_DISP",item107);
						grabar_nvs("1_NEUMA_1",item108);
						grabar_nvs("1_NEUMA_2",item109);
						grabar_nvs("1_NEUMA_3",item110);
						grabar_nvs("1_NEUMA_4",item111);
						grabar_nvs("1_NEUMA_5",item112);
						grabar_nvs("1_NEUMA_6",item113);
						grabar_nvs("1_NEUMA_7",item114);
						grabar_nvs("1_NEUMA_8",item115);
						grabar_nvs("1_NEUMA_9",item116);
						grabar_nvs("1_NEUMA_10",item117);
						grabar_nvs("1_NEUMA_11",item118);
						grabar_nvs("1_NEUMA_12",item119);
						grabar_nvs("1_NEUMA_13",item120);
						grabar_nvs("1_NEUMA_14",item121);
						grabar_nvs("1_NEUMA_15",item122);
						grabar_nvs("1_NEUMA_16",item123);
						grabar_nvs("1_GIROS_DISP",item124);
						grabar_nvs("1_INCLINACION_D",item125);
						grabar_nvs("1_ALARMA_BOTON",item126);
						grabar_nvs("1_INPUT_DIG1",item127);
						grabar_nvs("1_MARCHA_TRANSM",item128);
						grabar_nvs("1_TORQUE_DISP",item129);
						grabar_nvs("1_POTEN_DISP",item130);
						grabar_nvs("1_VELOCIDAD_DIS",item131);
						grabar_nvs("1_MSNM_DISP",item132);
						grabar_nvs("1_SHIFT_DISP",item133);


						ESP_LOGW(TAGL, "Data HTTP saved");
						responhttp=trama_endp( "SETPORTS", "msg","Data saved..");
						}
					else{
						ESP_LOGE(TAGL, "ERROR DE LOGIN");
						responhttp=trama_endp( "SETPORTS", "msg","Error token..");
					}
					cJSON_Delete(root2);
		          }
		       }//else
	end:
	   httpd_resp_set_status(req,HTTPD_200);
	   httpd_resp_sendstr(req, responhttp);
	   // End response
	   httpd_resp_send_chunk(req, NULL, 0);


	    return ESP_OK;
}

static const httpd_uri_t setports_uri = {
    .uri       = "/SETPORTS",
    .method    = HTTP_POST,
    .handler   = setports_handler,
    .user_ctx  = NULL
};

static esp_err_t webb_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	responhttp=trama_endp("WEBBUFFER", "DATA",get_webbuffer());

	httpd_resp_set_status(req,HTTPD_200);
	httpd_resp_sendstr(req, responhttp);

	// End response
	httpd_resp_send_chunk(req, NULL, 0);
	return ESP_OK;
}

static const httpd_uri_t webb_uri = {
    .uri       = "/WEBBUFFER",
    .method    = HTTP_POST,
    .handler   = webb_handler,
    .user_ctx  = NULL
};

static esp_err_t temp_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	responhttp=trama_endp("TEMP", "DATA",get_temp());

	httpd_resp_set_status(req,HTTPD_200);
	httpd_resp_sendstr(req, responhttp);

	// End response
	httpd_resp_send_chunk(req, NULL, 0);
	return ESP_OK;
}

static const httpd_uri_t temp_uri = {
    .uri       = "/TEMP",
    .method    = HTTP_POST,
    .handler   = temp_handler,
    .user_ctx  = NULL
};

static esp_err_t transm_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	responhttp=trama_endp("TRANSM", "DATA",get_transm());

	httpd_resp_set_status(req,HTTPD_200);
	httpd_resp_sendstr(req, responhttp);

	// End response
	httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t transm_uri = {
    .uri       = "/TRANSM",
    .method    = HTTP_POST,
    .handler   = transm_handler,
    .user_ctx  = NULL
};

static esp_err_t tanq_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	responhttp=trama_endp("TANQ", "DATA",get_tanq());

	httpd_resp_set_status(req,HTTPD_200);
	httpd_resp_sendstr(req, responhttp);

	// End response
	httpd_resp_send_chunk(req, NULL, 0);
	return ESP_OK;
}

static const httpd_uri_t tanq_uri = {
    .uri       = "/TANQ",
    .method    = HTTP_POST,
    .handler   = tanq_handler,
    .user_ctx  = NULL
};

static esp_err_t disp_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	responhttp=trama_endp("DISP", "DATA",get_disp());

	httpd_resp_set_status(req,HTTPD_200);
	httpd_resp_sendstr(req, responhttp);

	// End response
	httpd_resp_send_chunk(req, NULL, 0);
	return ESP_OK;
}

static const httpd_uri_t disp_uri = {
    .uri       = "/DISP",
    .method    = HTTP_POST,
    .handler   = disp_handler,
    .user_ctx  = NULL
};

static esp_err_t bat_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	responhttp=trama_endp("BAT", "DATA",get_bat());

	httpd_resp_set_status(req,HTTPD_200);
	httpd_resp_sendstr(req, responhttp);

	// End response
	httpd_resp_send_chunk(req, NULL, 0);
	return ESP_OK;
}

static const httpd_uri_t bat_uri = {
    .uri       = "/BAT",
    .method    = HTTP_POST,
    .handler   = bat_handler,
    .user_ctx  = NULL
};

static esp_err_t motor_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	responhttp=trama_endp("MOTOR", "DATA",get_motor());

	httpd_resp_set_status(req,HTTPD_200);
	httpd_resp_sendstr(req, responhttp);

	// End response
	httpd_resp_send_chunk(req, NULL, 0);
	return ESP_OK;
}

static const httpd_uri_t motor_uri = {
    .uri       = "/MOTOR",
    .method    = HTTP_POST,
    .handler   = motor_handler,
    .user_ctx  = NULL
};

static esp_err_t interup_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	responhttp=trama_endp("INTERUP","DATA", get_interup());

	httpd_resp_set_status(req,HTTPD_200);
	httpd_resp_sendstr(req, responhttp);

	// End response
	httpd_resp_send_chunk(req, NULL, 0);
	return ESP_OK;
}

static const httpd_uri_t interup_uri = {
    .uri       = "/INTERUP",
    .method    = HTTP_POST,
    .handler   = interup_handler,
    .user_ctx  = NULL
};

static esp_err_t tempo_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	responhttp=trama_endp("TEMPO", "DATA",get_tempo());

	httpd_resp_set_status(req,HTTPD_200);
	httpd_resp_sendstr(req, responhttp);

	// End response
	httpd_resp_send_chunk(req, NULL, 0);
	return ESP_OK;
}

static const httpd_uri_t tempo_uri = {
    .uri       = "/TEMPO",
    .method    = HTTP_POST,
    .handler   = tempo_handler,
    .user_ctx  = NULL
};

static esp_err_t gnss_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	responhttp=trama_endp("GNSS", "DATA",get_gnss());

	httpd_resp_set_status(req,HTTPD_200);
	httpd_resp_sendstr(req, responhttp);

	// End response
	httpd_resp_send_chunk(req, NULL, 0);
	return ESP_OK;
}

static const httpd_uri_t gnss_uri = {
    .uri       = "/GNSS",
    .method    = HTTP_POST,
    .handler   = gnss_handler,
    .user_ctx  = NULL
};

static esp_err_t ds18b20_handler(httpd_req_t *req)
{
	char *responhttp=NULL;
	responhttp=trama_endp("DS18B20", "DATA",get_ds18b20());

	httpd_resp_set_status(req,HTTPD_200);
	httpd_resp_sendstr(req, responhttp);

	// End response
	httpd_resp_send_chunk(req, NULL, 0);
	return ESP_OK;
}

static const httpd_uri_t ds18b20_uri = {
    .uri       = "/DS18B20",
    .method    = HTTP_POST,
    .handler   = ds18b20_handler,
    .user_ctx  = NULL
};
/*
static esp_err_t _handler(httpd_req_t *req)
{
	char *responhttp=NULL;


	       httpd_resp_set_status(req,HTTPD_200);
		httpd_resp_sendstr(req, responhttp);


	           // End response
	           httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static const httpd_uri_t _uri = {
    .uri       = "/",
    .method    = HTTP_POST,
    .handler   = _handler,
    .user_ctx  = NULL
};

static esp_err_t _handler(httpd_req_t *req)
{
	char *responhttp=NULL;


	       httpd_resp_set_status(req,HTTPD_200);
		httpd_resp_sendstr(req, responhttp);


	           // End response
	           httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static const httpd_uri_t _uri = {
    .uri       = "/",
    .method    = HTTP_POST,
    .handler   = _handler,
    .user_ctx  = NULL
};

static esp_err_t _handler(httpd_req_t *req)
{
	char *responhttp=NULL;


	       httpd_resp_set_status(req,HTTPD_200);
		httpd_resp_sendstr(req, responhttp);


	           // End response
	           httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static const httpd_uri_t _uri = {
    .uri       = "/",
    .method    = HTTP_POST,
    .handler   = _handler,
    .user_ctx  = NULL
};

static esp_err_t _handler(httpd_req_t *req)
{
	char *responhttp=NULL;


	       httpd_resp_set_status(req,HTTPD_200);
		httpd_resp_sendstr(req, responhttp);


	           // End response
	           httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static const httpd_uri_t _uri = {
    .uri       = "/",
    .method    = HTTP_POST,
    .handler   = _handler,
    .user_ctx  = NULL
};

static esp_err_t _handler(httpd_req_t *req)
{
	char *responhttp=NULL;


	       httpd_resp_set_status(req,HTTPD_200);
		httpd_resp_sendstr(req, responhttp);


	           // End response
	           httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static const httpd_uri_t _uri = {
    .uri       = "/",
    .method    = HTTP_POST,
    .handler   = _handler,
    .user_ctx  = NULL
};*/
/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/hello", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    } else if (strcmp("/echo", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

/* An HTTP PUT handler. This demonstrates realtime
 * registration and deregistration of URI handlers
 */
static esp_err_t ctrl_put_handler(httpd_req_t *req)
{
    char buf;
    int ret;

    if ((ret = httpd_req_recv(req, &buf, 1)) <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    if (buf == '0') {
        /* URI handlers can be unregistered using the uri string */
        ESP_LOGI(TAGL, "Unregistering /hello and /echo URIs");
        httpd_unregister_uri(req->handle, "/hello");
        httpd_unregister_uri(req->handle, "/echo");
        /* Register the custom error handler */
        httpd_register_err_handler(req->handle, HTTPD_404_NOT_FOUND, http_404_error_handler);
    }
    else {
        ESP_LOGI(TAGL, "Registering /hello and /echo URIs");
      //  httpd_register_uri_handler(req->handle, &hello);
        httpd_register_uri_handler(req->handle, &echo);
        /* Unregister custom error handler */
        httpd_register_err_handler(req->handle, HTTPD_404_NOT_FOUND, NULL);
    }

    /* Respond with empty body */
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t ctrl = {
    .uri       = "/ctrl",
    .method    = HTTP_PUT,
    .handler   = ctrl_put_handler,
    .user_ctx  = NULL
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 21;//Cantidad de uri maximas
    config.lru_purge_enable = true;
    config.stack_size=8096;
    config.recv_wait_timeout  = 30;                       //<---- This one here
    config.send_wait_timeout  = 30;

    // Start the httpd server
    ESP_LOGI(TAGL, "Starting server on port: '%d'", config.server_port);

    token_httser= leer_nvs("5_TOK-HTTPSER");
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAGL, "Registering URI handlers");
        httpd_register_uri_handler(server, &version_uri);
        httpd_register_uri_handler(server, &resetesp_uri);
        httpd_register_uri_handler(server, &trama_uri);
        httpd_register_uri_handler(server, &server_uri);
        httpd_register_uri_handler(server, &wifist_uri);
        httpd_register_uri_handler(server, &wifiap_uri);
        httpd_register_uri_handler(server, &sethttp_uri);
        httpd_register_uri_handler(server, &setports_uri);
        httpd_register_uri_handler(server, &defaultesp_uri);
	    httpd_register_uri_handler(server, &transm_uri);
	    httpd_register_uri_handler(server, &webb_uri);
	    httpd_register_uri_handler(server, &temp_uri);
	    httpd_register_uri_handler(server, &tanq_uri);
	    httpd_register_uri_handler(server, &disp_uri);
		httpd_register_uri_handler(server, &bat_uri);
		httpd_register_uri_handler(server, &motor_uri);
		httpd_register_uri_handler(server, &interup_uri);
		httpd_register_uri_handler(server, &tempo_uri);
	    httpd_register_uri_handler(server, &gnss_uri);
	    httpd_register_uri_handler(server, &ds18b20_uri);
        httpd_register_uri_handler(server, &echo);
        httpd_register_uri_handler(server, &ctrl);
        #if CONFIG_EXAMPLE_BASIC_AUTH
        httpd_register_basic_auth(server);
        #endif
        return server;
    }

    ESP_LOGI(TAGL, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAGL, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAGL, "Starting webserver");
        *server = start_webserver();
    }
}




#endif /* MAIN_LOCAL_HTTP_SERVER_H_ */
