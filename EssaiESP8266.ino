#include <ESP8266WiFi.h>
#include "StepperPWI.h"		//La librairie utilisé pour piloter le moteur steps
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
#define IN5 14
  
//DEMMARRAGE
void setup(){//_________________________SETUP__________________________________
int Etat;
// Paramétrage des 4 IO dédié au stepper en mode output, dédié au stepper
pinMode(IN1,OUTPUT);
pinMode(IN2,OUTPUT);
pinMode(IN3,OUTPUT);
pinMode(IN4,OUTPUT);
// Paramétrage de la pin pour la LED et allumage
pinMode(IN5,OUTPUT);
digitalWrite(IN5,true);

Serial.begin(115200);
//Serial.setDebugOutput(true);	Affiche du debug sur la connexion Wifi ...
delay(2000);                // Une petite pause au démmarage pour que l'affichage se passe bien sur le terminal ...
SerialPrintParamMonEsp();	//Print sur serial des paramètres de mon ESP (aucune config)
Etat = ConfigWifiMonEsp();
if (Etat<0) GerErreurs(Etat);
}

void loop(void){//__________________________LOOP________________________________
String ReponseBrute;				// Variable String récupérant RepDomotics
String Etat24="24", Etat25="25";	// IDX 24= RetourEtatPortePoules, IDX 25= ConsignePortePoules

//INTERROGATION DOMOTICS
HTTPMonEsp(&ReponseBrute,"devices&rid=24");
Etat24 = ParseJson(&ReponseBrute,String(Etat24),"Data"); //IDX 24= EtatPortePoules
Serial.printf("\nValeur de Etat24: ");
Serial.println(Etat24);
ReponseBrute="";

HTTPMonEsp(&ReponseBrute,"devices&rid=25");
Etat25 = ParseJson(&ReponseBrute,String(Etat25),"Data"); //IDX 25= ConsignePortePoules
Serial.printf("Valeur de Etat25: ");
Serial.println(Etat25);
ReponseBrute="";

//ACTION MOTEUR CONDITIONNELLE + MàJ DOMOTICS
if (Etat24!=Etat25){
	if (Etat25=="On"){
		Serial.printf("Domoticz demande \"ON\" => Tourner moteur dans le sens horaire\n");
		TournerMoteur(true,4095,900);  
		Serial.printf("Fin de rotation moteur.\nMaJ Domotics...\n");
		HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=24&switchcmd=On");
	}else if (Etat25=="Off"){
		Serial.print("Domoticz demande \"OFF\" => Tourner moteur dans le sens ANTI-horaire\n");
		TournerMoteur(false,4095,900);
		Serial.printf("Fin de rotation moteur.\nMaJ Domotics...\n");
		HTTPMonEsp(&ReponseBrute, "command&param=switchlight&idx=24&switchcmd=Off");
	}else{
		HTTPMonEsp(&ReponseBrute,"command&param=addlogmessage&message=!!!'Poule': Réponse Domotics Innatendu");
		GerErreurs(-100);
	}
}else{
	Serial.printf("Domoticz ne demande pas de changement.\nIDX24= %s\tIDX25=%s\n",Etat24.c_str(), Etat25.c_str());
}

//VERIFICATION QUE LA COMMANDE A ETE PRISE EN COMPTE
Serial.print("\nVerification prise en compte commande par Domotics\n");
Etat24="24", Etat25="25";	// IDX 24= RetourEtatPortePoules, IDX 25= ConsignePortePoules
HTTPMonEsp(&ReponseBrute,"devices&rid=24");
Etat24 = ParseJson(&ReponseBrute,String(Etat24),"Data"); //IDX 24= RetourEtatPortePoules
Serial.printf("\nValeur de Etat24: ");
Serial.println(Etat24);
ReponseBrute="";

HTTPMonEsp(&ReponseBrute,"devices&rid=25");
Etat25 = ParseJson(&ReponseBrute,String(Etat25),"Data"); //IDX 25= ConsignePortePoules
Serial.printf("Valeur de Etat25: ");
Serial.println(Etat25);
ReponseBrute="";

if (Etat24!=Etat25){
	HTTPMonEsp(&ReponseBrute,"command&param=addlogmessage&message=PouleCommandeNonPriseEnCompte");
	GerErreurs(-200);
	}else{
	Serial.print("Modification bien prise en compte par Domoticz.\n");
}

//DODO
yield();		//C'est pour donner la main a la couche TCPIP avant de s'endormir.				
digitalWrite(IN5,false);
Serial.print("Dodo 60sec   ^_^\n\n\n\n");
ESP.deepSleep(1800000000,WAKE_RF_DEFAULT);	//le fameux mode deepsleep en µsec: 300 000 000 = 5mn
delay(60000);	// Pause de 60sec (60000msec), mais qui ne s'excute jamais lorsque le deepSleep est réalisé
}