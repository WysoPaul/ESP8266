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
  
//DEMMARRAGE
void setup(){//_________________________SETUP__________________________________
int Etat;
// Paramétrage des 4 IO dédié au stepper en mode output, dédié au stepper
pinMode(IN1,OUTPUT);
pinMode(IN2,OUTPUT);
pinMode(IN3,OUTPUT);
pinMode(IN4,OUTPUT);

Serial.begin(115200);
//Serial.setDebugOutput(true);	Affiche du debug sur la connexion Wifi ...
delay(2000);                // Une petite pause au démmarage pour que l'affichage se passe bien sur le terminal ...
SerialPrintParamMonEsp();	//Print sur serial des paramètres de mon ESP (aucune config)
Etat = ConfigWifiMonEsp();
if (Etat<0) GerErreurs(Etat);
}

void loop(void){//__________________________LOOP________________________________
//CONNECTION AU SERVEUR
Serial.printf("Tentative de connexion a %s\n",DOMOTICS);
WiFiClient client;    				// Cré un client wifi (je sais pas ce que ça veut dire ...)
//Toutes le variables pour le parse et la requète HTTP :-/
char status[32] = {0};				// C'est là qu'on va stoqué l'info que le serveur à bien compris la question #ToDo ça dois pouvoir être optimisé
char RepDomotics[2000]={0};			// Variable Char stockant la réponse HTTP du serveur
String ReponseBrute;				// Variable String récupérant RepDomotics ...   PAS OPTIMAL DU TOUT

String Etat24="24", Etat25="25";	// IDX 24= RetourEtatPortePoules, IDX 25= ConsignePortePoules


//Etat= HTTPMonEsp(); Je voulais créer une lib, mais je sais pas comment passer "l'instance client" à la fonction ... #ToDo
String Url;
String Requette="devices&rid=24";    //La requette vers Domoticz après "/json.htm?type="
unsigned long Timeout;

//CONNECTION DOMOTICS
if (client.connect(DOMOTICS,PORT)<0) {   //On réalise la connection au serveur:port, si elle est réussi on poursuit
      client.stop();
      GerErreurs(-10);
  }
Serial.printf("Le serveur est disponible :-) \n");

//ENVOIS COMMANDE HTTP
//Constitution de la requette. Si j'utilise pas la fonction String("") j'ai une erreur de compilation ...
Url= String("GET /json.htm?type=") + Requette + " HTTP/1.1\r\n" +
"Host: " + DOMOTICS + "\r\n" +					//#TODO je devrais pouvoir remplacer l'adresse IP par la constante DOMOTICS
"Connection: Close\r\n" +
"\r\n";

Serial.printf("\nEnvois a \"%s\" la requette:\n",DOMOTICS);
Serial.println(Url);                                //#TODO Je voulais fusionner ces deux lignes (printf et println) en utilisant %c ou %s mais j'ai des erreurs de compilation => ????
client.print(Url);

//REPONSE SERVEUR
Timeout = millis();
while (client.available() == 0) {
	yield();
	if (millis() - Timeout > 5000) {		//Timeout 5sec venant d'un code du web ...
		client.stop();
		GerErreurs(-20);
    }
}
// Check HTTP status
client.readBytesUntil('\r', status, sizeof(status));
if (strcmp(status, "HTTP/1.1 200 OK") != 0)
	GerErreurs(-30);
// Skip HTTP headers
								//char endOfHeaders[] = "\r\n\r\n";
if (!client.find("\r\n\r\n"))	//(!client.find(endOfHeaders))
	GerErreurs(-30);
Serial.printf("Le serveur a repondu :-)\n");
	yield();
//PARSSAGE DE LA REPONSE
client.readBytesUntil('\r',RepDomotics,sizeof(RepDomotics)); //A noter qu'il n'y a jamais de \r, ça doit s'arreter au timeout ...
//Serial.print(RepDomotics);
/*
//					Pour tester 
client.stop();
delay(30000);
	yield();
	//CONNECTION DOMOTICS
if (client.connect(DOMOTICS,PORT)<0) {   //On réalise la connection au serveur:port, si elle est réussi on poursuit
      client.stop();
      GerErreurs(-10);
  }
Serial.printf("Le serveur est disponible :-) \n");
Requette="devices&rid=25";
Url= String("GET /json.htm?type=") + Requette + " HTTP/1.1\r\n" +
"Host: " + DOMOTICS + "\r\n" +					//#TODO je devrais pouvoir remplacer l'adresse IP par la constante DOMOTICS
"Connection: Close\r\n" +
"\r\n";
	yield();
Serial.printf("\nEnvois la requette:\n");
client.print(Url);
Serial.printf("\nDEBUG----Requette envoyé!\n");
//REPONSE SERVEUR
Timeout = millis();
while (client.available() == 0) {
	yield();
	if (millis() - Timeout > 5000) {		//Timeout 5sec venant d'un code du web ...
		client.stop();
		GerErreurs(-20);
    }
}
// Check HTTP status
client.readBytesUntil('\r', status, sizeof(status));
if (strcmp(status, "HTTP/1.1 200 OK") != 0)
	GerErreurs(-30);
// Skip HTTP headers
								//char endOfHeaders[] = "\r\n\r\n";
if (!client.find("\r\n\r\n"))	//(!client.find(endOfHeaders))
	GerErreurs(-30);
Serial.printf("Le serveur a repondu :-)\n");

//PARSSAGE DE LA REPONSE
client.readBytesUntil('\r',RepDomotics,sizeof(RepDomotics)); //A noter qu'il n'y a jamais de \r, ça doit s'arreter au timeout ...
client.stop();
Serial.print(RepDomotics);
Etat25 = ParseJson(&ReponseBrute,String(Etat25),"Data"); //IDX 25= ConsignePortePoules
Serial.printf("\nDEBUG---- Valeur de Etat25: ");	//DEBUG##############################
Serial.println(Etat25);							//DEBUG##############################

//					Pour tester 






yield();
Serial.printf("\nDEBUG----Client connected: ");	//DEBUG##############################
Serial.println(client.connected());
Serial.printf("\nDEBUG----taille de client: ");	//DEBUG##############################
Serial.println(client.available());
Serial.printf("\nDEBUG----Flush du client\n");	//DEBUG##############################
client.flush();
Serial.printf("\nDEBUG----taille de client: ");	//DEBUG##############################
Serial.println(client.available() );
client.stop();
*/
ReponseBrute=String(RepDomotics);				//Et là c'est pas très optimal le fait de décalrer un char[2000] puis un string pour la même opération ...
Serial.printf("\nDEBUG----Appel de ParseJson");	//DEBUG##############################
Etat24 = ParseJson(&ReponseBrute,String(Etat24),"Data"); //IDX 24= RetourEtatPortePoules
Serial.printf("\nDEBUG---- Valeur de Etat24: ");	//DEBUG##############################
Serial.println(Etat24);							//DEBUG##############################
yield();
/*
// ********** Voilà, là on connait l'état de IDX24, maintenant il faut avoir celui IDX25, donc on refait une nouvelle requète ***********
// ********** Pour le moment je fais un brutal copié collé du code ******
//-----------------------------------------------------------------------
Serial.printf("\nDEBUG----Clear status\n");	//DEBUG##############################
status[32]={0};
Serial.printf("\nDEBUG----Status= %c\n",status);
Serial.printf("\nDEBUG----Clear RepDomotics\n");	//DEBUG##############################
RepDomotics[32]={0};
Serial.printf("\nDEBUG----RepDomotics= %c\n",RepDomotics);
Serial.printf("\nDEBUG----Clear ReponseBrute\n");	//DEBUG##############################
ReponseBrute="";
Serial.printf("\nDEBUG----RepBrute= ");
Serial.println(ReponseBrute);

Serial.printf("\nDEBUG----Client connected: ");	//DEBUG##############################
Serial.println(client.connected());
Serial.printf("\nDEBUG----taille de client: ");	//DEBUG##############################
Serial.println(client.available() );
Serial.printf("\nDEBUG----Flush du client\n");	//DEBUG##############################
client.flush();
Serial.printf("\nDEBUG----taille de client: ");	//DEBUG##############################
Serial.println(client.available() );
Serial.printf("\nDEBUG---- ");
Serial.printf("Connexion etablie au reseau %s\nRouteur %s\nForce du signal: %d dBm\n", WiFi.SSID().c_str(),WiFi.BSSIDstr().c_str(),WiFi.RSSI());
Serial.printf("\nDEBUG---- ");
Serial.printf("Alias: %s\nAdress IP: %s\nNetMask: %s\nPasserelle: %s\n",WiFi.hostname().c_str(),WiFi.localIP().toString().c_str(),WiFi.subnetMask().toString().c_str(),WiFi.gatewayIP().toString().c_str());

Serial.printf("\nDEBUG---- Pause de 5sec et lancement connection\n");	//DEBUG##############################
*/
delay(30000);
//CONNECTION DOMOTICS
if (client.connect(DOMOTICS,PORT)<0) {   //On réalise la connection au serveur:port, si elle est réussi on poursuit
      client.stop();
      GerErreurs(-10);
  }
Serial.printf("Le serveur est disponible :-) \n");

//ENVOIS COMMANDE HTTP
//Constitution de la requette. Si j'utilise pas la fonction String("") j'ai une erreur de compilation ...
//------------------ MàJ de la requette --------------------------------- 
Requette="devices&rid=25";
Url= String("GET /json.htm?type=") + Requette + " HTTP/1.1\r\n" +
"Host: " + DOMOTICS + "\r\n" +					//#TODO je devrais pouvoir remplacer l'adresse IP par la constante DOMOTICS
"Connection: Close\r\n" +
"\r\n";

Serial.printf("\nEnvois a \"%s\" la requette:\n",DOMOTICS);
Serial.println(Url);                                //#TODO Je voulais fusionner ces deux lignes (printf et println) en utilisant %c ou %s mais j'ai des erreurs de compilation => ????
client.print(Url);
Serial.printf("\nDEBUG----Requette envoyé!\n");
//REPONSE SERVEUR
Timeout = millis();
while (client.available() == 0) {
	yield();
	if (millis() - Timeout > 5000) {		//Timeout 5sec venant d'un code du web ...
		client.stop();
		GerErreurs(-20);
    }
}
// Check HTTP status
client.readBytesUntil('\r', status, sizeof(status));
if (strcmp(status, "HTTP/1.1 200 OK") != 0)
	GerErreurs(-30);
// Skip HTTP headers
								//char endOfHeaders[] = "\r\n\r\n";
if (!client.find("\r\n\r\n"))	//(!client.find(endOfHeaders))
	GerErreurs(-30);
Serial.printf("Le serveur a repondu :-)\n");

//PARSSAGE DE LA REPONSE
client.readBytesUntil('\r',RepDomotics,sizeof(RepDomotics)); //A noter qu'il n'y a jamais de \r, ça doit s'arreter au timeout ...
client.stop();
ReponseBrute=String(RepDomotics);				//Et là c'est pas très optimal le fait de décalrer un char[2000] puis un string pour la même opération ...
//---------------------------------------------------------------------

Serial.printf("\nDEBUG----Appel de ParseJson");	//DEBUG##############################
Etat25 = ParseJson(&ReponseBrute,String(Etat25),"Data"); //IDX 25= ConsignePortePoules
Serial.printf("\nDEBUG---- Valeur de Etat25: ");	//DEBUG##############################
Serial.println(Etat25);							//DEBUG##############################




//ACTION MOTEUR CONDITIONNELLE
if (Etat24!=Etat25){
	if (Etat25=="On"){
		Serial.printf("Domoticz demande \"ON\" => Tourner moteur dans le sens horaire\n");
		TournerMoteur(true,4095,900);  
		Serial.printf("Fin de rotation moteur.\n");
	}else if (Etat25=="Off"){
		Serial.print("Domoticz demande \"OFF\" => Tourner moteur dans le sens ANTI-horaire\n");
		TournerMoteur(false,4095,900);
		Serial.printf("Fin de rotation moteur.\n");
	}else{
		GerErreurs(-100);
	}
}else{
	Serial.printf("Domoticz ne demande pas de changement.\nIDX24= %s\tIDX25=%s\n",Etat24.c_str(), Etat25.c_str());
}


//#ToDo Maintenant il faut rajouter la mise à jour IDX24, s'assurer que c'est bien enregistré, eventuellement écrire un message dans le Log de Domotics


// DODO
yield();		//C'est pour donner la main a la couche TCPIP avant de s'endormir.				
Serial.print("Dodo ^_^\n");
ESP.deepSleep(60000000,WAKE_RF_DEFAULT);	//le fameux mode deepsleep en µsec: 300 000 000 = 5mn
delay(60000);	// Pause de 60sec (60000msec), mais qui ne s'excute jamais lorsque le deepSleep est réalisé
}