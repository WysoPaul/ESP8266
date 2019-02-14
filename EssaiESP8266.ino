#include <ESP8266WiFi.h>
ADC_MODE(ADC_VCC);
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

#define IOFENETRE 13	// GPIO relié à l'ILS qui détecte l'ouverture de la fenetre (HIGH = Fenetre ouverte / Aimant absent)
#define IOBUZZER 14		// GPIO relié au Buzzer
#define IDX_WC 7		// IDX define dans Domotics (## a mettre à jour)
char OldFenetre = 0;		// ## Pour que ca marche avec un deepsleep il faudra sauvegardé la valeur entre les reboot (sinon il faudrait interrrogé Domotics à chaque (!) reveille :(  )
int i_DureeOuvert;		// ## idem à sauvegarder dans la mémoire de la clock

//DEMMARRAGE
void setup(){//_________________________SETUP__________________________________

//Initialisation
String ReponseBrute, EtatIDX = "";
int Etat=0;		//Enregistre les retours des fonctions
//String PrkReboot = "command&param=addlogmessage&message=";
pinMode(IOFENETRE,INPUT_PULLUP);
pinMode(IOBUZZER,OUTPUT);
String S_boot=ESP.getResetReason();		//C'est bien un String!
Serial.begin(115200);

Serial.println("Tension alim: ");
Serial.println(ESP.getVcc());

//Actions
if(String("Deep-Sleep Wake")==S_boot){						//Vérifier si switch sur arduino gère les string
			//Actions in case of Deepsleep wake-up:
			//Read Fenetre
			//Si fenêtre fermé
				//Si OldFenetre = Fenetre
					//Sortie if et switch (pour atteindre Dodo ^_^ )
				//Sinon (càd fenêtre était ouverte)
					//Ecriture terminal
					//Reset i_DureeOuvert
					//Connexion Wifi
					//Connexion Domotics
					//Màj etat IDX => OFF
					//MàJ tension alim
			//Sinon (càd fenetre ouverte)
				//Incrémentation DureeOuvert ...
				//Si OldFenetre != Fenetre
					//Ecriture terminal
					//Connexion Wifi
					//Connexion Domotics
					//Màj etat IDX => ON
					//MàJ tension alim
				//Sinon rien
}else if(String("External System")==S_boot){
			//Actions in case of reset:
			//OldFenetre <= Fenetre
			//i_DureeOuvert = 0
			//Ecriture sur terminal série
			//Connexion wifi
			//Connexion Domotics
			//Ecriture dans Log "boot reason"
			//Màj etat IDX (ou Récupérer valeur et synchronisation ...)
			//MàJ tension alim
			//=> Dodo
}
//		ElseIFcase "Software Watchdog":
			//Actions in case of watchdog reboot:
			//OldFenetre <= Fenetre
			//i_DureeOuvert = 0
			//Ecriture sur terminal série
			//Connexion wifi
			//Connexion Domotics
			//Ecriture dans Log "boot reason"
			//Màj etat IDX (ou Récupérer valeur et synchronisation ...)
			//MàJ tension alim
			//=> Dodo

//Dodo ^_^

//Petit bout de code pour lire et ecrire mémoire ...
ESP.rtcUserMemoryRead(32, (uint32_t*)  &OldFenetre, sizeof(OldFenetre));
Serial.println("Valeur en memoire: ");
Serial.println(OldFenetre);
Serial.println("Incrementation de val memoire de +1");
OldFenetre += 1;
ESP.rtcUserMemoryWrite(32, (uint32_t*) &OldFenetre, sizeof(OldFenetre));
ESP.rtcUserMemoryRead(32, (uint32_t*) &OldFenetre, sizeof(OldFenetre));
Serial.println("Nouvelle valeur en memoire: ");
Serial.println(OldFenetre);


Serial.print("\n\nRAISON DU BOOT: ");
Serial.println(S_boot);
	


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

bool Porte;
Porte = digitalRead(IOPORTE);
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
//Dodo ^_^
yield();
delay(500); //a remplacer par un deep sleep
}

void loop(void){//__________________________LOOP________________________________
}