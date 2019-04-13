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
IPAddress IPSTATIC(192,168,1,121);
#include "GerErreurs.h"
#include "HTTPMonEsp.h"
#include "InitMonESPWifi.h"

#include "DHT.h"
#define IO5 14				//Pin de la LED
#define DHTTYPE DHT22		//Type de capteur de DHT pour lecture température
#define DHTPIN 5			//Pin relié au DHT
DHT dht(DHTPIN, DHTTYPE);	//Structure(?!) de la lib DHT ...
#define IDX_DHT 21

#define IDX_EtatP 24		// IDX 24= RetourEtatPortePoules
#define IDX_Consigne 25		// IDX 25= ConsignePortePoules

#define VITESSE 900			// 1'30" pour les 24tours
#define NBTOUR (4095*24)


String ReponseBrute;		//Variable globale String pour les requète HTTP récupérant RepDomotics  ... c'est moche mais
float t, h;					//Variables pour DHT

//DEMMARRAGE
void setup(){//_________________________SETUP__________________________________
int Etat;
// Paramétrage des 4 IO dédié au stepper en mode output, dédié au stepper
pinMode(IO1,OUTPUT);
pinMode(IO2,OUTPUT);
pinMode(IO3,OUTPUT);
pinMode(IO4,OUTPUT);
// Paramétrage de la pin pour la LED et allumage
pinMode(IO5,OUTPUT);
digitalWrite(IO5,true);
dht.begin();


Serial.begin(115200);
//Serial.setDebugOutput(true);	Affiche du debug sur la connexion Wifi ...
delay(2000);                // Une petite pause au démmarage pour que l'affichage se passe bien sur le terminal ...
RecuperationTempHum();		//On interroge le DHT avant d'activer le wifi ... sinon avec le RT et le IT les timming sont pas respectés ...
SerialPrintParamMonEsp();	//Print sur serial des paramètres de mon ESP (aucune config)
Etat = ConfigWifiMonEsp();
if (Etat<0) GerErreurs(Etat);
}

void loop(void){//__________________________LOOP________________________________

String Etat24 = String(IDX_EtatP), Etat25= String(IDX_Consigne);	// IDX 24= RetourEtatPortePoules, IDX 25= ConsignePortePoules
String Msg="";

//INTERROGATION DOMOTICS
ReponseBrute="";
Msg = "devices&rid=" + String(IDX_EtatP);
HTTPMonEsp(&ReponseBrute,Msg);
Etat24 = ParseJson(&ReponseBrute,String(Etat24),"Data"); //IDX 24= EtatPortePoules
Serial.printf("\nValeur de Etat24: ");
Serial.println(Etat24);

ReponseBrute="";
Msg = "devices&rid=" + String(IDX_Consigne);
HTTPMonEsp(&ReponseBrute,Msg);
Etat25 = ParseJson(&ReponseBrute,String(Etat25),"Data"); //IDX 25= ConsignePortePoules
Serial.printf("Valeur de Etat25: ");
Serial.println(Etat25);
ReponseBrute="";


//ACTION MOTEUR CONDITIONNELLE + MàJ DOMOTICS
if (Etat24!=Etat25){
	if (Etat25=="On"){
		Serial.printf("Domoticz demande \"ON\" => Tourner moteur dans le sens horaire\n");
		TournerMoteur(false,NBTOUR,VITESSE);  //(Cw, NbSteps: 4095 = 1tour, Speed € [900-1000])
		Serial.printf("Fin de rotation moteur.\nMaJ Domotics...\n");
		Msg = "command&param=switchlight&idx=" + String(IDX_EtatP) + "&switchcmd=On";
		HTTPMonEsp(&ReponseBrute, Msg);
	}else if (Etat25=="Off"){
		Serial.print("Domoticz demande \"OFF\" => Tourner moteur dans le sens ANTI-horaire\n");
		TournerMoteur(true,NBTOUR,VITESSE);
		Serial.printf("Fin de rotation moteur.\nMaJ Domotics...\n");
		Msg = "command&param=switchlight&idx=" +  String(IDX_EtatP) + "&switchcmd=Off";
		HTTPMonEsp(&ReponseBrute, Msg);
	}else{
		HTTPMonEsp(&ReponseBrute,"command&param=addlogmessage&message=!!!'Poule': Réponse Domotics Innatendu");
		GerErreurs(-100);
	}
}else{
	Serial.printf("Domoticz ne demande pas de changement.\nIDX24= %s\tIDX25=%s\n",Etat24.c_str(), Etat25.c_str());
}

//VERIFICATION QUE LA COMMANDE A ETE PRISE EN COMPTE
Serial.print("\nVerification prise en compte commande par Domotics\n");
Etat24 = String(IDX_EtatP), Etat25 = String(IDX_Consigne);	// IDX 24= RetourEtatPortePoules, IDX 25= ConsignePortePoules

Msg = "devices&rid=" + String(IDX_EtatP);
HTTPMonEsp(&ReponseBrute,Msg);
Etat24 = ParseJson(&ReponseBrute,String(Etat24),"Data"); //IDX 24= RetourEtatPortePoules
Serial.printf("\nValeur de Etat24: ");
Serial.println(Etat24);
ReponseBrute="";

Msg = "devices&rid=" + String(IDX_Consigne);
HTTPMonEsp(&ReponseBrute,Msg);
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

//Mise à jour de la température et humidité extérieure
Msg = "command&param=udevice&idx=" + String(IDX_DHT) + "&nvalue=0&svalue=";
Msg += String(t) + ";" + String(h) + ";0";
HTTPMonEsp(&ReponseBrute, Msg);


//DODO
yield();		//C'est pour donner la main a la couche TCPIP avant de s'endormir.				
digitalWrite(IO5,false);
Serial.print("Dodo 30mn   ^_^\n\n\n\n");
ESP.deepSleep(1800000000,WAKE_RF_DEFAULT);	//le fameux mode deepsleep en µsec: 300 000 000 = 5mn, 1800000000 =30mn
delay(60000);	// Pause de 60sec (60000msec), mais qui ne s'excute jamais lorsque le deepSleep est réalisé
}



void RecuperationTempHum(){
int okKo = 1;
do{
	h = dht.readHumidity();	//Lit l'humidité
    t = dht.readTemperature();	//Lit la température en Celcius (cas sans argument)
	
	// Check if any reads failed and exit early (to try again).
	if (isnan(h) || isnan(t)) {
		Serial.println("!!! Failed to read from DHT sensor!");
		delay(2000);
		okKo++;
		if(okKo>20) GerErreurs(-10001);
	}else{
		Serial.printf("Temp= %f°C \tHumidité= %f%% \tNbTentative= %d\n",t,h,okKo);
		okKo = 0;
	}	
}while(okKo!=0);
}

	
	
	
	
	






