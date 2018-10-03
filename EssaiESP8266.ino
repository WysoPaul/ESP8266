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

#define IOPORTE 13			// GPIO relié au capteur d'ouverture de porte
#define IOVERROU 12			// GPIO relié au capteur du vérou de la porte
#define IDX_PORTE 1		// Idem pour les IDX de Domotics
#define IDX_VERROU 25
bool OldPorte, OldVerrou;

//DEMMARRAGE
void setup(){//_________________________SETUP__________________________________
int Etat=0;
pinMode(IOPORTE,INPUT);
pinMode(IOVERROU,INPUT);
OldPorte = digitalRead(IOPORTE);
OldVerrou = digitalRead(IOVERROU);
Etat = ConfigWifiMonEsp();	//Paramétrage Wifi et initialisation connexion
if (Etat<0) GerErreurs(Etat);
}

void loop(void){//__________________________LOOP________________________________
String ReponseBrute;		// Variable String récupérant RepDomotics
String EtatIDX = "";
bool Porte, Verrou;

Porte = digitalRead(IOPORTE);
Verrou = digitalRead(IOVERROU);
Serial.begin(115200);
Serial.print("\n\n-------------------------------------------------------------\n\n");
Serial.print("FC Porte = ");
Serial.println(Porte);
Serial.print("FC OldPorte = ");
Serial.println(OldPorte);
yield();

if (OldPorte !=Porte){
	//INTERROGATION DOMOTICS sur état IDX_PORTE
	EtatIDX	=	"devices&rid=";
	EtatIDX += IDX_PORTE;
	HTTPMonEsp(&ReponseBrute,EtatIDX);
	EtatIDX = ParseJson(&ReponseBrute,String(IDX_PORTE),"Data");	// String ParseJson(String MessageHHTP, String IDX_Cherché, String Champ recherché)
	ReponseBrute = "\nValeur de IDX_";
	ReponseBrute += IDX_PORTE;
	ReponseBrute += ": ";
	ReponseBrute += EtatIDX;
	Serial.println(ReponseBrute);
	ReponseBrute="";
	if (true == Porte && String("On") != EtatIDX)	HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=1&switchcmd=On");
	if (false == Porte && String("Off") != EtatIDX)	HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=1&switchcmd=Off");
	OldPorte = Porte;
}
//Pause
delay(500);
}