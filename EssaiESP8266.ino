#include <ESP8266WiFi.h>
#include <ID-PSW.h>	/* Inclus les #define de:
		NOMWIFI (SSID de mon réseau Wifi)
		MDPWIFI (Mot de passe de mon réseau WiFi)
		DOMOTICS (adresse IP sous format texte de mon serveur)
	et la déclaration de
		const uint16_t PORT
		IPAddress IPSTATIC
		IPAddress PASSERELLE
		IPAddress MaskNet	*/
#include "GerErreurs.h"
#include "HTTPMonEsp.h"
#include "InitMonESPWifi.h"

  
//DEMMARRAGE
void setup(){//_________________________SETUP__________________________________
int Etat;

Serial.begin(115200);
//Serial.setDebugOutput(true);	Affiche du debug sur la connexion Wifi ...
delay(2000);                // Une petite pause au démmarage pour que l'affichage se passe bien sur le terminal ...
SerialPrintParamMonEsp();	//Print sur serial des paramètres de mon ESP (aucune config)
Etat = ConfigWifiMonEsp();
if (Etat<0) GerErreurs(Etat);
}

void loop(void){//__________________________LOOP________________________________
String ReponseBrute;				// Variable String récupérant RepDomotics
String Etat24="24";	// !!!!!!!!!!!!!!!! A adapter pour le besoin !!!!!!!!!!!!!

//INTERROGATION DOMOTICS
HTTPMonEsp(&ReponseBrute,"devices&rid=24");		// !!!!!!!!!!!!!!!! A adapter pour le besoin !!!!!!!!!!!!!
Etat24 = ParseJson(&ReponseBrute,String(Etat24),"Data");
Serial.printf("\nValeur de Etat24: ");			// !!!!!!!!!!!!!!!! A adapter pour le besoin !!!!!!!!!!!!!
Serial.println(Etat24);
ReponseBrute="";

//ACTION
HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=24&switchcmd=On");// !!!!!!!!!!!!!!!! A adapter pour le besoin !!!!!!!!!!!!!

//Si Erreur
//HTTPMonEsp(&ReponseBrute,"command&param=addlogmessage&message=!!!'Poule': Réponse Domotics Innatendu");
//GerErreurs(-100);



//DODO
yield();		//C'est pour donner la main a la couche TCPIP avant de s'endormir.				
digitalWrite(IN5,false);
Serial.print("Dodo 30mn   ^_^\n\n\n\n");
ESP.deepSleep(1800000000,WAKE_RF_DEFAULT);	//le fameux mode deepsleep en µsec: 300 000 000 = 5mn, 1800000000 =30mn
delay(60000);	// Pause de 60sec (60000msec), mais qui ne s'excute jamais lorsque le deepSleep est réalisé
}