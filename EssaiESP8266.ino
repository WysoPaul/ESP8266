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
bool Porte, Verrou;

//DEMMARRAGE
void setup(){//_________________________SETUP__________________________________
int Etat=0;
pinMode(IOPORTE,INPUT);
pinMode(IOVERROU,INPUT);
Porte = digitalRead(IOPORTE);
Verrou = digitalRead(IOVERROU);
Serial.begin(115200);
Serial.print("\n\n");
Serial.print("FC Porte = ");
Serial.println(Porte);
Serial.print("FC Verrou = ");
Serial.println(Verrou);

//SerialPrintParamMonEsp();	//Print sur serial des paramètres de mon ESP (aucune config)
Etat = ConfigWifiMonEsp();	//Paramétrage Wifi et initialisation connexion
if (Etat<0) GerErreurs(Etat);
}

void loop(void){//__________________________LOOP________________________________
String ReponseBrute;		// Variable String récupérant RepDomotics
String EtatIDX = "";

//INTERROGATION DOMOTICS sur état IDX_PORTE
//DEBUG
EtatIDX	=	"devices&rid=";
EtatIDX += IDX_PORTE;


//HTTPMonEsp(&ReponseBrute,String("devices&rid=") + String(IDX_PORTE));		// ATTENTION, c'est la première fois que je fusionne la chaine de caractères avec une constante
HTTPMonEsp(&ReponseBrute,EtatIDX);
//Fin DEBUG

EtatIDX = ParseJson(&ReponseBrute,String(IDX_PORTE),"Data");	// String ParseJson(String MessageHHTP, String IDX_Cherché, String Champ recherché)
ReponseBrute = "\nValeur de ";
ReponseBrute += IDX_PORTE;
ReponseBrute += ": ";
ReponseBrute += EtatIDX;
Serial.println(ReponseBrute);
ReponseBrute="";

// A SE POSER LA QUESTION
// Pour réduire la conso, si le modem se met/est mis en someille. Il vaut mieux monitorer les changement d'état des IO sans interroger Domotics
// Et sur le lien avec Domotics, enchainner les requètes sur une unique connexion ?! ... pourrait avoir un intérêt

if (true == Porte && String("On") != EtatIDX)	HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=1&switchcmd=On");
if (false == Porte && String("Off") != EtatIDX)	HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=1&switchcmd=Off");

//DODO
yield();		//C'est pour donner la main a la couche TCPIP avant de s'endormir.				
Serial.print("Dodo 0.5sec   ^_^\n\n\n\n");
//ESP.deepSleep(500000,WAKE_RF_DEFAULT);	//activation du mode deepsleep en µsec: 300 000 000 = 5mn, 1800000000 =30mn, 500000 = 0.5Sec
delay(60000);	// Pause de 60sec (60000msec), mais qui ne s'excute jamais lorsque le deepSleep est réalisé
}