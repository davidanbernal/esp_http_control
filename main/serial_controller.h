#ifndef SERIAL_CONTROLLER_H
#define SERIAL_CONTROLLER_H

/* Este controlador contiene todo lo relacionado con el protocolo de comunicaciones que toma los datos
 * provenientes de la comunicacion serial entre los microcontroladores, el almacenado en vectores de estos datos
 * y el posterior armado de la trama web que contiene esta información
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include <stdlib.h>
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "nvs_controller.h"

//Para tarjeta IvAdventure Mini
//#define TXD_PIN (GPIO_NUM_23)
//#define RXD_PIN (GPIO_NUM_22)
//Para modulos de prueba
#define TXD_PIN (GPIO_NUM_23)
#define RXD_PIN (GPIO_NUM_22)
#define amount_of_temperature_sensors 3
#define amount_of_digital_inputs 1
#define amount_of_analog_sensors 8
#define amount_of_tanques_sensors 4
#define amount_of_inclinacion_sensors 1
#define amount_of_sensoresGPS_sensors 1
#define amount_of_interruptores_sensors 2
#define amount_of_ventiladores_sensors 2

static const int RX_BUF_SIZE = 1024;
uint8_t * keys_with_ports;
uint8_t keys_ports_counter = 0;
char *web_buffer; //Esta es la variable que contiene la informacion que va a la web





/* Estas son las estructuras de datos de cada uno de los modulos que se pueden pedir al STM32
 */

typedef struct{ //Estructura temperaturas
    int valor1;
    uint8_t id; //puerto
}temperaturas;

typedef struct{
	int valor1;
	uint8_t id;
}digital_inputs;

typedef struct{ //Estructura analogicos
    int valor1;
    int valor2;
    uint8_t id; //puerto
}analogicos;

typedef struct{ //Estructura tanques
    int valor1;
    int valor2;
    int valor3;
    int valor4;
    int valor5;
    uint8_t id; //puerto
}tanques;

typedef struct{ //Estructura inclinaciones
    int valor1;
    int valor2;
    int valor3;
    uint8_t id; //puerto
}inclinaciones;

typedef struct{ //Estructura GPS
	bool valor1; //true or false
    char * valor2; //timestamp
    float valor3; //latitud
    float valor4; //longitud
    uint16_t valor5; //error de precision
    uint16_t valor6; //altura
    uint16_t valor7; //velocidad
    uint16_t valor8; //angulo azimut
    uint16_t id; //puerto
}GPS;

typedef struct{ //Estructura interruptores
    int valor1;
    uint8_t id; //puerto
}interruptores;

typedef struct{ //Estructura ventiladores
    int valor1;
    uint8_t id; //puerto
}ventiladores;

//Estructura de datos con valores booleanos para verificar si los modulos respondieron
typedef struct{
	bool temperatura;
	bool liquido;
	bool digital_input;
	bool inclinacion;
	bool gps;
	bool analogico;
	bool digital_output;
	bool ventilador;
}modulos_response;

typedef enum { //Tipo solicitud
	READ_PORT = 0,
	WRITE_PORT,
	//El 90% de las solicitudes son para estos dos primeros enums
	READ_UID_STM32,
	SEND_RS485,
	SCAN_CANBUS,
	SCAN_LORA,
	SEND_LORA,
	READ_ID_SLAVE,
	STATUS_INTERRUPTOR,
	STATUS_INTERRUPTOR_INVERT,
	STATUS_ELECTROVENTILADOR,
	SEND_TASK_TIPO_SOLICITUD,
	ALMACENAR_MINIMO_TANQUES,
	ALMACENAR_MAXIMO_TANQUES,
	ALMACENAR_UMBRAL_MAXIMO_TANQUES,
	ALMACENAR_UMBRAL_MINIMO_TANQUES
	//and so on...
}TipoSolicitud;

typedef enum { //modulos
	NONE_MODULE = 0,
	TEMPERATURE_SENSORS,
	HUMEDAD_SENSORS,
	VOLTAJE_SENSORS,
	DIGITAL_INPUT_SENSORS,
	PROXIMIDAD_SENSORS,
	LIQUIDO_SENSORS,
	INCLINACION_SENSORS,
	GPS_SENSORS,
	ANALOGICO_SENSORS,
	TEMPORIZADORES_SENSORS,
	DIGITAL_OUTPUT_SENSORS,
	VENTILADOR_SENSORS,
	UID_RESPONSE
}ModulosApp;

typedef enum { //Llaves IdAdventureMini
	PRESACEI = 1,
	TEMPTRANS,
	TEMPTURBO,
	TEMPESCAP,
	TEMPREFRI,
	TEMPACE,
	PRESCOM,
	VENTILADOR_A,
	VENTILADOR_B,
	VENTILADOR_C,
	VENTILADOR_A_TEMP,
	VENTILADOR_A_RPM,
	VENTILADOR_B_TEMP,
	VENTILADOR_B_RPM,
	VENTILADOR_C_TEMP,
	VENTILADOR_C_RPM,
	CLIMATIZACION,
	NEVERA,
	TEMPAMBINT,
	TEMPAMBEXT,
	RESHELADAGUA,
	INTERRUPTOR_CLIMATIZADOR,
	INTERRUPTOR_VENTILACION,
	INTERRUPTOR_NEVERA,
	BATERIA_A_VOL,
	BATERIA_A_CUR,
	BATERIA_B_VOL,
	BATERIA_B_CUR,
	BATERIA_C_VOL,
	BATERIA_C_CUR,
	FRESHWATER,
	BLACKWATER,
	GREYWATER,
	OILOTHER,
	BALLAST,
	SLUDGE,
	BILGE,
	SEAWATER,
	TEMP_TRANSM,
	MARCHA_TRANSM,
	TODO_TRANSM_DIFERENCIAL,
	FRONTAL_TRANSM_DIFERENCIAL,
	CENTRAL_TRANSM_DIFERENCIAL,
	TRASERO_TRANSM_DIFERENCIAL,
	DESBLOQUEADO_TRANSM_DIFERENCIAL,
	FGD_TRANSM_TRACCION,
	RGD_TRANSM_TRACCION,
	AGD_TRANSM_TRACCION,
	WINCHE_A,
	WINCHE_B,
	WINCHE_C,
	WINCHE_A_ESTIRAR,
	WINCHE_A_RECOGER,
	WINCHE_B_ESTIRAR,
	WINCHE_B_RECOGER,
	WINCHE_C_ESTIRAR,
	WINCHE_C_RECOGER,
	FINAL_WINCHE_A_ESTIRAR,
	FINAL_WINCHE_A_RECOGER,
	FINAL_WINCHE_B_ESTIRAR,
	FINAL_WINCHE_B_RECOGER,
	FINAL_WINCHE_C_ESTIRAR,
	FINAL_WINCHE_C_RECOGER,
	NIEBLA,
	COMPRESOR,
	TOLDO_SUPERIOR,
	TOLDO_LATERAL,
	MALETERO,
	TEMPORIZADOR_1,
	TEMPORIZADOR_2,
	TEMPORIZADOR_3,
	TEMPORIZADOR_4,
	TEMPORIZADOR_5,
	TEMPORIZADOR_6,
	TEMPORIZADOR_7,
	TEMPORIZADOR_8,
	TEMPORIZADOR_9,
	TEMPORIZADOR_10,
	TEMPAMB_DISP,
	TEMPCABIN_DISP,
	GIROS_DISP,
	PRESION_DISP,
	GASOLINA_DISP,
	TORQUE_DISP,
	POTEN_DISP,
	RPM_DISP,
	VELOCIDAD_DISP,
	INCLINACION_DISP_ROLL_PITCH_YAW,
	MSNM_DISP,
	SHIFT_DISP,
	PRESDELAN,
	PRESTRAS,
	RIGDELAN,
	RIGTRAS,
	RIGTODO,
	ALTDELAN,
	ALTTRAS,
	ALTTODO,
	RIBDELAN,
	RIBTRAS,
	RIBTODO,
	NEUMA_1,
	NEUMA_2,
	NEUMA_3,
	NEUMA_4,
	NEUMA_5,
	NEUMA_6,
	NEUMA_7,
	NEUMA_8,
	NEUMA_9,
	NEUMA_10,
	NEUMA_11,
	NEUMA_12,
	NEUMA_13,
	NEUMA_14,
	NEUMA_15,
	NEUMA_16,
	COMPRESOR_NEUMA,
	GNSS_GGA,
	GNSS_VEL,
	GNSS_DIST,
	ALARMA_BOTON_PANICO,
	SYSGO_1,
	SYSGO_2,
	SYSGO_3,
	SYSGO_4,
	SYSGO_5,
	BATERIA_INTERNA_1,
	INPUT_DIGITAL_1,
	ACELERADOR_1,
	ACELERADOR_2,
	ACELERADOR_3,
	ACELERADOR_4,
	AMOUNT_OF_ENUMS_IN_KEYVALUE
}keyValue;

const uart_config_t uart_config_for_protocol = { //EL CONST SIGNIFICA QUE NO SE PUEDEN MODIFICAR LOS CONTENIDOS DE ESTE OBJETO UNA VEZ SE ENCUENTRA INICIALIZADO
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    //.source_clk = UART_SCLK_APB,
};

static EventGroupHandle_t synchronize_serial_coms_group;
const int not_parsing_data = BIT0;
//Variable global de web buffer
char *web_buffer = "|";
//Variables globales de los endpoints
char TRANS[100];
char TEMP[100];
char TANQ[100];
char DISP[100];
char BAT[100];
char MOTOR[100];
char INTERRUP[100];
char TEMPO[100];
char GNSS[100];
char DS18B20[100]="NOT AVAILABLE";

//-------------------------------LLAMADAS  DE VARIABLES-------------
char *get_webbuffer(void){return  web_buffer;}
char *get_transm(void){return TRANS;}
char *get_temp(void){return TEMP;}
char *get_tanq(void){return TANQ;}
char *get_disp(void){return DISP;}
char *get_motor(void){return MOTOR;}
char *get_bat(void){return BAT;}
char *get_interup(void){return INTERRUP;}
char *get_tempo(void){return TEMPO;}
char *get_gnss(void){return GNSS;}
char *get_ds18b20(void){return DS18B20;}
//-----------------------------------------------
temperaturas temperatura[amount_of_temperature_sensors];
digital_inputs digital_input[amount_of_digital_inputs];
analogicos analogico[amount_of_analog_sensors];
tanques tanque[amount_of_tanques_sensors];
inclinaciones inclinacion[amount_of_inclinacion_sensors];
GPS sensorGPS[amount_of_sensoresGPS_sensors];
interruptores interruptor[amount_of_interruptores_sensors];
ventiladores ventilador[amount_of_ventiladores_sensors];
modulos_response verificador;

QueueHandle_t uart_queue;

/*Esta es una funcion cuyo fin es separar una cadena con tokens en un vector de cadenas
 * ejemplo: "cadena1123:cadena212:cadena33" -> ["cadena1123", "cadena212", "cadena33"]
 */
static char ** split_sensors(char *token_copy, uint8_t *i, const char * tok_char){
	static const char *SPLIT_TASK_TAG = "SPLIT_SENSORS";
    esp_log_level_set(SPLIT_TASK_TAG, ESP_LOG_INFO);
	*i = 0;
	char *token_sensors;
	char **token_sensors_array = NULL;
    // Use strtok to split the string into tokens
    token_sensors = strtok(token_copy, tok_char);
    while (token_sensors != NULL) {
        // Allocate memory for the token and copy it
        token_sensors_array = realloc(token_sensors_array, (*i + 1) * sizeof(char *));
        token_sensors_array[*i] = malloc(strlen(token_sensors) + 1);
        strcpy(token_sensors_array[*i], token_sensors);
        // Get the next token
        token_sensors = strtok(NULL, tok_char);
        *i = *i+1;
    }
//    uint8_t q;
//    for(q=0; q<*i; q++){
//    	ESP_LOGI(SPLIT_TASK_TAG, "Splitted Sensor data is: %s and i is %d", token_sensors_array[q], *i);
//    }
    return token_sensors_array;
}


static void endpoint_discriminator(){
	keyValue TRANS_GROUP[] = {TEMP_TRANSM, TODO_TRANSM_DIFERENCIAL, FRONTAL_TRANSM_DIFERENCIAL, CENTRAL_TRANSM_DIFERENCIAL, TRASERO_TRANSM_DIFERENCIAL, DESBLOQUEADO_TRANSM_DIFERENCIAL, FGD_TRANSM_TRACCION, RGD_TRANSM_TRACCION, AGD_TRANSM_TRACCION};
	keyValue TEMP_GROUP[] = {TEMPAMBINT, TEMPAMBEXT, CLIMATIZACION, NEVERA, RESHELADAGUA, INTERRUPTOR_CLIMATIZADOR, INTERRUPTOR_VENTILACION, INTERRUPTOR_NEVERA};
	keyValue TANQ_GROUP[] = {FRESHWATER, BLACKWATER, GREYWATER, OILOTHER, BALLAST, SLUDGE, BILGE, SEAWATER};
	keyValue DISP_GROUP[] = {TEMPAMB_DISP, TEMPCABIN_DISP, PRESION_DISP, INCLINACION_DISP_ROLL_PITCH_YAW, GASOLINA_DISP};
	keyValue BAT_GROUP[] = {BATERIA_A_VOL, BATERIA_A_CUR};
	keyValue MOTOR_GROUP[] = {TEMPTRANS, TEMPTURBO, TEMPESCAP, TEMPREFRI, TEMPACE, PRESACEI, PRESCOM, VENTILADOR_A, VENTILADOR_B, VENTILADOR_C, ACELERADOR_1, ACELERADOR_2, ACELERADOR_3, ACELERADOR_4};
	keyValue INTERUP_GROUP[] = {NIEBLA, COMPRESOR, TOLDO_SUPERIOR, TOLDO_LATERAL, MALETERO};
	keyValue TEMPO_GROUP[] = {TEMPORIZADOR_1, TEMPORIZADOR_2, TEMPORIZADOR_3, TEMPORIZADOR_4, TEMPORIZADOR_5, TEMPORIZADOR_6, TEMPORIZADOR_7, TEMPORIZADOR_8, TEMPORIZADOR_9, TEMPORIZADOR_10};
	keyValue GNSS_GROUP[] = {GNSS_GGA};
	strcpy(TRANS, "|");
	strcpy(TEMP, "|");
	strcpy(TANQ, "|");
	strcpy(DISP, "|");
	strcpy(MOTOR, "|");
	strcpy(BAT, "|");
	strcpy(INTERRUP, "|");
	strcpy(TEMPO, "|");
	strcpy(GNSS, "|");
	char * web_buffer_for_endpoints = malloc(400*sizeof(char));
	strcpy(web_buffer_for_endpoints, web_buffer);
	uint8_t q, j, i, number;
	char ** web_buffer_tokenized = split_sensors(web_buffer_for_endpoints, &q, "|");
	char * buffer = malloc(100*sizeof(char));
	for(j = 0; j < q; j++){
		strcpy(buffer, web_buffer_tokenized[j]);
		//keys_groups = check_if_value_is_in_group(atoi(strtok(web_buffer_tokenized[j], ":")));
		number = atoi(strtok(web_buffer_tokenized[j], ":"));
		//ESP_LOGI("INSIDE THE LOOP", "This is buffer %s, this is the number %d", buffer, number);
		for(i = 0; i < (sizeof(TRANS_GROUP)/sizeof(TRANS_GROUP[0])); i++){
			if(number == TRANS_GROUP[i]){
				strcat(TRANS, buffer);
			    strcat(TRANS, "|");
			}
		}
		for(i = 0; i < (sizeof(TEMP_GROUP)/sizeof(TEMP_GROUP[0])); i++){
			if(number == TEMP_GROUP[i]){
				strcat(TEMP, buffer);
			    strcat(TEMP, "|");
			    //ESP_LOGI("INSIDE THE LOOP", "This is TEMP %s", TEMP);
			}
		}
		for(i = 0; i < (sizeof(TANQ_GROUP)/sizeof(TANQ_GROUP[0])); i++){
			if(number == TANQ_GROUP[i]){
				strcat(TANQ, buffer);
			    strcat(TANQ, "|");
			    //ESP_LOGI("INSIDE THE LOOP", "This is TANQ %s", TANQ);
			}
		}
		for(i = 0; i < (sizeof(DISP_GROUP)/sizeof(DISP_GROUP[0])); i++){
			if(number == DISP_GROUP[i]){
				strcat(DISP, buffer);
			    strcat(DISP, "|");
			    //ESP_LOGI("INSIDE THE LOOP", "This is DISP %s", DISP);
			}
		}
		for(i = 0; i < (sizeof(BAT_GROUP)/sizeof(BAT_GROUP[0])); i++){
			if(number == BAT_GROUP[i]){
				strcat(BAT, buffer);
			    strcat(BAT, "|");
			    //ESP_LOGI("INSIDE THE LOOP", "This is BAT %s", BAT);

			}
		}
		for(i = 0; i < (sizeof(MOTOR_GROUP)/sizeof(MOTOR_GROUP[0])); i++){
			if(number == MOTOR_GROUP[i]){
				strcat(MOTOR, buffer);
			    strcat(MOTOR, "|");
			    //ESP_LOGI("INSIDE THE LOOP", "This is MOTOR %s", MOTOR);

			}
		}
		for(i = 0; i < (sizeof(INTERUP_GROUP)/sizeof(INTERUP_GROUP[0])); i++){
			if(number == INTERUP_GROUP[i]){
				strcat(INTERRUP, buffer);
			    strcat(INTERRUP, "|");
			    //ESP_LOGI("INSIDE THE LOOP", "This is INTERRUP %s", INTERRUP);

			}
		}
		for(i = 0; i < (sizeof(TEMPO_GROUP)/sizeof(TEMPO_GROUP[0])); i++){
			if(number == TEMPO_GROUP[i]){
				strcat(TEMPO, buffer);
			    strcat(TEMPO, "|");
			    //ESP_LOGI("INSIDE THE LOOP", "This is TEMPO %s", TEMPO);

			}
		}
		for(i = 0; i < (sizeof(GNSS_GROUP)/sizeof(GNSS_GROUP[0])); i++){
			if(number == GNSS_GROUP[i]){
				strcat(GNSS, buffer);
			    strcat(GNSS, "|");
			    //ESP_LOGI("INSIDE THE LOOP", "This is GNSS %s", GNSS);
			}
		}
	}
/*	ESP_LOGI("ENDPOINT_DISCRIMINATOR", "This is TEMP %s", TEMP);
	ESP_LOGI("ENDPOINT_DISCRIMINATOR", "This is TRANS %s", TRANS);
	ESP_LOGI("ENDPOINT_DISCRIMINATOR", "This is TANQ %s", TANQ);
	ESP_LOGI("ENDPOINT_DISCRIMINATOR", "This is DISP %s", DISP);
	ESP_LOGI("ENDPOINT_DISCRIMINATOR", "This is BAT %s", BAT);
	ESP_LOGI("ENDPOINT_DISCRIMINATOR", "This is MOTOR %s", MOTOR);
	ESP_LOGI("ENDPOINT_DISCRIMINATOR", "This is INTERRUP %s", INTERRUP);
	ESP_LOGI("ENDPOINT_DISCRIMINATOR", "This is TEMPO %s", TEMPO);
	ESP_LOGI("ENDPOINT_DISCRIMINATOR", "This is GNSS %s", GNSS);*/
	free(buffer);
	free(web_buffer_for_endpoints);
}

//Esta informacion depende netamente de la licencia
static uint8_t vector_positions(uint8_t port_associated){
	switch(port_associated){
		//Temperaturas
		case 0:
			return 0;
		case 1:
			return 1;
		case 2:
			return 2;

		//Tanques
		case 5:
			return 0;
		case 6:
			return 1;
		case 7:
			return 2;
		case 8:
			return 3;

		//Inclinaciones
		case 9:
			return 0;

		//Analogicos
		case 10:
			return 0;
		case 11:
			return 1;
		case 12:
			return 2;
		case 13:
			return 3;
		case 14:
		 	return 4;
		case 15:
			return 5;
		case 16:
			return 6;
		case 17:
			return 7;

		//GPS
		case 18:
			return 0;

		//Ventiladores
		case 40:
			return 0;
		case 41:
			return 1;

		//Interruptores
		case 42:
			return 0;
		case 43:
			return 1;

		default:
			return 255;
	}
}


/*Esta funcion toma los datos que ya se encuentran almacenados en vectores y los guarda en la cadena web_buffer
 * de la forma que lo estipula la definición del armado de tramas web
 */
static void web_buffer_constructor(modulos_response verificador){
	char * auxiliary_string = malloc(12*sizeof(char));
	uint8_t port_associated;
	uint8_t vector_position;
	strcpy(web_buffer, "|");
	for (uint8_t i = 0; i < keys_ports_counter; i++){
		sprintf(auxiliary_string, "%d", keys_with_ports[i]);
		strcat(web_buffer, auxiliary_string);
		strcat(web_buffer, ":");
		port_associated = pre_readx_nvs(keys_with_ports[i]);
		sprintf(auxiliary_string, "%d", port_associated);
		strcat(web_buffer, auxiliary_string);
		strcat(web_buffer, ":");
		vector_position = vector_positions(port_associated);
		if(port_associated <= 2){
		    //Estamos en temperaturas
			if(verificador.temperatura){
			    sprintf(auxiliary_string, "%d", temperatura[vector_position].valor1);
			    strcat(web_buffer, auxiliary_string);
			}
			else{
				strcat(web_buffer, "-");
			}
		}
		else if(port_associated == 4){
			if(verificador.digital_input){
				sprintf(auxiliary_string, "%d", digital_input[vector_position].valor1);
				strcat(web_buffer, auxiliary_string);
			}
			else{
				strcat(web_buffer, "-");
			}
		}
		else if(port_associated >= 5 && port_associated <= 8){
			//Estamos en tanques
			if(verificador.liquido){
				sprintf(auxiliary_string, "%d", tanque[vector_position].valor1);
				strcat(web_buffer, auxiliary_string);
				strcat(web_buffer, ":");
				sprintf(auxiliary_string, "%d", tanque[vector_position].valor2);
				strcat(web_buffer, auxiliary_string);
				strcat(web_buffer, ":");
				sprintf(auxiliary_string, "%d", tanque[vector_position].valor3);
				strcat(web_buffer, auxiliary_string);
				strcat(web_buffer, ":");
				sprintf(auxiliary_string, "%d", tanque[vector_position].valor4);
				strcat(web_buffer, auxiliary_string);
				strcat(web_buffer, ":");
				sprintf(auxiliary_string, "%d", tanque[vector_position].valor5);
				strcat(web_buffer, auxiliary_string);
			}
			else{
				strcat(web_buffer, "-");
			}
		}
		else if(port_associated == 9){
			//Estamos en inclinacion
			if(verificador.inclinacion){
				sprintf(auxiliary_string, "%d", inclinacion[vector_position].valor1);
				strcat(web_buffer, auxiliary_string);
				strcat(web_buffer, ":");
				sprintf(auxiliary_string, "%d", inclinacion[vector_position].valor2);
				strcat(web_buffer, auxiliary_string);
				strcat(web_buffer, ":");
				sprintf(auxiliary_string, "%d", inclinacion[vector_position].valor3);
				strcat(web_buffer, auxiliary_string);
			}
			else{
				strcat(web_buffer, "-");
			}
		}
		else if(port_associated >= 10 && port_associated <= 17){
			//Estamos en analogicos
			if(verificador.analogico){
				sprintf(auxiliary_string, "%d", analogico[vector_position].valor1);
				strcat(web_buffer, auxiliary_string);
				strcat(web_buffer, ":");
				sprintf(auxiliary_string, "%d", analogico[vector_position].valor2);
				strcat(web_buffer, auxiliary_string);
			}
			else{
				strcat(web_buffer, "-");
			}
		}
		else if(port_associated == 18){
			//Estamos en GPS
			if(verificador.gps){
				sprintf(auxiliary_string, "%d", sensorGPS[vector_position].valor1);
				strcat(web_buffer, auxiliary_string);
				strcat(web_buffer, ":");
				strcat(web_buffer, sensorGPS[vector_position].valor2);
				strcat(web_buffer, ":");
				sprintf(auxiliary_string, "%f", sensorGPS[vector_position].valor3);
				strcat(web_buffer, auxiliary_string);
				strcat(web_buffer, ":");
				sprintf(auxiliary_string, "%f", sensorGPS[vector_position].valor4);
				strcat(web_buffer, auxiliary_string);
				strcat(web_buffer, ":");
				sprintf(auxiliary_string, "%d", sensorGPS[vector_position].valor5);
				strcat(web_buffer, auxiliary_string);
			}
			else{
				strcat(web_buffer, "-");
			}
		}
		else if(port_associated >= 40 && port_associated <= 41){
			//Estamos en ventiladores
			if(verificador.ventilador){
				sprintf(auxiliary_string, "%d", ventilador[vector_position].valor1);
				strcat(web_buffer, auxiliary_string);
			}
			else{
				strcat(web_buffer, "-");
			}

		}
		else if(port_associated >= 42 && port_associated <= 43){
			//Estamos en interruptores
			if(verificador.digital_output){
				sprintf(auxiliary_string, "%d", interruptor[vector_position].valor1);
				strcat(web_buffer, auxiliary_string);
			}
			else{
				strcat(web_buffer, "-");
			}
		}
		strcat(web_buffer, "|");
	}
	//strcat(web_buffer, "%");
	free(auxiliary_string);
}

/* Esta funcion transmite por UART una solicitud de información de 7 bytes a la STM32 como lo estipula el
 * prtocolo de comunicaciones
 */
static void RequestForSTMData(const char* logName, uint8_t tipo_solicitud, uint8_t Port, uint8_t SlaveNumber, uint8_t New_value, uint8_t New_value2, uint8_t New_value3, uint8_t modulos_app){
	const uint8_t request_to_stm[7] = {tipo_solicitud, Port, SlaveNumber, New_value, New_value2, New_value3, modulos_app};
	//const uint8_t request_to_stm[7] = {0, 0, 0, 0, 255, 0, 1};
    const int txBytes = uart_write_bytes(UART_NUM_1, request_to_stm, 7);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
}

/*Tarea de transmisión por UART, esta tarea esencialmente pide información de los todos los modulos de la STM32
 * y luego arma el web buffer con la información que está almacenada en los vectores, el event group
 * es para no saturar el canal
 */

/*Esta funcion es la que distribuye la informacion a los respectivos vectores
 *
 */
static void allocate_data(uint8_t port_min, uint8_t port_max, uint8_t module, char** sensor_data){
	static const char *ALLOCATE_TASK_TAG = "ALLOCATE_DATA";
    esp_log_level_set(ALLOCATE_TASK_TAG, ESP_LOG_INFO);
	uint8_t port = port_min;
	uint8_t i=0;
	uint8_t q = 0;
	uint8_t j;
	char ** splitted_sensor_data;
    switch(module){
		case TEMPERATURE_SENSORS:
			while(port <= port_max){
				temperatura[i].valor1 = atoi(sensor_data[i]);
			//	ESP_LOGI(ALLOCATE_TASK_TAG, "This is temperature posicion %d VALOR1: %d", i, temperatura[i].valor1);
				i++;
				port++;
			}
			break;

		case DIGITAL_INPUT_SENSORS:
			while(port <= port_max){
				digital_input[i].valor1 = atoi(sensor_data[i]);
			//	ESP_LOGI(ALLOCATE_TASK_TAG, "This is temperature posicion %d VALOR1: %d", i, digital_input[i].valor1);
				i++;
				port++;
			}
			break;

		case LIQUIDO_SENSORS:
			while(port <= port_max){
				splitted_sensor_data = split_sensors(sensor_data[q], &j, ":");
				tanque[i].valor1 = atoi(splitted_sensor_data[0]);
				tanque[i].valor2 = atoi(splitted_sensor_data[1]);
				tanque[i].valor3 = atoi(splitted_sensor_data[2]);
				tanque[i].valor4 = atoi(splitted_sensor_data[3]);
				tanque[i].valor5 = atoi(splitted_sensor_data[4]);
			/*	ESP_LOGI(ALLOCATE_TASK_TAG, "This is tanque posicion %d, VALOR1 %d", i, tanque[i].valor1);
				ESP_LOGI(ALLOCATE_TASK_TAG, "This is tanque posicion %d, VALOR2 %d", i, tanque[i].valor2);
				ESP_LOGI(ALLOCATE_TASK_TAG, "This is tanque posicion %d, VALOR3 %d", i, tanque[i].valor3);
				ESP_LOGI(ALLOCATE_TASK_TAG, "This is tanque posicion %d, VALOR4 %d", i, tanque[i].valor4);
				ESP_LOGI(ALLOCATE_TASK_TAG, "This is tanque posicion %d, VALOR5 %d", i, tanque[i].valor5);*/
				i++;
				port++;
				q++;
			}
			break;

		case ANALOGICO_SENSORS:
			while(port <= port_max){
				splitted_sensor_data = split_sensors(sensor_data[q], &j, ":");
				analogico[i].valor1 = atoi(splitted_sensor_data[0]);
				analogico[i].valor2 = atoi(splitted_sensor_data[1]);
//				ESP_LOGI(ALLOCATE_TASK_TAG, "This is analogico posicion %d VALOR1: %d", i, analogico[i].valor1);
//				ESP_LOGI(ALLOCATE_TASK_TAG, "This is analogico posicion %d VALOR2: %d", i, analogico[i].valor2);
				i++;
				port++;
				q++;
			}
			break;

		case GPS_SENSORS:
			while(port <= port_max){
				splitted_sensor_data = split_sensors(sensor_data[q], &j, ":");
				if(strcmp(splitted_sensor_data[0], "true") == 0){
					sensorGPS[i].valor1 = true;
				}
				sensorGPS[i].valor2 = splitted_sensor_data[1];
				sensorGPS[i].valor3 = atof(splitted_sensor_data[2]);
				sensorGPS[i].valor4 = atof(splitted_sensor_data[3]);
				sensorGPS[i].valor5 = atoi(splitted_sensor_data[4]);
				sensorGPS[i].valor6 = atoi(splitted_sensor_data[5]);
				sensorGPS[i].valor7 = atoi(splitted_sensor_data[6]);
				sensorGPS[i].valor8 = atoi(splitted_sensor_data[7]);
	/*			ESP_LOGI(ALLOCATE_TASK_TAG, "This is sensorGPS posicion %d VALOR1: %d", i, sensorGPS[i].valor1);
				ESP_LOGI(ALLOCATE_TASK_TAG, "This is sensorGPS posicion %d VALOR2: %s", i, sensorGPS[i].valor2);
				ESP_LOGI(ALLOCATE_TASK_TAG, "This is sensorGPS posicion %d VALOR3: %f", i, sensorGPS[i].valor3);
				ESP_LOGI(ALLOCATE_TASK_TAG, "This is sensorGPS posicion %d VALOR4: %f", i, sensorGPS[i].valor4);
				ESP_LOGI(ALLOCATE_TASK_TAG, "This is sensorGPS posicion %d VALOR5: %d", i, sensorGPS[i].valor5);
				ESP_LOGI(ALLOCATE_TASK_TAG, "This is sensorGPS posicion %d VALOR6: %d", i, sensorGPS[i].valor6);
				ESP_LOGI(ALLOCATE_TASK_TAG, "This is sensorGPS posicion %d VALOR7: %d", i, sensorGPS[i].valor7);
				ESP_LOGI(ALLOCATE_TASK_TAG, "This is sensorGPS posicion %d VALOR8: %d", i, sensorGPS[i].valor8);*/
				i++;
				port++;
				q++;
			}
			break;

		case DIGITAL_OUTPUT_SENSORS:
			while(port <= port_max){
				interruptor[i].valor1 = atoi(sensor_data[i]);
		//		ESP_LOGI(ALLOCATE_TASK_TAG, "This is interruptor posicion %d VALOR1: %d", i, interruptor[i].valor1);
				i++;
				port++;
			}
			break;

		case VENTILADOR_SENSORS:
			while(port <= port_max){
				ventilador[i].valor1 = atoi(sensor_data[i]);
			//	ESP_LOGI(ALLOCATE_TASK_TAG, "This is ventilador posicion %d VALOR1: %d", i, ventilador[i].valor1);
				i++;
				port++;
			}
			break;

		case INCLINACION_SENSORS:
			while(port <= port_max){
				splitted_sensor_data = split_sensors(sensor_data[q], &j, ":");
				/*		inclinacion[i].valor1 = atoi(splitted_sensor_data[0]);
				inclinacion[i].valor2 = atoi(splitted_sensor_data[1]);
				inclinacion[i].valor3 = atoi(splitted_sensor_data[2]);
				ESP_LOGI(ALLOCATE_TASK_TAG, "This is inclinacion posicion %d, VALOR1 %d", i, inclinacion[i].valor1);
				ESP_LOGI(ALLOCATE_TASK_TAG, "This is inclinacion posicion %d, VALOR2 %d", i, inclinacion[i].valor2);
				ESP_LOGI(ALLOCATE_TASK_TAG, "This is inclinacion posicion %d, VALOR3 %d", i, inclinacion[i].valor3);*/
				i++;
				port++;
				q++;
			}
			break;

		default:
				ESP_LOGW(ALLOCATE_TASK_TAG, "Logica de asignacion no implementada para este modulo");
    }
	sensorGPS[0].valor1 = true;
	sensorGPS[0].valor2 = "1677863090000";
	sensorGPS[0].valor3 = 41.2610583;
	sensorGPS[0].valor4 = 1.7750433;
	sensorGPS[0].valor5 = 3;
	sensorGPS[0].valor6 = 52;
	sensorGPS[0].valor7 = 5;
	sensorGPS[0].valor8 = 354;
}


/*Esta funcion separa la informacion que proviene de UART para obtener los datos, los puertos y el modulo
 * y después hacer la distribucion de esta informacion a los vectores haciendo uso de la funcion allocate data
 */

static esp_err_t parse_data(uint8_t* data) {
    static const char *PARSE_TASK_TAG = "PARSE_DATA";
    esp_log_level_set(PARSE_TASK_TAG, ESP_LOG_INFO);
    ESP_LOGI(PARSE_TASK_TAG, "Parse data begin");
    char UART_Payload[150];
    sprintf(UART_Payload, "%s", data);
    char *token;
    char *sensor_data = NULL;
    char **splitted_sensor_data = NULL;
    uint8_t i;
    uint8_t port_min = 0;
    uint8_t port_max = 0;
    uint8_t module = 0;

    if (strstr(UART_Payload, "p") == NULL){
    	ESP_LOGE(PARSE_TASK_TAG, "Invalid data format: could not find 'p' delimiter");
    	xEventGroupSetBits(synchronize_serial_coms_group, not_parsing_data);
        goto cleanup;  // jump to the cleanup section of the code
    }
    if (strstr(UART_Payload, "-") == NULL){
    	ESP_LOGE(PARSE_TASK_TAG, "Invalid data format: could not find '-' delimiter");
    	xEventGroupSetBits(synchronize_serial_coms_group, not_parsing_data);
        goto cleanup;  // jump to the cleanup section of the code
    }
    if (strstr(UART_Payload, "d") == NULL){
    	ESP_LOGE(PARSE_TASK_TAG, "Invalid data format: could not find 'g' delimiter");
    	xEventGroupSetBits(synchronize_serial_coms_group, not_parsing_data);
        goto cleanup;  // jump to the cleanup section of the code
    }
    if (strstr(UART_Payload, "m") == NULL){
    	ESP_LOGE(PARSE_TASK_TAG, "Invalid data format: could not find 'm' delimiter");
    	xEventGroupSetBits(synchronize_serial_coms_group, not_parsing_data);
        goto cleanup;  // jump to the cleanup section of the code
    }

    token = strtok(UART_Payload, "p-");
    port_min = (uint8_t) strtoul(token, NULL, 10);

    token = strtok(NULL, "d");
    port_max = (uint8_t) strtoul(token, NULL, 10);

    token = strtok(NULL, "m");
    sensor_data = token;

    token = strtok(NULL, "");
    module = (uint8_t) strtoul(token, NULL, 10);

    if(module == 0){
    	ESP_LOGE(PARSE_TASK_TAG, "Module is zero, response not valid");
     	xEventGroupSetBits(synchronize_serial_coms_group, not_parsing_data);
     	goto cleanup;
    }

    ESP_LOGI(PARSE_TASK_TAG, "This is sensor data: %s", sensor_data);

    uint8_t comma_count = 0;
    for(int j = 0; j <= strlen(sensor_data); j++){
        if (sensor_data[j] == ',') {
            comma_count++;
        }
    }

    if(comma_count != (port_max-port_min)){
    	ESP_LOGE(PARSE_TASK_TAG, "Invalid data format: not enough data");
     	xEventGroupSetBits(synchronize_serial_coms_group, not_parsing_data);
     	goto cleanup;
    }

    //If it reaches this point, then it means the protocol has at least some degree of validity
    splitted_sensor_data = split_sensors(sensor_data, &i, ",");
    allocate_data(port_min, port_max, module, splitted_sensor_data);
    free(splitted_sensor_data);
    xEventGroupSetBits(synchronize_serial_coms_group, not_parsing_data);
    return ESP_OK;

cleanup:
    return ESP_FAIL;
}


void UART_tx_task(void *arg){
	static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    //uart_data uart;
    uint8_t *data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    uint16_t time_to_request = atoi(readx_nvs("1_TRAMA-TIME"));
    web_buffer = malloc(400*sizeof(char));
    strcpy(web_buffer, "|");
    while (1) {
    	memset(&verificador, 0, sizeof(verificador));

    	xEventGroupWaitBits(synchronize_serial_coms_group, not_parsing_data, pdTRUE, pdTRUE, pdMS_TO_TICKS(3000));
        RequestForSTMData("REQUEST_FOR_TEMPERATURE_SENSORS", (uint8_t) READ_PORT, 0, 0, 0, 255, 0, (uint8_t) TEMPERATURE_SENSORS); //Está implementado
    	if(xQueueReceive(uart_queue, &data, 5000/portTICK_PERIOD_MS)){
    		ESP_LOGI(TX_TASK_TAG, "Recibi un item de la cola cuyo valor es %s", data);
    		verificador.temperatura = (parse_data(data) == ESP_OK) ? true : false;
    		verificador.gps = true;
    	}
    	else{
    		ESP_LOGI(TX_TASK_TAG, "No recibi item de la cola");
    		verificador.temperatura = false;
    	}

    	xEventGroupWaitBits(synchronize_serial_coms_group, not_parsing_data, pdTRUE, pdTRUE, pdMS_TO_TICKS(3000));
    	RequestForSTMData("REQUEST_FOR_DIGITAL_INPUT_SENSORS", (uint8_t) READ_PORT, 0, 0, 0, 255, 0, (uint8_t) DIGITAL_INPUT_SENSORS);
    	if(xQueueReceive(uart_queue, &data, 5000/portTICK_PERIOD_MS)){
    		ESP_LOGI(TX_TASK_TAG, "Recibi un item de la cola cuyo valor es %s", data);
    		verificador.digital_input = (parse_data(data) == ESP_OK) ? true : false;
    	}
    	else{
    		ESP_LOGI(TX_TASK_TAG, "No recibi item de la cola");
    		verificador.digital_input = false;
    	}


//    	xEventGroupWaitBits(synchronize_serial_coms_group, not_parsing_data, pdTRUE, pdTRUE, pdMS_TO_TICKS(3000));
//        RequestForSTMData("REQUEST_FOR_LIQUIDO_SENSORS", (uint8_t) READ_PORT, 0, 0, 0, 255, 0, (uint8_t) LIQUIDO_SENSORS);
//    	if(xQueueReceive(uart_queue, &data, 5000/portTICK_PERIOD_MS) == pdPASS){
//    		ESP_LOGI(TX_TASK_TAG, "Recibi un item de la cola cuyo valor es %s", data);
//    		verificador.liquido = (parse_data(data) == ESP_OK) ? true : false;
//    	}
//    	else{
//    		ESP_LOGI(TX_TASK_TAG, "No recibi item de la cola");
//    		verificador.liquido = false;
//    	}

 /*   	RequestForSTMData("REQUEST_FOR_INCLINACION_SENSORS", (uint8_t) READ_PORT, 0, 0, 0, 255, 0, (uint8_t) INCLINACION_SENSORS); //Está implementado, retorna los puertos 5,6,7,8
    	if(xQueueReceive(uart_queue, &data, 5000/portTICK_PERIOD_MS)){
    		ESP_LOGI(TX_TASK_TAG, "Recibi un item de la cola cuyo valor es %s", data);
    		//verificador.inclinacion = (parse_data(data) == ESP_OK) ? true : false;;
    	}
    	else{
    		ESP_LOGI(TX_TASK_TAG, "No recibi item de la cola");
    		verificador.inclinacion = false;
    	}*/

//      RequestForSTMData("REQUEST_FOR_GPS_SENSORS", (uint8_t) READ_PORT, 0, 0, 0, 255, 0, (uint8_t) GPS_SENSORS);
//      xEventGroupWaitBits(synchronize_serial_coms_group, not_parsing_data, true, true, portMAX_DELAY);

    	//xEventGroupWaitBits(synchronize_serial_coms_group, not_parsing_data, true, true, portMAX_DELAY); //El event group sirve para implementar el TimeOut
    	RequestForSTMData("REQUEST_FOR_ANALOGICO_SENSORS", (uint8_t) READ_PORT, 0, 0, 0, 255, 0, (uint8_t) ANALOGICO_SENSORS);
    	if(xQueueReceive(uart_queue, &data, 5000/portTICK_PERIOD_MS)){
    		ESP_LOGI(TX_TASK_TAG, "Recibi un item de la cola cuyo valor es %s", data);
    		verificador.analogico = (parse_data(data) == ESP_OK) ? true : false;;
    	}
    	else{
    		ESP_LOGI(TX_TASK_TAG, "No recibi item de la cola");
    		verificador.analogico = false;
    	}

//        xEventGroupWaitBits(synchronize_serial_coms_group, not_parsing_data, pdTRUE, pdTRUE, pdMS_TO_TICKS(3000));
//        RequestForSTMData("REQUEST_FOR_DIGITAL_OUTPUT_SENSORS", (uint8_t) READ_PORT, 0, 0, 0, 255, 0, (uint8_t) DIGITAL_OUTPUT_SENSORS);
//    	if(xQueueReceive(uart_queue, &data, 5000/portTICK_PERIOD_MS)){
//    		ESP_LOGI(TX_TASK_TAG, "Recibi un item de la cola cuyo valor es %s", data);
//    		verificador.digital_output = (parse_data(data) == ESP_OK) ? true : false;;
//    	}
//    	else{
//    		ESP_LOGI(TX_TASK_TAG, "No recibi item de la cola");
//    		verificador.digital_output = false;
//    	}
//
//    	xEventGroupWaitBits(synchronize_serial_coms_group, not_parsing_data, pdTRUE, pdTRUE, pdMS_TO_TICKS(3000));
//    	RequestForSTMData("REQUEST_FOR_VENTILADOR_SENSORS", (uint8_t) READ_PORT, 0, 0, 0, 255, 0, (uint8_t) VENTILADOR_SENSORS);
//    	if(xQueueReceive(uart_queue, &data, 5000/portTICK_PERIOD_MS)){
//    		ESP_LOGI(TX_TASK_TAG, "Recibi un item de la cola cuyo valor es %s", data);
//    		verificador.ventilador = (parse_data(data) == ESP_OK) ? true : false;
//    	}
//    	else{
//    		ESP_LOGI(TX_TASK_TAG, "No recibi item de la cola");
//    		verificador.ventilador = false;
//    	}
//
    	if (verificador.temperatura | verificador.liquido | verificador.inclinacion | verificador.gps | verificador.analogico | verificador.digital_output | verificador.ventilador) {
    	    // call web_buffer_constructor function here
    		web_buffer_constructor(verificador);
            ESP_LOGI(TX_TASK_TAG, "Este es el web buffer %s", web_buffer);
            endpoint_discriminator();
    	} else {
    	    ESP_LOGI(TX_TASK_TAG, "No UART response from any module. Can't build web buffer");
    	}

        vTaskDelay(time_to_request / portTICK_PERIOD_MS);
    }
   free(web_buffer);
}


/*Tarea que recibe de UART. Una vez recibe informacion, la envia a la funcionn parse data para procesarla
 *
 */
void UART_rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    //uart_data uart;
    //uart.data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    uint8_t *data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
        	data[rxBytes] = 0;
        	ESP_LOGI(RX_TASK_TAG, "This is data received from STM32: %s", (char *) data);
            //parse_data(data, rxBytes);
            xQueueSend(uart_queue, &data, (TickType_t)5);
        }
    }
    free(data);
}

//Matricular llaves se refiere a asignar cada llave a un puerto específico, esta información depende netamente del usuario
//Y de como configure la ivAdventureMini desde el endpoint
void matricular_llaves(void){
	static const char *MATRICULA_LLAVES_TASK_TAG = "MATRICULA_DE_LLAVES";
    esp_log_level_set(MATRICULA_LLAVES_TASK_TAG, ESP_LOG_INFO);
	//Formato de esta matricula:
	//writex_nvs(llave, Puerto_asignado_a_esta_llave);

	//Llaves de temperatura
	pre_writex_nvs(TEMPTRANS, 0);
	pre_writex_nvs(NEVERA, 1);

    //Llaves de ventiladores
	pre_writex_nvs(VENTILADOR_A, 40);
	pre_writex_nvs(VENTILADOR_C, 41);

	//Llave de entrada digital
	pre_writex_nvs(INPUT_DIGITAL_1, 4);

	//Llaves de tanques
	pre_writex_nvs(GREYWATER, 6);
	pre_writex_nvs(OILOTHER, 7);
	pre_writex_nvs(BALLAST, 8);

    //Llaves de inclinacion
	pre_writex_nvs(INCLINACION_DISP_ROLL_PITCH_YAW, 9);

	//Llaves de analógico
	pre_writex_nvs(PRESACEI, 10);
	pre_writex_nvs(PRESCOM, 11);
	pre_writex_nvs(ACELERADOR_1, 12);
	pre_writex_nvs(ACELERADOR_2, 13);
	pre_writex_nvs(ACELERADOR_3, 14);
	pre_writex_nvs(ACELERADOR_4, 15);
	pre_writex_nvs(BATERIA_A_CUR, 16);
	pre_writex_nvs(PRESION_DISP, 17);

	//Llaves de interruptores
	pre_writex_nvs(FGD_TRANSM_TRACCION, 42);
	//writex_nvs("40", 42);
	pre_writex_nvs(RGD_TRANSM_TRACCION, 43);
	//writex_nvs("41", 43);

	//Llaves de GPS
    pre_writex_nvs(GNSS_GGA, 18);

	ESP_LOGI(MATRICULA_LLAVES_TASK_TAG, "Llaves matriculadas!");
}

/*Esta funcion carga a la RAM de la ESP32 cuales llaves se encuentran habilitadas en el endpoint
 * esta información se encuentra cargada en la NVS, la idea de esta funcion es que se tenga que recorrer
 * las 132 posibles llaves que pueden estar habilitadas una sola vez (al inicio del programa) para de esta manera
 * no tener que recorrer las 132 llaves cada vez que se tenga que armar el web_buffer
 */
void load_keys_with_port_assigned_to_RAM(){
    static const char *LOADING_KEYS_TAG = "LOADING_ASSIGNED_KEYS";
    esp_log_level_set(LOADING_KEYS_TAG, ESP_LOG_INFO);
    keys_with_ports = malloc(1*sizeof(uint8_t));
    uint8_t aux;
    for(uint8_t i=1; i<AMOUNT_OF_ENUMS_IN_KEYVALUE; i++){
    	aux = pre_readx_nvs(i);
    	switch(aux){
    	case KEY_NOT_ALLOCATED:
    		//ESP_LOGW("REVISANDO_KEYS", "KEY NOT ALLOCATED!");
			break;
    	case ERROR_READING_NVS:
    		//ESP_LOGW("REVISANDO_KEYS", "ERROR READING KEY!");
    		break;
    	default:
    		ESP_LOGW(LOADING_KEYS_TAG, "key %d %s has port %d", i, llaves_tarjeta[i-1], aux);
    		keys_with_ports[keys_ports_counter] = i;
    		keys_ports_counter++;
    		keys_with_ports = realloc(keys_with_ports, (keys_ports_counter + 1) * sizeof(uint8_t ));
    		break;
    	}
    }

}

#endif
