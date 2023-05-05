/*
 * json_controller.h
 *
 *  Created on: Mar 15, 2023
 *      Author: david
 */

#ifndef MAIN_JSON_CONTROLLER_H_
#define MAIN_JSON_CONTROLLER_H_

static const char *panicdata="|121:1|";


char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


char *trama_endp( char *endpoints, char *nameid,char *datos)
		{
		    char *string = NULL;

		    cJSON *monitor = cJSON_CreateObject();
		    if (cJSON_AddStringToObject(monitor, "endpoint", endpoints) == NULL)
		   	    {
		   	        goto end;
		   	    }
		    if (cJSON_AddStringToObject(monitor, nameid, datos) == NULL)//pendiente extraer nombre del archivo
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

char *trama_sensors(void)
{

	//ESP_LOGW(TAG, "Este es el NVS %s", leer_nvs("tramanvs1"));
    char *string = NULL;
   // char *temp= leer_nvs("7_USER_ID");
    char *temp= "9LjcSDc+bgWwy4eKVJTVNlNxu/5ZtMom6/mOhx8nLg68pMxoZA3diJ6m04zeITzz";


    cJSON *monitor = cJSON_CreateObject();

    if (cJSON_AddStringToObject(monitor, "user", temp) == NULL)
    {
        goto end;
    }
    if (cJSON_AddStringToObject(monitor, "type", "IAM_T") == NULL)
        {
            goto end;
        }
    if (cJSON_AddStringToObject(monitor, "trama", get_webbuffer()) == NULL)

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

char *trama_panic(void)
{

	    char *string = NULL;
	    char *temp= "9LjcSDc+bgWwy4eKVJTVNlNxu/5ZtMom6/mOhx8nLg68pMxoZA3diJ6m04zeITzz";
	    char* s = concat(panicdata, get_gnss());

	    cJSON *monitor = cJSON_CreateObject();

	    if (cJSON_AddStringToObject(monitor, "user", temp) == NULL)
	    {
	        goto end;
	    }
	    if (cJSON_AddStringToObject(monitor, "type", "IAM_B") == NULL)
	        {
	            goto end;
	        }
	    if (cJSON_AddStringToObject(monitor, "trama", s) == NULL)
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
	    free(s);
	    return string;
}

char *trama_alarma(void)
{

	    char *string = NULL;
	    char *temp= "9LjcSDc+bgWwy4eKVJTVNlNxu/5ZtMom6/mOhx8nLg68pMxoZA3diJ6m04zeITzz";
	    cJSON *monitor = cJSON_CreateObject();

	    if (cJSON_AddStringToObject(monitor, "user", temp) == NULL)
	    {
	        goto end;
	    }
	    if (cJSON_AddStringToObject(monitor, "type", "IAM_A") == NULL)
	        {
	            goto end;
	        }
	    if (cJSON_AddStringToObject(monitor, "trama", "Esto es una alarma") == NULL)

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

char *trama_registro(void)
{

	    char *string = NULL;
	    char *temp= "9LjcSDc+bgWwy4eKVJTVNlNxu/5ZtMom6/mOhx8nLg68pMxoZA3diJ6m04zeITzz";
	    cJSON *monitor = cJSON_CreateObject();

	    if (cJSON_AddStringToObject(monitor, "idEmpresa", 95) == NULL)
	    {
	        goto end;
	    }
	    if (cJSON_AddStringToObject(monitor, "habilitado",true) == NULL)
	        {
	            goto end;
	        }
	    if (cJSON_AddStringToObject(monitor, "indAsignado", false) == NULL)

	            {
	                goto end;
	            }
	    if (cJSON_AddStringToObject(monitor, "serialInterno", temp) == NULL)

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

#endif /* MAIN_JSON_CONTROLLER_H_ */
