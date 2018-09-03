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
#define IN5 14
boolean Etat = false;
boolean Retour = false;
 
//DEMMARRAGE
void setup(){//_________________________SETUP__________________________________
//int Etat;
delay(5000);
pinMode(12,OUTPUT);		// GPIO12 = Relays, Note: il ne fonctionne qu'avec le 220Volts
pinMode(13,OUTPUT);		// GPIO13 = LED
pinMode(0,INPUT_PULLUP);		// GPIO0 = Bouton, au repos GPIO0 = High = true, Appuyé GPIO0= LOW = false

Serial.begin(115200);
digitalWrite(13, HIGH);
delay(5000);
digitalWrite(13, LOW);

}

void loop(void){//__________________________LOOP________________________________
if (!digitalRead(0)){
	Serial.printf("Bouton Appuye");
	Serial.printf("Bouton ");
	Serial.println(digitalRead(0));
	Etat=true;
}
digitalWrite(13,Etat);
digitalWrite(12,Etat);
Serial.printf("LED ");
Serial.println(Etat);
Etat=false;
delay(1000);
}