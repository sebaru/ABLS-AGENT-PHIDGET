/******************************************************************************************************************************/
/* ABLS-AGENT-PHIDGET/phidget.h   Header et constantes des modules Phidget                                                    */
/* Projet Abls-Habitat                   Gestion d'habitat                                                18.03.2021 21:59:33 */
/* Auteur: LEFEVRE Sebastien                                                                                                  */
/******************************************************************************************************************************/
/*
 * Phidget.h
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

#ifndef _ABLS_AGENT_PHIDGET_H_
 #define _ABLS_AGENT_PHIDGET_H_
 #include <phidget22.h>
 #include <abls-agent-libs/abls-agent-libs.h>

 struct ABLS_PHIDGET_VARS                                                            /* Communication entre DLS et la Phidget */
  { GSList *Liste_sensors;                                                                        /* List of Phidget Elements */
  };

 struct ABLS_PHIDGET_ELEMENT
  { struct THREAD *module; /* Module père de l'élément */
    PhidgetHandle handle;
    gboolean attached;
    JsonNode *element; /* Issu du module->config au chargement */
  };

/****************************************************** Déclaration des prototypes ********************************************/

#endif
/*----------------------------------------------------------------------------------------------------------------------------*/
