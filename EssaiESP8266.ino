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
IPAddress IPSTATIC(192,168,1,206);	// Adresse IP de mon équipement !!-- A ADAPTER A CHAQUE COMPILATION, surtout EN MODE TEST --!!
#include "GerErreurs.h"
#include "HTTPMonEsp.h"
#include "InitMonESPWifi.h"

#define IOPORTE 13			// GPIO relié au capteur d'ouverture de porte
#define IOVERROU 14			// GPIO relié au capteur du vérou de la porte (6/4/19: IO12 cramé, passage à IO14)
#define IDX_PORTE 7		// Idem pour les IDX de Domotics
#define IDX_VERROU 8
bool OldPorte, OldVerrou;
String ReponseBrute, EtatIDX = "";

//DEMMARRAGE
void setup(){//_________________________SETUP__________________________________
int Etat=0;
String PrkReboot = "command&param=addlogmessage&message=";
pinMode(IOPORTE,INPUT_PULLUP);
pinMode(IOVERROU,INPUT_PULLUP);
OldPorte = digitalRead(IOPORTE);
OldVerrou = digitalRead(IOVERROU);

Serial.begin(115200);
Serial.print("\n\nRAISON DU BOOT: ");
Serial.println(ESP.getResetReason());
Etat = ConfigWifiMonEsp();	//Paramétrage Wifi et initialisation connexion
if (Etat<0) GerErreurs(Etat);

//Ecriture dans log domotics raison Boot
PrkReboot +="IDX";
PrkReboot +=IDX_PORTE;
PrkReboot +="et";
PrkReboot +=IDX_VERROU;
PrkReboot +="_RebootReason_";
PrkReboot += ESP.getResetReason();
PrkReboot.replace(" ", "");
Serial.println("INSCRIPTION DANS LOG DOMOTICS\n");
HTTPMonEsp(&ReponseBrute,PrkReboot);
//PrkReboot.equalsIgnoreCase("Software Watchdog")

//SYNCHRONISATION INITIALE ENTRE ETAT PORT ET DOMOTICS
Serial.print("\nSYNCHRONISATION INITIALE ENTRE ETAT PORT ET DOMOTICS\n");
EtatIDX	=	"devices&rid=";
EtatIDX += IDX_PORTE;
HTTPMonEsp(&ReponseBrute,EtatIDX);
EtatIDX = ParseJson(&ReponseBrute,String(IDX_PORTE),"Data");	// String ParseJson(String MessageHHTP, String IDX_Cherché, String Champ recherché)
ReponseBrute = "\nVALEUR DE IDX_";
ReponseBrute += IDX_PORTE;
ReponseBrute += ": ";
ReponseBrute += EtatIDX;
Serial.println(ReponseBrute);
Serial.print("FC OldPorte = ");
Serial.println(OldPorte);
ReponseBrute="";
if (true == OldPorte && String("On") != EtatIDX)	HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=7&switchcmd=On");		// !!! Faut vraiment faire une concaténation avec IDX_PORTE ...
if (false == OldPorte && String("Off") != EtatIDX)	HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=7&switchcmd=Off");		// !!! Faut vraiment faire une concaténation avec IDX_PORTE ...

//SYNCHRONISATION INITIALE ENTRE ETAT VERROU ET DOMOTICS
Serial.print("\nSYNCHRONISATION INITIALE ENTRE ETAT VERROU ET DOMOTICS\n");
EtatIDX	=	"devices&rid=";
EtatIDX += IDX_VERROU;
HTTPMonEsp(&ReponseBrute,EtatIDX);
EtatIDX = ParseJson(&ReponseBrute,String(IDX_VERROU),"Data");	// String ParseJson(String MessageHHTP, String IDX_Cherché, String Champ recherché)
ReponseBrute = "\nVALEUR DE IDX_";
ReponseBrute += IDX_VERROU;
ReponseBrute += ": ";
ReponseBrute += EtatIDX;
Serial.println(ReponseBrute);
Serial.print("FC OldVerrou = ");
Serial.println(OldVerrou);
ReponseBrute="";
if (true == OldVerrou && String("On") != EtatIDX)	HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=8&switchcmd=On");		// !!! Faut vraiment faire une concaténation avec IDX_PORTE ...
if (false == OldVerrou && String("Off") != EtatIDX)	HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=8&switchcmd=Off");		// !!! Faut vraiment faire une concaténation avec IDX_PORTE ...




}

void loop(void){//__________________________LOOP________________________________
bool Porte, Verrou;
Porte = digitalRead(IOPORTE);
Verrou = digitalRead(IOVERROU);
yield();

//Control de Porte et action en fct
if (OldPorte !=Porte){
	//INTERROGATION DOMOTICS sur état IDX_PORTE
	EtatIDX	=	"devices&rid=";
	EtatIDX += IDX_PORTE;
	HTTPMonEsp(&ReponseBrute,EtatIDX);
	EtatIDX = ParseJson(&ReponseBrute,String(IDX_PORTE),"Data");	// String ParseJson(String MessageHHTP, String IDX_Cherché, String Champ recherché)
	ReponseBrute = "\nVALEUR DE IDX_";
	ReponseBrute += IDX_PORTE;
	ReponseBrute += ": ";
	ReponseBrute += EtatIDX;
	Serial.println(ReponseBrute);
	Serial.print("FC Porte = ");
	Serial.println(Porte);
	Serial.print("FC OldPorte = ");
	Serial.println(OldPorte);
	Serial.print("\n==> ENVOIE COMMANDE DOMOTICS\n");
	ReponseBrute="";
	if (true == Porte && (String("On") != EtatIDX)){
		HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=7&switchcmd=On");		// !!! Faut vraiment faire une concaténation avec IDX_PORTE ...
	}else if (false == Porte && String("Off") != EtatIDX){
		HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=7&switchcmd=Off");		// !!! Faut vraiment faire une concaténation avec IDX_PORTE ...
	}
	OldPorte = Porte;
}
yield();

//Control du verrou et action en fct
if (OldVerrou !=Verrou){
	//INTERROGATION DOMOTICS sur état IDX_VERROU
	EtatIDX	=	"devices&rid=";
	EtatIDX += IDX_VERROU;
	HTTPMonEsp(&ReponseBrute,EtatIDX);
	EtatIDX = ParseJson(&ReponseBrute,String(IDX_VERROU),"Data");	// String ParseJson(String MessageHHTP, String IDX_Cherché, String Champ recherché)
	ReponseBrute = "\nVALEUR DE IDX_";
	ReponseBrute += IDX_VERROU;
	ReponseBrute += ": ";
	ReponseBrute += EtatIDX;
	Serial.println(ReponseBrute);
	Serial.print("FC Verrou = ");
	Serial.println(Verrou);
	Serial.print("FC OldVerrou = ");
	Serial.println(OldVerrou);
	Serial.print("\n==> ENVOIE COMMANDE DOMOTICS\n");
	ReponseBrute="";
	if (true == Verrou && (String("On") != EtatIDX)){
		HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=8&switchcmd=On");		// !!! Faut vraiment faire une concaténation avec IDX_PORTE ...
	}else if (false == Verrou && String("Off") != EtatIDX){
		HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=8&switchcmd=Off");		// !!! Faut vraiment faire une concaténation avec IDX_PORTE ...
	}
	OldVerrou = Verrou;
}
//Pause
yield();
delay(500);
}