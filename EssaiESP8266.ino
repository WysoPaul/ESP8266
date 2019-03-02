#include <ESP8266WiFi.h>
ADC_MODE(ADC_VCC);								//Pour activer la fonction lecture tension alimentation
//#include <ID-PSW.h>
#include "ID-PSW.h"	/* Inclus les #define de:
		NOMWIFI (SSID de mon réseau Wifi)
		MDPWIFI (Mot de passe de mon réseau WiFi)
		DOMOTICS (adresse IP sous format texte de mon serveur)
	et la déclaration de
		const uint16_t PORT
		IPAddress PASSERELLE
		IPAddress MaskNet
		IPAddress IPSTATIC				!!-- à vérifier lors de la compilation de     P R O D U C T I O N     --!!
*/
#include "GerErreurs.h"
#include "HTTPMonEsp.h"
#include "InitMonESPWifi.h"



//---------------------------
//Macro de compilation
//---------------------------
	//#define BAVARD
	//#define PWIDEBUG

#ifdef PWIDEBUG
	#ifndef BAVARD
		#define BAVARD
	#endif
#endif


//---------------------------
//Macro du code
//---------------------------
#define IOCAPTEUR 13					// GPIO relié à l'ILS qui détecte l'ouverture de la fenetre (HIGH = Fenetre ouverte / Aimant absent)
#define IOBUZZER 14						// GPIO relié au Buzzer
#define IDX1 20							// IDX define dans Domotics (### a mettre à jour ###)
#define DEEPSLEEP_DURATION 300000000	// En microseconds 300 000 000 = 5mn, 20 000 000 = 20Sec
#define DIVDUREE 5						// DEEPSLEEP_DURATION/DIVDUREE qd la fenêtre est et etait ouverte
#define DUREE_OUVERT_MAX 5				// Nombre de cycles de DeepSLeep avec capteur actif qui entrainent une action


//---------------------------
//Protoypes
//---------------------------
void Notification(char* NbCycles);
void ReadBoolMemRtc(char Offset, boolean ValBinaire );
void WriteBoolMemRtc(char Offset, boolean ValBinaire);
void AfficherMemRtc(); //Fonction pour le debug ...


void setup(){//_________________________SETUP__________________________________
//---------------------------
//Declaration des variables locales
//---------------------------
String S_ReponseBrute = "", S_EtatIDX = "", S_PrkReboot = "", S_MessageIDX = "", S_MessageLog = "";
int i_RetourEtatFcts=0, i_Alim=0;
char c_EtatCapteur1=0, c_OldEtatCapteur1=0, c_DureeOuvert=0;

//---------------------------
//Initialisations
//---------------------------
pinMode(IOCAPTEUR,INPUT_PULLUP);
pinMode(IOBUZZER,OUTPUT);
Serial.begin(115200);
S_PrkReboot = ESP.getResetReason();
i_Alim = ESP.getVcc();
c_EtatCapteur1 = digitalRead(IOCAPTEUR);
S_MessageIDX = "command&param=switchlight&idx=" + String(IDX1) + "&switchcmd=";
S_MessageLog = "command&param=addlogmessage&message=";
//&&&&&&&&& Il serait interessant de formellement/explicitement couper le modem/WIFI par défaut ... &&&&&&&&&


#ifdef PWIDEBUG 
	Serial.println("\n\n## ---- Variable avant lecture memoire -----");
	Serial.printf("## Old etat capteur: %d\n",c_OldEtatCapteur1);
	Serial.printf("## Nb Cycles: %d\n",c_DureeOuvert);
	Serial.println("## Lecture memoire ...");
#endif

yield();
ESP.rtcUserMemoryRead(1, (uint32_t*) &c_OldEtatCapteur1, sizeof(c_OldEtatCapteur1));
ESP.rtcUserMemoryRead(2, (uint32_t*) &c_DureeOuvert, sizeof(c_DureeOuvert));

#ifdef BAVARD
Serial.printf("\nRaison du boot: %s\n",S_PrkReboot.c_str());
//Serial.println(S_PrkReboot);
Serial.printf("Tension alim: %d\n",i_Alim);
Serial.printf("Etat capteur: %d,\n",c_EtatCapteur1);
Serial.printf("Old etat capteur: %d\n",c_OldEtatCapteur1);
Serial.printf("Nb Cycles: %d\n\n",c_DureeOuvert);
delay(3000);
#endif
yield();

//Choix actions
if(String("Deep-Sleep Wake") == S_PrkReboot){					//Actions in case of Deepsleep wake-up:
			
			if(0 == c_EtatCapteur1){							//La fenêtre est FERME (L'aimant est Present, ILS fermé sur GND)
				if(c_EtatCapteur1 == c_OldEtatCapteur1){
					#ifdef BAVARD
						Serial.println("La fenetre EST ET ETAIT fermee.\n Dodo ^_^");
					#endif
					ESP.deepSleep((uint32_t)DEEPSLEEP_DURATION,WAKE_NO_RFCAL);
					}
				else{
					#ifdef BAVARD
						Serial.println("On vient de FERMER la fenetre");
					#endif
					c_DureeOuvert=0;
					c_OldEtatCapteur1 = c_EtatCapteur1;
					S_MessageIDX +="Off";
				}
			
			}else{												//La fenetre est OUVERTE (l'aimant esst absent, ILS ouvert, entrée PULL UP)
				c_DureeOuvert += 1;								// !!! risque de saturer la variable: MAX 255 !!!
				ESP.rtcUserMemoryWrite(2, (uint32_t*) &c_DureeOuvert, sizeof(c_DureeOuvert));
				if (c_DureeOuvert > DUREE_OUVERT_MAX) Notification(&c_DureeOuvert);
				
				if(c_EtatCapteur1 == c_OldEtatCapteur1){
					#ifdef BAVARD
						Serial.println("La fenetre EST ET ETAIT ouverte.\n Dodo ^_^");
					#endif
					ESP.deepSleep((uint32_t)DEEPSLEEP_DURATION/DIVDUREE,WAKE_NO_RFCAL);
				}
				else{
					#ifdef BAVARD
						Serial.println("On vient d'OUVRIR la fenetre");
					#endif
					c_OldEtatCapteur1 = c_EtatCapteur1;
					S_MessageIDX +="On";
				}
			}
}else{															//In all others cases (dont 'External System'')
			c_OldEtatCapteur1 = c_EtatCapteur1;
			c_DureeOuvert = 0;
			S_MessageLog += IDX1;
			S_MessageLog += S_PrkReboot;
			//S_MessageLog += "__Coupure_Alim__";
			if(0 == c_EtatCapteur1){
				#ifdef BAVARD
					Serial.println("Reboot, fenêtre FERMEE");
				#endif
				S_MessageIDX +="Off";
			}else{
				#ifdef BAVARD
					Serial.println("Reboot, fenêtre OUVERTE");
				#endif
				S_MessageIDX +="On";
			}
}
/*		ElseIFcase "Software Watchdog":
			c_OldEtatCapteur1 = c_EtatCapteur1;
			c_DureeOuvert=0;
			if(0 == c_EtatCapteur1){
				S_MessageIDX +="Off";
			}else{
				S_MessageIDX +="On";
			}
			S_MessageLog += IDX1;
			S_MessageLog +="__W_A_T_C_H_D_O_G__"
*/			
#ifdef BAVARD
	Serial.printf("\nEnvois à Domotics\n");
	Serial.println(S_MessageIDX);
	Serial.println(S_MessageLog);
	Serial.printf("\n");
	delay(2000);
#endif

yield();
i_RetourEtatFcts = ConfigWifiMonEsp();							//Paramétrage Wifi et initialisation connexion
if (i_RetourEtatFcts<0) GerErreurs(i_RetourEtatFcts);
HTTPMonEsp(&S_ReponseBrute,S_MessageIDX);						//Màj de Domotics avec etat IDX
//HTTPMonEsp(&S_ReponseBrute,S_MessageIDX);						//Màj de Domotics avec tension alim &&&&&&&&& A Faire &&&&&&&&& (rajouter paramètre &battery=XXX (0-100, 255 = Nobatt)
S_MessageLog.replace(" ", "");
if (S_MessageLog != "command&param=addlogmessage&message=") HTTPMonEsp(&S_ReponseBrute,S_MessageLog);	//Ecriture message LOG qd necessaire

#ifdef BAVARD
	Serial.println("\nEcriture memoire:");
	Serial.printf("FYI: Etat Capteur: %d\n", c_EtatCapteur1);
	Serial.printf("Old etat capteur: %d\n",c_OldEtatCapteur1);
	Serial.printf("Nb Cycles: %d\n",c_DureeOuvert);
	Serial.println("Puis Dodo ^_^");
	delay(3000);
#endif

ESP.rtcUserMemoryWrite(1, (uint32_t*) &c_OldEtatCapteur1, sizeof(c_OldEtatCapteur1));
ESP.rtcUserMemoryWrite(2, (uint32_t*) &c_DureeOuvert, sizeof(c_DureeOuvert));

#ifdef PWIDEBUG
	Serial.println("\n## Relecture memoire");
	ESP.rtcUserMemoryRead(1, (uint32_t*) &c_OldEtatCapteur1, sizeof(c_OldEtatCapteur1));
	ESP.rtcUserMemoryRead(2, (uint32_t*) &c_DureeOuvert, sizeof(c_DureeOuvert));
	Serial.printf("## FYI: Etat Capteur: %d\n", c_EtatCapteur1);
	Serial.printf("## Old etat capteur: %d\n",c_OldEtatCapteur1);
	Serial.printf("## Nb Cycles: %d\n",c_DureeOuvert);
	delay(3000);
#endif

//Dodo ^_^
ESP.deepSleep((uint32_t)DEEPSLEEP_DURATION,WAKE_NO_RFCAL);
}

void loop(void){//__________________________LOOP________________________________
}

void Notification(char* NbCycles){
for(int i= 0;i<20;i++){
	tone(14,20);
	delay(300);
	noTone(14);
	tone(14,1000);
	delay(300);
}
delay(1000);
noTone(14);
//&&&&&&&&& A Faire &&&&&&&&&
//Il faut envoyer une notification a Domoticz
//Ou mieux traiter le critère dans domoticz et monitorer une variable dans Domoticz pour déclencher le buzzer (... peut consommer plus de batteries)
}


#ifdef PWIDEBUG
	void AfficherMemRtc(){
	char charVar;
	Serial.println("\n----------------- DUMP BRUT memoir RTC");
	for(int i = 1; i<127;i++){
		ESP.rtcUserMemoryRead(i, (uint32_t*)&charVar, sizeof(charVar));
		Serial.print(charVar,HEX);
	}

	Serial.println("\n----------------- DUMP formaté memoir RTC");
	for(int i = 1; i<127;i++){
		ESP.rtcUserMemoryRead(i, (uint32_t*)&charVar, sizeof(charVar));
		if(0 == charVar){
			Serial.print("00");
		}else{
			if(charVar<16){
				Serial.print("0");
				Serial.print(charVar,HEX);
			}else{
				Serial.print(charVar,HEX);
			}
		}
		if(0==(i%4)) Serial.printf("\n");
	}
	}
#endif

/*  Rappel pour utiliser la fonction parse de JSON
HTTPMonEsp(&S_ReponseBrute,EtatIDX);
EtatIDX = ParseJson(&S_ReponseBrute,String(IDX_PORTE),"Data");	// String ParseJson(String MessageHHTP, String IDX_Cherché, String Champ recherché)
*/
