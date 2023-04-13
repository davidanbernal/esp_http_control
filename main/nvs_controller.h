#ifndef NVS_CONTROLLER_H
#define NVS_CONTROLLER_H

#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_log.h"
#include "string.h"
#include <stdlib.h>
#include "nvs_flash.h"


//MACROS RELACIONADOS A LA LECTURA DE LA NVS
//#define ERASE_FLASH_MEMORY_PIN GPIO_NUM_22
#define KEY_NOT_ALLOCATED 0XFF //No existe el puerto 255 por lo que si la funcion de lectura retorna este valor, significa que la llave no tiene puerto asignado
#define ERROR_READING_NVS 0XFE //No existe el puerto 254 por lo que si la función de lectura retorna este valor, significa que hubo un error leyendo la nvs
#define RESET_COUNTER_KEY 254 //Leer y escribir en la NVS el literal 254 me permite acceder al contador de resets que lleva la tarjeta en el intento de flashear la OTA de la STM32
#define ERASE_FLASH_MEMORY_PIN 12 //SOLO CON MODULOS


const char* llaves_tarjeta[] = {
    "1_PRESACEI",
    "1_TEMPTRANS",
    "1_TEMPTURBO",
    "1_TEMPESCAP",
    "1_TEMPREFRI",
    "1_TEMPACE",
    "1_PRESCOM",
	"1_VENTILADOR_A",
    "1_VENTILADOR_B",
    "1_VENTILADOR_C",
    "1_VENTILADO_A_T",
    "1_VENTILADO_A_R",
    "1_VENTILADO_B_T",
    "1_VENTILADO_B_R",
    "1_VENTILADO_C_T",
    "1_VENTILADO_C_R",
    "1_CLIMATIZACIO",
    "1_NEVERA",
    "1_TEMPAMBINT",
    "1_TEMPAMBEXT",
    "1_RESHELADAGUA",
    "1_INTERRUPTOR_C",
    "1_INTERRUPTOR_V",
    "1_INTERRUPTOR_N",
    "1_BATERIA_A_VO",
    "1_BATERIA_A_CU",
    "1_BATERIA_B_VO",
    "1_BATERIA_B_CU",
    "1_BATERIA_C_VO",
    "1_BATERIA_C_CU",
    "1_FRESHWATER",
    "1_BLACKWATER",
    "1_GREYWATER",
    "1_OILOTHER",
    "1_BALLAST",
    "1_SLUDGE",
    "1_BILGE",
    "1_SEAWATER",
    "1_TEMP_TRANSM",
    "1_MARCHA_TRANSM",
    "1_TODO_TRANSM",
    "1_FRONTAL_TRA",
    "1_CENTRAL_TRANS",
    "1_TRASERO_TRANS",
    "1_DESBLOQUEADO_",
    "1_FGD_TRANSM",
    "1_RGD_TRANSM",
    "1_AGD_TRANSM",
    "1_WINCHE_A",
    "1_WINCHE_B",
    "1_WINCHE_C",
    "1_WINCHE_A_ES",
    "1_WINCHE_A_RE",
    "1_WINCHE_B_ES",
    "1_WINCHE_B_RE",
    "1_WINCHE_C_ES",
    "1_WINCHE_C_RE",
    "1_FIN_WINC_A_ES",
    "1_FIN_WINC_A_RE",
    "1_FIN_WINC_B_ES",
    "1_FIN_WINC_B_RE",
    "1_FIN_WINC_C_ES",
    "1_FIN_WINC_C_RE",
    "1_NIEBLA",
    "1_COMPRESOR",
    "1_TOLDO_SUPERIOR",
    "1_TOLDO_LATERAL",
    "1_MALETERO",
    "1_TEMPORIZADOR1",
    "1_TEMPORIZADOR2",
    "1_TEMPORIZADOR3",
    "1_TEMPORIZADOR4",
    "1_TEMPORIZADOR5",
    "1_TEMPORIZADOR6",
    "1_TEMPORIZADOR7",
    "1_TEMPORIZADOR8",
    "1_TEMPORIZADOR9",
    "1_TEMPORIZADO10",
    "1_TEMPAMB_DISP",
    "1_TEMPCABIN_DIS",
    "1_GIROS_DISP",
    "1_PRESION_DISP",
    "1_GASOLINA_DISP",
    "1_TORQUE_DISP",
    "1_POTEN_DISP",
    "1_RPM_DISP",
    "1_VELOCIDAD_DIS",
    "1_INCLINACION_D",
    "1_MSNM_DISP",
    "1_SHIFT_DISP",
    "1_PRESDELAN",
    "1_PRESTRAS",
    "1_RIGDELAN",
    "1_RIGTRAS",
    "1_RIGTODO",
    "1_ALTDELAN",
    "1_ALTTRAS",
    "1_ALTTODO",
    "1_RIBDELAN",
    "1_RIBTRAS",
    "1_RIBTODO",
    "1_NEUMA_1",
    "1_NEUMA_2",
    "1_NEUMA_3",
    "1_NEUMA_4",
    "1_NEUMA_5",
    "1_NEUMA_6",
    "1_NEUMA_7",
    "1_NEUMA_8",
    "1_NEUMA_9",
    "1_NEUMA_10",
    "1_NEUMA_11",
    "1_NEUMA_12",
    "1_NEUMA_13",
    "1_NEUMA_14",
    "1_NEUMA_15",
    "1_NEUMA_16",
    "1_COMPRESOR_NE",
    "1_GNSS_GGA",
    "1_GNSS_VEL",
    "1_GNSS_DIST",
    "1_ALARMA_BOTON",
    "1_SYSGO_1",
    "1_SYSGO_2",
    "1_SYSGO_3",
    "1_SYSGO_4",
    "1_SYSGO_5",
    "1_BATERIA_INT1",
    "1_INPUT_DIG1",
    "1_ACELERADOR_1",
    "1_ACELERADOR_2",
    "1_ACELERADOR_3",
    "1_ACELERADOR_4",
};


char * readx_nvs(const char * registro){  //FUNCION PARA LEER,
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
    //printf("\n");
    //printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else {
    	 size_t required_size = 0;
    	 nvs_get_str(my_handle, registro, NULL, &required_size);
    	 char *read_value = malloc(required_size);
    	 err = nvs_get_str(my_handle, registro, read_value, &required_size);
         nvs_close(my_handle);
         switch (err) {
           case ESP_OK:
               //printf("Done\n");
               //printf("Dato Leido = %d \n", read_value);
        	   return read_value;
               break;

           case ESP_ERR_NVS_NOT_FOUND:
               //printf("The value is not initialized yet!\n");
               return "ESP_ERR_NVS_NOT_FOUND";
               break;

           default:
        	   //printf("Error (%s) reading!\n", esp_err_to_name(err));
        	   return "ERROR_READING_NVS";
        	   break;
          }
      }
    return "x"; //nunca debería llegar aca
}

uint16_t pre_readx_nvs(uint8_t registro_num){
	uint16_t value;
	char * value_string;
	//printf("Esta es la llave_tarjeta %s", llaves_tarjeta[registro_num-1]);
	value_string = readx_nvs(llaves_tarjeta[registro_num-1]);
	if(strcmp(value_string, "ESP_ERR_NVS_NOT_FOUND") == 0){
	    return KEY_NOT_ALLOCATED;
	}
	else if(strcmp(value_string, "ERROR_READING_NVS") == 0){
	    return ERROR_READING_NVS;
	}
	else{
		value = atoi(value_string);
	}
	return value;
}

void writex_nvs(const char * registro, const char * valor){// el campo registro es como vas a llamar ese valor a guardar y el campo valor para este caso es string pero puedes pasarlo a entero o el que necesites
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
     } else {
        printf("Done\n");
     }

     // Write
	 err = nvs_set_str(my_handle, registro, valor);
     printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

     err = nvs_commit(my_handle);
     printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

     // Close
     //free(registro);
     nvs_close(my_handle);
}

void pre_writex_nvs(uint8_t registro_num, uint16_t valor){
	//printf("%d this is registro_num ", registro_num);
	//printf("This is llaves tarjeta %s", llaves_tarjeta[registro_num-1]);
	char * valor_string = malloc(6*sizeof(char));
	sprintf(valor_string, "%d", valor);
	writex_nvs(llaves_tarjeta[registro_num-1], valor_string);
	free(valor_string);
}

char *leer_nvs(const char* registro){
	 nvs_handle_t my_handle;
	 nvs_open("storage", NVS_READWRITE, &my_handle);
	 size_t required_size = 0;
	nvs_get_str(my_handle, registro, NULL, &required_size);
	char *webdatasd = malloc(required_size);
	nvs_get_str(my_handle, registro, webdatasd, &required_size);
	//printf("Read data: %s\n", webdatasd);
	 nvs_close(my_handle);
	 return webdatasd;

}

void grabar_nvs(const char* registro, const char* dato){
	 nvs_handle_t my_handle;
	 nvs_open("storage", NVS_READWRITE, &my_handle);
	 nvs_set_str(my_handle, registro, dato);
	// printf("Write data: %s\n", dato);
	 nvs_commit(my_handle);
	 nvs_close(my_handle);
}
//Esta función se usa para mapear el puerto, en la NVS el par ''Puerto'': posicion_vector
//no se puede guardar de esa manera porque algunos valores de puertos podrían sobreescribir las llaves
//Esta funcion es una capa adicional que matricula puertos con posicion de vector en la NVS desde la posicion 150
//Como las llaves van desde la 1 hasta la 132, no debería haber problemas de sobreescritura
uint8_t map_port(uint8_t port, const char option){
	if(option == 'm') //mapear
		return (port+150);
	else //des-mapear
		return port - 150;
}






#endif



