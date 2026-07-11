/******************************************************************************************************************************/
/* ABLS-AGENT-PHIDGET/phidget.c  Gestion des modules PHIDGET ABLS-AGENT-PHIDGET                                               */
/* Projet Abls-Habitat                   Gestion d'habitat                                                18.03.2021 22:02:42 */
/* Auteur: LEFEVRE Sebastien                                                                                                  */
/******************************************************************************************************************************/
/*
 * Phidget.c
 * This file is part of Abls-Habitat
 *
 * Copyright (C) 1988-2026 - Sébastien LEFÈVRE
 *
 * ABLS-AGENT-PHIDGET is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ABLS-AGENT-PHIDGET is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ABLS-AGENT-PHIDGET; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

 #include <stdio.h>
 #include <fcntl.h>

 #include "phidget.h"

/******************************************************************************************************************************/
/* Charger_un_Hub: Charge un Hub dans la librairie                                                                            */
/* Entrée: La structure Json representant le hub                                                                              */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void Phidget_print_error ( struct ABLS_PHIDGET_ELEMENT *canal )
  { PhidgetReturnCode errorCode;
    gchar *capteur = Json_get_string(canal->element, "capteur");
    gchar *classe  = Json_get_string(canal->element, "classe");
    size_t errorDetailLen = 256;
    const gchar* errorString;
    gchar errorDetail[errorDetailLen];
    Phidget_getLastError(&errorCode, &errorString, errorDetail, &errorDetailLen);
    Info( __func__, canal->agent->agent_classe, canal->agent->agent_tech_id, LOG_ERR, "Phidget Error %d for '%s' (%s) : %s - %s",
          errorCode, capteur, classe, errorString, errorDetail );
  }
/******************************************************************************************************************************/
/* Phidget_onAIError: Appelé quand une erreur est constatée sur le module Phidget                                             */
/* Entrée: le channel, le contexte, et la description de l'erreur                                                             */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void CCONV Phidget_onError (PhidgetHandle ph, void *ctx, Phidget_ErrorEventCode code, const char *description)
  { struct ABLS_PHIDGET_ELEMENT *canal = ctx;

    gchar *agent_acronyme = Json_get_string(canal->element, "agent_acronyme");
    gchar *classe         = Json_get_string(canal->element, "classe");

    if ( !strcmp ( classe, "AI" ) )
    { Info( __func__, canal->agent->agent_classe, canal->agent->agent_tech_id, LOG_ERR, "Error for '%s:%s' : '%s' (code %X). Set in_range = FALSE.",
            canal->agent->agent_tech_id, agent_acronyme, description, code );
       Mqtt_Send_AI ( canal->agent, canal->element, 0.0, FALSE );
     }
    else
     { Info( __func__, canal->agent->agent_classe, canal->agent->agent_tech_id, LOG_ERR, "Error for '%s:%s' : '%s' (code %X).",
             canal->agent->agent_tech_id, agent_acronyme, description, code );
     }
  }
/******************************************************************************************************************************/
/* Phidget_onPHSensorChange: Appelé quand un module I/O PHSensor a changé de valeur                                           */
/* Entrée: le channel, le contexte, et la nouvelle valeur                                                                     */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void CCONV Phidget_onPHSensorChange ( PhidgetPHSensorHandle handle, void *ctx, double valeur )
  { struct ABLS_PHIDGET_ELEMENT *canal = ctx;
    gchar *agent_acronyme = Json_get_string(canal->element, "agent_acronyme");
    Info( __func__, canal->agent->agent_classe, canal->agent->agent_tech_id, LOG_DEBUG, "'%s:%s' = %f", canal->agent->agent_tech_id, agent_acronyme, valeur );
    Mqtt_Send_AI ( canal->agent, canal->element, valeur, TRUE );
  }
/******************************************************************************************************************************/
/* Phidget_onTemperatureSensorChange: Appelé quand un module I/O Temperaute a changé de valeur                                */
/* Entrée: le channel, le contexte, et la nouvelle valeur                                                                     */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void CCONV Phidget_onTemperatureSensorChange ( PhidgetTemperatureSensorHandle handle, void *ctx, double valeur )
  { struct ABLS_PHIDGET_ELEMENT *canal = ctx;
    gchar *agent_acronyme = Json_get_string(canal->element, "agent_acronyme");
    Info( __func__, canal->agent->agent_classe, canal->agent->agent_tech_id, LOG_DEBUG, "'%s:%s' = %f", canal->agent->agent_tech_id, agent_acronyme, valeur );
    Mqtt_Send_AI ( canal->agent, canal->element, valeur, TRUE );
  }
/******************************************************************************************************************************/
/* Phidget_onVoltableInputChange: Appelé quand un module I/O VoltageInput a changé de valeur                                  */
/* Entrée: le channel, le contexte, et la nouvelle valeur                                                                     */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void CCONV Phidget_onVoltageInputChange ( PhidgetVoltageInputHandle handle, void *ctx, double valeur )
  { struct ABLS_PHIDGET_ELEMENT *canal = ctx;
    gchar *agent_acronyme = Json_get_string(canal->element, "agent_acronyme");
    Info( __func__, canal->agent->agent_classe, canal->agent->agent_tech_id, LOG_DEBUG, "'%s:%s' = %f", canal->agent->agent_tech_id, agent_acronyme, valeur );
    Mqtt_Send_AI ( canal->agent, canal->element, valeur, TRUE );
  }
/******************************************************************************************************************************/
/* Phidget_onVoltageSensorChange: Appelé quand un module I/O VoltageSensor a changé de valeur                                   */
/* Entrée: le channel, le contexte, et la nouvelle valeur                                                                     */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void CCONV Phidget_onVoltageSensorChange ( PhidgetVoltageInputHandle handle, void *ctx, double valeur,
                                                   Phidget_UnitInfo *sensorUnit )
  { struct ABLS_PHIDGET_ELEMENT *canal = ctx;
    gchar *agent_acronyme = Json_get_string(canal->element, "agent_acronyme");
    Info( __func__, canal->agent->agent_classe, canal->agent->agent_tech_id, LOG_DEBUG, "'%s:%s' = %f", canal->agent->agent_tech_id, agent_acronyme, valeur );
    Mqtt_Send_AI ( canal->agent, canal->element, valeur, TRUE );
  }
/******************************************************************************************************************************/
/* Phidget_onVoltageRatoiInputChange: Appelé quand un module I/O RatioInput a changé de valeur                                */
/* Entrée: le channel, le contexte, et la nouvelle valeur                                                                     */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void CCONV Phidget_onVoltageRatioSensorChange ( PhidgetVoltageRatioInputHandle ch, void *ctx, double valeur,
                                                        Phidget_UnitInfo *sensorUnit)
  { struct ABLS_PHIDGET_ELEMENT *canal = ctx;
    gchar *agent_acronyme = Json_get_string(canal->element, "agent_acronyme");
    Info( __func__, canal->agent->agent_classe, canal->agent->agent_tech_id, LOG_DEBUG, "'%s:%s' = %f", canal->agent->agent_tech_id, agent_acronyme, valeur );
    Mqtt_Send_AI ( canal->agent, canal->element, valeur, TRUE );
  }
/******************************************************************************************************************************/
/* Phidget_onVoltableInputChange: Appelé quand un module I/O VoltageInput a changé de valeur                                  */
/* Entrée: le channel, le contexte, et la nouvelle valeur                                                                     */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void CCONV Phidget_onDigitalInputChange ( PhidgetDigitalInputHandle handle, void *ctx, int valeur )
  { struct ABLS_PHIDGET_ELEMENT *canal = ctx;
    gchar *agent_acronyme = Json_get_string(canal->element, "agent_acronyme");
    Info( __func__, canal->agent->agent_classe, canal->agent->agent_tech_id, LOG_DEBUG, "'%s:%s' = %d", canal->agent->agent_tech_id, agent_acronyme, valeur );
    Mqtt_Send_DI ( canal->agent, canal->element, (valeur ? TRUE : FALSE) );
  }
/******************************************************************************************************************************/
/* Phidget_onVoltableInputChange: Appelé quand un module I/O VoltageInput a changé de valeur                                  */
/* Entrée: le channel, le contexte, et la nouvelle valeur                                                                     */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void CCONV Phidget_onSensorChange ( PhidgetVoltageInputHandle handle, void *ctx, double valeur, Phidget_UnitInfo *sensorUnit )
  { struct ABLS_PHIDGET_ELEMENT *canal = ctx;
    gchar *agent_acronyme = Json_get_string(canal->element, "agent_acronyme");
    Info( __func__, canal->agent->agent_classe, canal->agent->agent_tech_id, LOG_DEBUG, "'%s:%s' = %f %s", canal->agent->agent_tech_id, agent_acronyme, valeur, sensorUnit->symbol );
    Mqtt_Send_AI ( canal->agent, canal->element, valeur, TRUE );
  }
/******************************************************************************************************************************/
/* Phidget_AnalogAttach: Appelé quand un canal analogique est en cours d'attachement                                          */
/* Entrée: le canal                                                                                                           */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void Phidget_AnalogAttach ( struct ABLS_PHIDGET_ELEMENT *canal )
  { gint intervalle = Json_get_int(canal->element, "intervalle");
    gchar *capteur  = Json_get_string(canal->element, "capteur");
    if (intervalle)
     { if (Phidget_setDataInterval( canal->handle, intervalle ) != EPHIDGET_OK) Phidget_print_error(canal); }

    if (!strcasecmp(capteur, "ADP1000-PH"))
     { /**/
     }
    else if (!strcasecmp(capteur, "ADP1000-ORP"))
     { /**/
     }
    else if (!strcasecmp(capteur, "1130-PH"))
     { if ( PhidgetVoltageInput_setSensorType ( (PhidgetVoltageInputHandle)canal->handle, SENSOR_TYPE_1130_PH ) != EPHIDGET_OK )
        { Phidget_print_error(canal); }
     }
    else if (!strcasecmp(capteur, "1130-ORP"))
     { if ( PhidgetVoltageInput_setSensorType ( (PhidgetVoltageInputHandle)canal->handle, SENSOR_TYPE_1130_ORP ) != EPHIDGET_OK )
        { Phidget_print_error(canal); }
     }
    else if (!strcasecmp(capteur, "TMP1200_0-PT100-3850"))
     { if ( PhidgetTemperatureSensor_setRTDType( (PhidgetTemperatureSensorHandle)canal->handle, RTD_TYPE_PT100_3850 ) != EPHIDGET_OK )
        { Phidget_print_error(canal); }
       if ( PhidgetTemperatureSensor_setRTDWireSetup( (PhidgetTemperatureSensorHandle)canal->handle, RTD_WIRE_SETUP_2WIRE ) != EPHIDGET_OK )
        { Phidget_print_error(canal); }
     }
    else if (!strcasecmp(capteur, "TMP1200_0-PT100-3920"))
     { if ( PhidgetTemperatureSensor_setRTDType( (PhidgetTemperatureSensorHandle)canal->handle, RTD_TYPE_PT100_3920 ) != EPHIDGET_OK )
        { Phidget_print_error(canal); }
       if ( PhidgetTemperatureSensor_setRTDWireSetup( (PhidgetTemperatureSensorHandle)canal->handle, RTD_WIRE_SETUP_2WIRE ) != EPHIDGET_OK )
        { Phidget_print_error(canal); }
     }
    else if (!strcasecmp(capteur, "AC-CURRENT-10A"))
     { if ( PhidgetVoltageInput_setSensorType ( (PhidgetVoltageInputHandle)canal->handle, SENSOR_TYPE_3500 ) != EPHIDGET_OK )
        { Phidget_print_error(canal); }
     }
    else if (!strcasecmp(capteur, "AC-CURRENT-25A"))
     { if ( PhidgetVoltageInput_setSensorType ( (PhidgetVoltageInputHandle)canal->handle, SENSOR_TYPE_3501 ) != EPHIDGET_OK )
        { Phidget_print_error(canal); }
     }
    else if (!strcasecmp(capteur, "AC-CURRENT-50A"))
     { if ( PhidgetVoltageInput_setSensorType ( (PhidgetVoltageInputHandle)canal->handle, SENSOR_TYPE_3502 ) != EPHIDGET_OK )
        { Phidget_print_error(canal); }
     }
    else if (!strcasecmp(capteur, "AC-CURRENT-100A"))
     { if ( PhidgetVoltageInput_setSensorType ( (PhidgetVoltageInputHandle)canal->handle, SENSOR_TYPE_3503 ) != EPHIDGET_OK )
        { Phidget_print_error(canal); }
     }
    else if (!strcasecmp(capteur, "TEMP_1124_0"))
     { if ( PhidgetVoltageRatioInput_setSensorType ( (PhidgetVoltageRatioInputHandle)canal->handle, SENSOR_TYPE_1124 ) != EPHIDGET_OK )
        { Phidget_print_error(canal); }
     }
  }
/******************************************************************************************************************************/
/* Phidget_onAttachHandler: Appelé quand un canal estmodule I/O VoltageRatio a changé de valeur                               */
/* Entrée: le channel, le contexte                                                                                            */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void CCONV Phidget_onAttachHandler ( PhidgetHandle handle, void *ctx )
  { struct ABLS_PHIDGET_ELEMENT *canal = ctx;
    int serial_number, port, num_canal;
    uint nbr_canaux;

    gchar *agent_acronyme = Json_get_string(canal->element, "agent_acronyme");
    gchar *classe         = Json_get_string(canal->element, "classe");
    gchar *capteur        = Json_get_string(canal->element, "capteur");
    Phidget_getDeviceSerialNumber(handle, &serial_number);
    Phidget_getDeviceChannelCount(handle, PHIDCHCLASS_NOTHING, &nbr_canaux );
    Phidget_getHubPort( handle, &port );
    Phidget_getChannel( handle, &num_canal );

    if ( !strcmp ( classe, "AI" ) ) { Phidget_AnalogAttach ( canal ); }

    Info( __func__, classe, agent_acronyme, LOG_NOTICE,
          "'%s:%s' Phidget S/N '%d' Port '%d' capteur '%s' (canal '%d') attached. %d channels available.",
          agent_acronyme, agent_acronyme, serial_number, port, capteur, num_canal, nbr_canaux );

    canal->attached = TRUE;
  }
/******************************************************************************************************************************/
/* Phidget_onDetachHandler: Appelé quand un canal est détaché                                                                 */
/* Entrée: le channel, le contexte                                                                                            */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void CCONV Phidget_onDetachHandler ( PhidgetHandle handle, void *ctx )
  { struct ABLS_PHIDGET_ELEMENT *canal = ctx;
    int serial_number, port, num_canal;
    uint nbr_canaux;

    gchar *agent_acronyme = Json_get_string(canal->element, "agent_acronyme");
    gchar *classe         = Json_get_string(canal->element, "classe");
    Phidget_getDeviceSerialNumber(handle, &serial_number);
    Phidget_getDeviceChannelCount(handle, PHIDCHCLASS_NOTHING, &nbr_canaux );
    Phidget_getHubPort( handle, &port );
    Phidget_getChannel( handle, &num_canal );

    Info( __func__, "phidget", agent_acronyme, LOG_NOTICE,
          "'%s:%s' Phidget S/N '%d' Port '%d' classe '%s' (canal '%d') detached . %d channels available.",
          agent_acronyme, agent_acronyme, serial_number, port, classe, num_canal, nbr_canaux );
    canal->attached = FALSE;
  }
/******************************************************************************************************************************/
/* Phidget_set_config: Zpplique la configuration a l'element en pj                                                            */
/* Entrée: La structure Json representant l'i/o                                                                               */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void Phidget_set_config ( struct ABLS_PHIDGET_ELEMENT *canal, gint serial, gint port, gboolean is_hub_port )
  { if (Phidget_setDeviceSerialNumber( (PhidgetHandle)canal->handle, serial ) != EPHIDGET_OK)
     { Phidget_print_error(canal);
       return;
     }

    if (Phidget_setIsHubPortDevice( (PhidgetHandle)canal->handle, is_hub_port ) != EPHIDGET_OK)
     { Phidget_print_error(canal);
       return;
     }

    if (Phidget_setHubPort( (PhidgetHandle)canal->handle, port ) != EPHIDGET_OK)
     { Phidget_print_error(canal);
       return;
     }

    if (Phidget_setIsRemote( (PhidgetHandle)canal->handle, 1 ) != EPHIDGET_OK)
     { Phidget_print_error(canal);
       return;
     }
  }
/******************************************************************************************************************************/
/* Phidget_Charger_un_IO: Charge les IO                                                                                       */
/* Entrée: La structure Json representant l'i/o                                                                               */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void Phidget_Charger_un_IO (JsonArray *array, guint index_, JsonNode *element, gpointer user_data )
  { struct ABLS_AGENT *agent = user_data;
    struct ABLS_PHIDGET_VARS *vars = agent->vars;
    gint serial    = Json_get_int   ( agent->api_config, "serial" );
    gchar *capteur = Json_get_string( element, "capteur" );
    gint port      = Json_get_int   ( element, "port" );

    Info( __func__, agent->agent_classe, agent->agent_tech_id, LOG_INFO, "Loading S/N %d, port '%d', capteur '%s'", serial, port, capteur );

    struct ABLS_PHIDGET_ELEMENT *canal = g_try_malloc0 ( sizeof(struct ABLS_PHIDGET_ELEMENT) );
    if (!canal)
     { Info( __func__, agent->agent_classe, agent->agent_tech_id, LOG_ALERT, "Memory Error on S/N %d, port '%d' capteur '%s'", serial, port, capteur );
       return;
     }

    canal->agent   = agent;                                                                      /* Sauvegarde du module père */
    canal->element = element;

    if (!strcasecmp(capteur, "ADP1000-PH"))
     { if ( PhidgetPHSensor_create( (PhidgetPHSensorHandle *)&canal->handle ) != EPHIDGET_OK ) goto error;
       if ( Phidget_setOnErrorHandler( canal->handle, Phidget_onError, canal ) ) goto error;
       if ( PhidgetPHSensor_setOnPHChangeHandler( (PhidgetPHSensorHandle)canal->handle, Phidget_onPHSensorChange, canal ) ) goto error;
       Phidget_set_config ( canal, serial, port, TRUE );
     }
    else if (!strcasecmp(capteur, "ADP1000-ORP"))
     { if ( PhidgetVoltageInput_create( (PhidgetVoltageInputHandle *)&canal->handle ) != EPHIDGET_OK ) goto error;
       if ( Phidget_setOnErrorHandler( canal->handle, Phidget_onError, canal ) ) goto error;
       if ( PhidgetVoltageInput_setOnVoltageChangeHandler( (PhidgetVoltageInputHandle)canal->handle,
                                                            Phidget_onVoltageInputChange, canal ) != EPHIDGET_OK ) goto error;
       Phidget_set_config ( canal, serial, port, TRUE );
     }
    else if (!strcasecmp(capteur, "1130-PH"))
     { if ( PhidgetVoltageInput_create( (PhidgetVoltageInputHandle *)&canal->handle ) != EPHIDGET_OK ) goto error;
       if ( Phidget_setOnErrorHandler( canal->handle, Phidget_onError, canal ) ) goto error;
       if ( PhidgetVoltageInput_setOnVoltageChangeHandler( (PhidgetVoltageInputHandle)canal->handle,
                                                            Phidget_onVoltageInputChange, canal ) != EPHIDGET_OK ) goto error;
       Phidget_set_config ( canal, serial, port, TRUE );
     }
    else if (!strcasecmp(capteur, "1130-ORP"))
     { if ( PhidgetVoltageInput_create( (PhidgetVoltageInputHandle *)&canal->handle ) != EPHIDGET_OK ) goto error;
       if ( Phidget_setOnErrorHandler( canal->handle, Phidget_onError, canal ) ) goto error;
       if ( PhidgetVoltageInput_setOnSensorChangeHandler( (PhidgetVoltageInputHandle)canal->handle,
                                                           Phidget_onSensorChange, canal ) != EPHIDGET_OK ) goto error;
       Phidget_set_config ( canal, serial, port, TRUE );
     }
    else if (!strcasecmp(capteur, "TMP1200_0-PT100-3850"))
     { if ( PhidgetTemperatureSensor_create( (PhidgetTemperatureSensorHandle *)&canal->handle ) != EPHIDGET_OK ) goto error;
       if ( Phidget_setOnErrorHandler( canal->handle, Phidget_onError, canal ) ) goto error;
       if ( PhidgetTemperatureSensor_setOnTemperatureChangeHandler( (PhidgetTemperatureSensorHandle)canal->handle,
                                                                     Phidget_onTemperatureSensorChange, canal ) != EPHIDGET_OK ) goto error;
       Phidget_set_config ( canal, serial, port, FALSE );
     }
    else if (!strcasecmp(capteur, "TMP1200_0-PT100-3920"))
     { if ( PhidgetTemperatureSensor_create( (PhidgetTemperatureSensorHandle *)&canal->handle ) != EPHIDGET_OK ) goto error;
       if ( Phidget_setOnErrorHandler( canal->handle, Phidget_onError, canal ) ) goto error;
       if ( PhidgetTemperatureSensor_setOnTemperatureChangeHandler( (PhidgetTemperatureSensorHandle)canal->handle,
                                                                     Phidget_onTemperatureSensorChange, canal ) != EPHIDGET_OK ) goto error;
       Phidget_set_config ( canal, serial, port, FALSE );
     }
    else if (!strcasecmp(capteur, "AC-CURRENT-10A"))
     { if ( PhidgetVoltageInput_create( (PhidgetVoltageInputHandle *)&canal->handle ) != EPHIDGET_OK ) goto error;
       if ( Phidget_setOnErrorHandler( canal->handle, Phidget_onError, canal ) ) goto error;
       if ( PhidgetVoltageInput_setOnSensorChangeHandler( (PhidgetVoltageInputHandle)canal->handle,
                                                           Phidget_onVoltageSensorChange, canal ) != EPHIDGET_OK ) goto error;
       Phidget_set_config ( canal, serial, port, TRUE );
     }
    else if (!strcasecmp(capteur, "AC-CURRENT-25A"))
     { if ( PhidgetVoltageInput_create( (PhidgetVoltageInputHandle *)&canal->handle ) != EPHIDGET_OK ) goto error;
       if ( Phidget_setOnErrorHandler( canal->handle, Phidget_onError, canal ) ) goto error;
       if ( PhidgetVoltageInput_setOnSensorChangeHandler( (PhidgetVoltageInputHandle)canal->handle,
                                                           Phidget_onVoltageSensorChange, canal ) != EPHIDGET_OK ) goto error;
       Phidget_set_config ( canal, serial, port, TRUE );
     }
    else if (!strcasecmp(capteur, "AC-CURRENT-50A"))
     { if ( PhidgetVoltageInput_create( (PhidgetVoltageInputHandle *)&canal->handle ) != EPHIDGET_OK ) goto error;
       if ( Phidget_setOnErrorHandler( canal->handle, Phidget_onError, canal ) ) goto error;
       if ( PhidgetVoltageInput_setOnSensorChangeHandler( (PhidgetVoltageInputHandle)canal->handle,
                                                           Phidget_onVoltageSensorChange, canal ) != EPHIDGET_OK ) goto error;
       if ( PhidgetVoltageInput_setVoltageRange( (PhidgetVoltageInputHandle)canal->handle, VOLTAGE_RANGE_5V) != EPHIDGET_OK ) goto error;
       Phidget_set_config ( canal, serial, port, TRUE );
     }
    else if (!strcasecmp(capteur, "AC-CURRENT-100A"))
     { if ( PhidgetVoltageInput_create( (PhidgetVoltageInputHandle *)&canal->handle ) != EPHIDGET_OK ) goto error;
       if ( Phidget_setOnErrorHandler( canal->handle, Phidget_onError, canal ) ) goto error;
       if ( PhidgetVoltageInput_setOnSensorChangeHandler( (PhidgetVoltageInputHandle)canal->handle,
                                                           Phidget_onVoltageSensorChange, canal ) != EPHIDGET_OK ) goto error;
       Phidget_set_config ( canal, serial, port, TRUE );
     }
    else if (!strcasecmp(capteur, "TEMP_1124_0"))
     { if ( PhidgetVoltageRatioInput_create( (PhidgetVoltageRatioInputHandle *)&canal->handle ) != EPHIDGET_OK ) goto error;
       if ( Phidget_setOnErrorHandler( canal->handle, Phidget_onError, canal ) ) goto error;
       if ( PhidgetVoltageRatioInput_setOnSensorChangeHandler( (PhidgetVoltageRatioInputHandle)canal->handle,
                                                                Phidget_onVoltageRatioSensorChange, canal ) != EPHIDGET_OK ) goto error;
       Phidget_set_config ( canal, serial, port, TRUE );
     }
    else if (!strcasecmp(capteur, "DIGITAL-INPUT"))
     { if ( PhidgetDigitalInput_create( (PhidgetDigitalInputHandle *)&canal->handle ) != EPHIDGET_OK ) goto error;
       if ( Phidget_setOnErrorHandler( canal->handle, Phidget_onError, canal ) ) goto error;
       if ( PhidgetDigitalInput_setOnStateChangeHandler( (PhidgetDigitalInputHandle)canal->handle, Phidget_onDigitalInputChange, canal ) ) goto error;
       Phidget_set_config ( canal, serial, port, TRUE );
     }
    else if (!strcasecmp(capteur, "REL2001_0"))
     { if ( PhidgetDigitalOutput_create( (PhidgetDigitalOutputHandle *)&canal->handle ) != EPHIDGET_OK ) goto error;
       if ( Phidget_setOnErrorHandler( canal->handle, Phidget_onError, canal ) ) goto error;
       Phidget_set_config ( canal, serial, port, TRUE );
     }
    else
    { Info( __func__, agent->agent_classe, agent->agent_tech_id, LOG_INFO,
                 "capteur phidget inconnue on S/N %d, port '%d' capteur '%s'", serial, port, capteur );
       goto error;
     }

    Phidget_setOnAttachHandler((PhidgetHandle)canal->handle, Phidget_onAttachHandler, canal);
    Phidget_setOnDetachHandler((PhidgetHandle)canal->handle, Phidget_onDetachHandler, canal);

    if (Phidget_openWaitForAttachment ((PhidgetHandle)canal->handle, 5000) != EPHIDGET_OK) goto error;
    vars->Liste_sensors = g_slist_prepend ( vars->Liste_sensors, canal );
    return;
error:
    Phidget_print_error(canal);
    g_free(canal);
  }
/******************************************************************************************************************************/
/* Phidget_Decharger_un_IO: Décharge une IO dans la librairie                                                                 */
/* Entrée: Le canal representant l'i/o                                                                                        */
/* Sortie: néant                                                                                                              */
/******************************************************************************************************************************/
 static void Phidget_Decharger_un_IO ( struct ABLS_PHIDGET_ELEMENT *canal )
  { Phidget_close ( (PhidgetHandle)canal->handle );
    gchar *capteur = Json_get_string( canal->element, "capteur" );

         if (!strcasecmp(capteur, "ADP1000-PH"))           PhidgetPHSensor_delete         ( (PhidgetPHSensorHandle *)&canal->handle );
    else if (!strcasecmp(capteur, "ADP1000-ORP"))          PhidgetVoltageInput_delete     ( (PhidgetVoltageInputHandle *)&canal->handle );
    else if (!strcasecmp(capteur, "1130-PH"))              PhidgetVoltageInput_delete     ( (PhidgetVoltageInputHandle *)&canal->handle );
    else if (!strcasecmp(capteur, "1130-ORP"))             PhidgetVoltageInput_delete     ( (PhidgetVoltageInputHandle *)&canal->handle );
    else if (!strcasecmp(capteur, "TMP1200_0-PT100-3850")) PhidgetTemperatureSensor_delete( (PhidgetTemperatureSensorHandle *)&canal->handle );
    else if (!strcasecmp(capteur, "TMP1200_0-PT100-3920")) PhidgetTemperatureSensor_delete( (PhidgetTemperatureSensorHandle *)&canal->handle );
    else if (!strcasecmp(capteur, "AC-CURRENT-10A"))       PhidgetVoltageInput_delete     ( (PhidgetVoltageInputHandle *)&canal->handle );
    else if (!strcasecmp(capteur, "AC-CURRENT-25A"))       PhidgetVoltageInput_delete     ( (PhidgetVoltageInputHandle *)&canal->handle );
    else if (!strcasecmp(capteur, "AC-CURRENT-50A"))       PhidgetVoltageInput_delete     ( (PhidgetVoltageInputHandle *)&canal->handle );
    else if (!strcasecmp(capteur, "AC-CURRENT-100A"))      PhidgetVoltageInput_delete     ( (PhidgetVoltageInputHandle *)&canal->handle );
    else if (!strcasecmp(capteur, "TEMP_1124_0"))          PhidgetVoltageRatioInput_delete( (PhidgetVoltageRatioInputHandle *)&canal->handle );
    else if (!strcasecmp(capteur, "DIGITAL-INPUT"))        PhidgetDigitalInput_delete     ( (PhidgetDigitalInputHandle *)&canal->handle );
    else if (!strcasecmp(capteur, "REL2001_0"))            PhidgetDigitalOutput_delete    ( (PhidgetDigitalOutputHandle *)&canal->handle );
    g_free(canal);
  }
/******************************************************************************************************************************/
/* Phidget_SET_DO: Met a jour une sortie TOR en fonction du jsonnode en parametre                                             */
/* Entrée: le module et le buffer Josn                                                                                        */
/* Sortie: Niet                                                                                                               */
/******************************************************************************************************************************/
 static void Phidget_SET_DO ( struct ABLS_AGENT *agent, JsonNode *msg )
  { struct ABLS_PHIDGET_VARS *vars = agent->vars;
    gchar *agent_acronyme = Json_get_string ( msg, "mqtt_token_lvl2" );
    gchar *tech_id        = Json_get_string ( msg, "tech_id" );
    gchar *acronyme       = Json_get_string ( msg, "acronyme" );

    if (!agent_acronyme)
     { Info( __func__, agent->agent_classe, agent->agent_tech_id, LOG_ERR, "Requete mal formée manque mqtt_token_lvl2" );
       return;
     }

    if (!Json_has_member ( msg, "etat" ))
     { Info( __func__, agent->agent_classe, agent->agent_tech_id, LOG_ERR, "Requete mal formée manque etat" );
       return;
     }

    gboolean etat = Json_get_bool ( msg, "etat" );
    Info( __func__, agent->agent_classe, agent->agent_tech_id, LOG_NOTICE, "SET_DO '%s:%s'/'%s:%s'=%d",
          agent->agent_tech_id, agent_acronyme, tech_id, acronyme, etat );

    GSList *liste = vars->Liste_sensors;
    while (liste)
     { struct ABLS_PHIDGET_ELEMENT *canal = liste->data;
       gchar *agent_acronyme = Json_get_string(canal->element, "agent_acronyme");
       gchar *classe         = Json_get_string(canal->element, "classe");

       if ( !strcasecmp ( classe, "DO" ) &&
            !strcasecmp ( agent_acronyme, agent_acronyme ) )
        { if ( PhidgetDigitalOutput_setState( (PhidgetDigitalOutputHandle)canal->handle, etat ) != EPHIDGET_OK )
           { Phidget_print_error ( canal ); }
          break;
        }
       liste = g_slist_next(liste);
     }
  }
/******************************************************************************************************************************/
/* Run_thread: Prend en charge un des sous thread de l'agent                                                                  */
/* Entrée: la structure THREAD associée                                                                                       */
/* Sortie: Niet                                                                                                               */
/******************************************************************************************************************************/
 gint main ( gint argc, gchar *argv[] )
  { struct ABLS_AGENT *agent = Agent_init ( argv[0], "phidget", ABLS_AGENT_PHIDGET_VERSION, sizeof(struct ABLS_PHIDGET_VARS), argc, argv );
    struct ABLS_PHIDGET_VARS *vars = agent->vars;

    gchar *hostname    = Json_get_string ( agent->api_config, "hostname" );
    gchar *password    = Json_get_string ( agent->api_config, "password" );
    gchar *description = Json_get_string ( agent->api_config, "description" );

again:
    PhidgetReturnCode result = PhidgetNet_addServer( hostname, hostname, 5661, password, 0 );
    if (result != EPHIDGET_OK)
     { const gchar *error;
       Phidget_getErrorDescription ( result, &error );
       Info( __func__, agent->agent_classe, agent->agent_tech_id, LOG_ERR, "PhidgetNet_addServer '%s' (%s) failed: '%s'",
             hostname, description, error );
       sleep(10);
       goto again;
     } else Info( __func__, agent->agent_classe, agent->agent_tech_id, LOG_INFO, "PhidgetNet_addServer '%s' (%s) success", hostname, description );

/* Chargement des I/O */
    Json_foreach_array_element ( agent->api_config, "IO", Phidget_Charger_un_IO, agent );

    while(agent->Agent_run == TRUE)                                                          /* On tourne tant que necessaire */
     { Agent_loop ( agent );                                             /* Loop sur l'agent pour mettre a jour la telemetrie */
/************************************************* Calcul de la comm **********************************************************/
       GSList *elements = vars->Liste_sensors;
       while ( elements )                                             /* Si tous les sensors sont attached, alors comm = TRUE */
        { struct ABLS_PHIDGET_ELEMENT *element = elements->data;
          if(element->attached == FALSE) break;
          elements = g_slist_next ( elements );
        }
       Agent_send_comm_to_master ( agent, (elements ? FALSE : TRUE) );
/****************************************************** Ecoute du master ******************************************************/
       JsonNode *mqtt_local_message;
       while ( (mqtt_local_message = Mqtt_get_message ( agent->mqtt_local ) ) != NULL )
        { if (Mqtt_topic_is ( mqtt_local_message, 2, "SET_DO", agent->agent_tech_id ))
           { Phidget_SET_DO ( agent, mqtt_local_message ); }
          Json_unref (mqtt_local_message);
        }
     }

    PhidgetNet_removeServer( hostname );                                                /* Arrete la connexion au hub phidget */
    g_slist_free_full ( vars->Liste_sensors, (GDestroyNotify) Phidget_Decharger_un_IO );
    Phidget_finalize(0); /* non thread_safe apres. */
    Agent_end(agent);
  }
/*----------------------------------------------------------------------------------------------------------------------------*/
