/*
Paul le 13/04/2018
InitMonESPWifi.cpp

Librairie qui contient tout la séquence de paramétrage par mes défaut de l'ESP
et la connexion au Wifi

Le fichier .cpp contient le code source de ma librairie
Le fichier .h est le fichier d'entête (pour décalrer les prototypes ou d'autre define ou cte)
Pour rappel: a la compiltation "#include <ID-PSW.h>" est remplacé par le contenu de ID-PSW.h, le fichier ".ccp" doit être rajouté au projet (via IDE ou commande make)
Le fichier "keywords.txt" est dédié à l'aide contexctuel de l'IDE d'Arduino
*/
#include <ESP8266WiFi.h>

void SerialPrintParamMonEsp(){
String EtatWifi="0";		//#ToDo probablement à viré vu que ça semble pas marché (voir plus bas)
//Message de boot sur port série: Détailles concernant l'ESP
Serial.printf("\n\n");
Serial.printf("ID de l'ESP: ");
Serial.println(ESP.getChipId());
Serial.printf("Taille de la memoire: ");
Serial.println(ESP.getFlashChipRealSize());
Serial.printf("Taille du code: ");
Serial.println(ESP.getSketchSize());
Serial.printf("Memoire restante: ");
Serial.println(ESP.getFreeSketchSpace());
Serial.printf("Taille de la pile: ");
Serial.println(ESP.getFreeHeap());
Serial.printf("Raison du Boot: ");
Serial.println(ESP.getResetReason());
EtatWifi=WiFi.getMode();			//(0,1,2,3)=WIFI_AP, WIFI_STA, WIFI_AP_STA or WIFI_OFF ... mais il semblerait que je récupère 3 dans tout les cas :/
Serial.printf("Etat du Wifi= %s\n",EtatWifi.c_str());
}

