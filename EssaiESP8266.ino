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

ADC_MODE(ADC_VCC); // Doit être  "tout en haut"
  
//DEMMARRAGE
void setup(){//_________________________SETUP__________________________________
int Etat=0;

Serial.begin(115200);
//Serial.setDebugOutput(true);	Affiche du debug sur la connexion Wifi ...
delay(2000);                // Une petite pause au démmarage pour que l'affichage se passe bien sur le terminal ...
SerialPrintParamMonEsp();	//Print sur serial des paramètres de mon ESP (aucune config)
Etat = ConfigWifiMonEsp();
if (Etat<0) GerErreurs(Etat);

//Pour terster les SLEEP Mode
//wifi_set_sleep_type(MODEM_SLEEP_T);		//Active le Light Sleep Mode (Wifi Off, Clock ON, CPU ON => 15mA)
//wifi_set_sleep_type(LIGHT_SLEEP_T);		//Active le Light Sleep Mode (Wifi Off, Clock OFF, CPU Pending => 0.4mA)
//Ca marche pas la fct et la variable ne sont pas déclaré ...

//Raison du reset
Serial.printf("Raison du booot: ");
Serial.println(ESP.getResetReason()); //Renvoi un string "human" readable de la raison du démmarage
//0 	unknown, 1 	normal boot, 2 	reset pin, 3 	software reset, 4 	watchdog reset

//Pour lire la tension d'alimentation

Serial.printf("\n Tension Alim: ");
Serial.println(ESP.getVcc());	// pour lire la tension de l'alim

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
Serial.print("Dodo 30mn   ^_^\n\n\n\n");
ESP.deepSleep(1800000000,WAKE_RF_DEFAULT);	//le fameux mode deepsleep en µsec: 300 000 000 = 5mn, 1800000000 =30mn
delay(60000);	// Pause de 60sec (60000msec), mais qui ne s'excute jamais lorsque le deepSleep est réalisé
}