#include <ESP8266WiFi.h>
ADC_MODE(ADC_VCC);								//Pour activer la fonction lecture tension alimentation
#include <ID-PSW.h>	/* Inclus les #define de:
		NOMWIFI (SSID de mon réseau Wifi)
		MDPWIFI (Mot de passe de mon réseau WiFi)
		DOMOTICS (adresse IP sous format texte de mon serveur)
	et la déclaration de
		const uint16_t PORT
		IPAddress PASSERELLE
		IPAddress MaskNet	*/
#include "GerErreurs.h"
#include "HTTPMonEsp.h"
#include "InitMonESPWifi.h"

IPAddress IPSTATIC(192,168,1,206);		// Adresse IP de mon équipement !!-- A ADAPTER A CHAQUE COMPILATION, surtout EN MODE TEST --!!
#define IOCAPTEUR 13					// GPIO relié à l'ILS qui détecte l'ouverture de la fenetre (HIGH = Fenetre ouverte / Aimant absent)
#define IOBUZZER 14						// GPIO relié au Buzzer
#define IDX1 7							// IDX define dans Domotics (### a mettre à jour ###)
#define DEEPSLEEP_DURATION 300000000	// En microseconds 300 000 000 = 5mn
#define DUREE_OUVERT_MAX 3				// Nombre de cycles de DeepSLeep avec capteur actif qui entrainent une action

void Notification(uint32_t* NbCycles);
void ReadBoolMemRtc(char Offset, boolean ValBinaire );
void WriteBoolMemRtc(char Offset, boolean ValBinaire);

void setup(){//_________________________SETUP__________________________________

//Declaration
String S_ReponseBrute = "", S_EtatIDX = "", S_PrkReboot = "", S_MessageIDX = "", S_MessageLog = "";
int i_RetourEtatFcts = 0, i_Alim = 0;
boolean b_EtatCapteur1 = false, b_OldEtatCapteur1 = false;
uint32_t ui32_DureeOuvert = 0;			//C'est un 32bits afin de pouvoir le charger sans cast dans la mémoire RTC

//Initialisation
pinMode(IOCAPTEUR,INPUT_PULLUP);
pinMode(IOBUZZER,OUTPUT);
Serial.begin(115200);
S_PrkReboot = ESP.getResetReason();
i_Alim = ESP.getVcc();
b_EtatCapteur1 = digitalRead(IOCAPTEUR);
S_MessageIDX = "command&param=switchlight&idx=" + String(IDX1) + "&switchcmd=";
S_MessageLog = "command&param=addlogmessage&message=";

//######### A Faire ############
//Lire b_OldEtatCapteur1 et ui32_DureeOuvert dans la mémoire RTC
ReadBoolMemRtc(1, b_OldEtatCapteur1);
ESP.rtcUserMemoryRead(32 + 2, (uint32_t*) &ui32_DureeOuvert, sizeof(ui32_DureeOuvert));


//DEBUG affiche sur le terminal
Serial.printf("\n\n\n");
Serial.printf("Raison du boot: ");
Serial.println(S_PrkReboot);
Serial.printf("Tension alim: ");
Serial.println(i_Alim);
Serial.printf("Etat capteur: ");
Serial.println(b_EtatCapteur1);
Serial.printf("Old etat capteur: ");
Serial.println(b_OldEtatCapteur1);
yield();

//Choix actions
if(String("Deep-Sleep Wake") == S_PrkReboot){				//Actions in case of Deepsleep wake-up:
			if(true == b_EtatCapteur1){						//Fenêtre est fermé
				if(b_EtatCapteur1 = b_OldEtatCapteur1){		//La fenetre est et était fermé
					//Dodo ^_^
					ESP.deepSleep((uint32_t)DEEPSLEEP_DURATION,WAKE_NO_RFCAL);
				}
				else{										//On vient de fermer la fenêtre
					ui32_DureeOuvert = 0;
					S_MessageIDX +="Off";
				}
			}else{											//càd fenetre est ouverte
				ui32_DureeOuvert += 1;							// !!!!!!!!!!!!!!! risque de saturer la variable !!!!!!!!!!!!
				if (ui32_DureeOuvert > DUREE_OUVERT_MAX) Notification(&ui32_DureeOuvert);
				if(b_EtatCapteur1 = b_OldEtatCapteur1){		//càd fenêtre est et était ouverte
					//Dodo ^_^
					ESP.deepSleep((uint32_t)DEEPSLEEP_DURATION,WAKE_NO_RFCAL);
				}
				else{										//càd fenêtre était fermé et on vient de l'ouvrir
					S_MessageIDX +="Off";
				}
			}
}else if(String("External System") == S_PrkReboot){			//Actions in case of real reboot:
			b_OldEtatCapteur1 = b_EtatCapteur1;
			ui32_DureeOuvert = 0;
			S_MessageLog += IDX1;
			S_MessageLog += "__Coupure_Alim__";
			if(true == b_EtatCapteur1){
				S_MessageIDX +="Off";
			}else{
				S_MessageIDX +="On";
			}
}
/*		ElseIFcase "Software Watchdog":
			b_OldEtatCapteur1 = b_EtatCapteur1;
			ui32_DureeOuvert = 0;
			if(true == b_EtatCapteur1){
				S_MessageIDX +="Off";
			}else{
				S_MessageIDX +="On";
			}
			S_MessageLog += IDX1;
			S_MessageLog +="__W_A_T_C_H_D_O_G__"
*/			

yield();
i_RetourEtatFcts = ConfigWifiMonEsp();									//Paramétrage Wifi et initialisation connexion
if (i_RetourEtatFcts<0) GerErreurs(i_RetourEtatFcts);
HTTPMonEsp(&S_ReponseBrute,S_MessageIDX);						//Màj etat IDX
//HTTPMonEsp(&S_ReponseBrute,S_MessageIDX);						//Màj tension alim  ######### A Faire ############
S_MessageLog.replace(" ", "");
if (S_MessageLog != "command&param=addlogmessage&message=") HTTPMonEsp(&S_ReponseBrute,S_MessageLog);						//Ecriture message LOG


//Ecrire b_OldEtatCapteur1 et ui32_DureeOuvert dans la mémoire RTC
WriteBoolMemRtc(1,b_OldEtatCapteur1);
ESP.rtcUserMemoryWrite(32 + 1, (uint32_t*) &ui32_DureeOuvert, sizeof(ui32_DureeOuvert));

//Dodo ^_^
ESP.deepSleep((uint32_t)DEEPSLEEP_DURATION,WAKE_NO_RFCAL);
//### Réflechire au différent mode de Wake ... en fonction des endroits ou on s'endore
}

void loop(void){//__________________________LOOP________________________________
}

void Notification(uint32_t* NbCycles){
	tone(14,500);
	delay(1000);
	tone(14,1000);
	delay(1000);
	noTone(14);
	//######### A Faire ############
	//Il faut envoyer une notification a Domoticz
	//Ou mieux traiter le critère dans domoticz et monitorer une variable dans Domoticz pour déclencher le buzzer (... peut consommer plus de batteries)
}

//Fonction pour lire et écrire un Booleen dans la mémoire RTC à la position offset + 32
//Data stored in the first 32 blocks will be lost after performing an OTA update
//Offset is measured in blocks of 4 bytes and can range from 0 to 127 blocks.
//Data should be 4-byte aligned.
void ReadBoolMemRtc(char Offset, boolean ValBinaire ){
	uint32_t ValeurMemoire=0;
	if(Offset>(127-32)) GerErreurs(-32760);
	ESP.rtcUserMemoryRead(32 + Offset, (uint32_t*) &ValeurMemoire, sizeof(ValeurMemoire));
	ValBinaire = (boolean) ValeurMemoire;	
}
void WriteBoolMemRtc(char Offset, boolean ValBinaire ){
	uint32_t ValeurMemoire=0;
	if(Offset>(127-32)) GerErreurs(-32760);
	ValeurMemoire = (uint32_t) ValBinaire;
	ESP.rtcUserMemoryWrite(32 + Offset, (uint32_t*) &ValeurMemoire, sizeof(ValeurMemoire));
}


/*  Rappel pour utiliser la fonction parse de JSON
HTTPMonEsp(&S_ReponseBrute,EtatIDX);
EtatIDX = ParseJson(&S_ReponseBrute,String(IDX_PORTE),"Data");	// String ParseJson(String MessageHHTP, String IDX_Cherché, String Champ recherché)
*/
