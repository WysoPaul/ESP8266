/*
Paul le 15/04/2018
GerErreurs.cpp

Librairie pour traiter les différentes problèmes / erreurs rencontré

Le fichier .cpp contient le code source de ma librairie
Le fichier .h est le fichier d'entête (pour décalrer les prototypes ou d'autre define ou cte)
Pour rappel: a la compiltation "#include <ID-PSW.h>" est remplacé par le contenu de ID-PSW.h, le fichier ".ccp" doit être rajouté au projet (via IDE ou commande make)
Le fichier "keywords.txt" est dédié à l'aide contexctuel de l'IDE d'Arduino
*/

#include <ESP8266WiFi.h>

void GerErreurs(int CodeErr){		//int Max= 32768
unsigned long DureeSommeille;	//Même si je stock au début des minutes, au moment du deepSleep le calcul *60*1 000 000 est tomporairement stocké dans la variable donc c'est un long
Serial.printf("\n!!!!!!!! ERREUR !!!!!!!!\n");
switch (CodeErr){
	case 0:		//Test d'appel de la fonction Gestion Erreur
		Serial.print("Appel de GerErreurs bien reussi\nRetour a la fonction d'appel\n");
		return;
		break;
	case -1:
		Serial.printf("La connection Wifi n'a pas pu etre etablie\n");
		DureeSommeille = 10;	//Minutes
		//On pourrait aussi rajouter le mode de réveille ...
		break;
	case -10:
		Serial.printf("Impossible de se connecteur au serveur\n");
		DureeSommeille = 10;
		break;
	case -20:
		Serial.printf("Le client prend trop de temps pour repondre\n");
		DureeSommeille = 10;
		break;
	case -30:
		Serial.printf("La reponse HTTP du client n'est pas conforme\n");
		DureeSommeille = 10;
		break;
	case -100:
		Serial.print("Reponse domoticz innatendu\n");	
		DureeSommeille = 10;
		//HTTPMonEsp(&ReponseBrute,"command&param=addlogmessage&message=!!!'Poule': Réponse Domotics Innatendu");	//ReponseBrute est pas déclarer #ToDo voir comment corrigé ça...
		break;
	case -200:
		Serial.print("La commande envoye n'a pas ete prise en compte par Domotics\n");	
		DureeSommeille = 10;
		//HTTPMonEsp(&ReponseBrute,"command&param=addlogmessage&message=!!!'Poule': Commande non pris en compte");	//ReponseBrute est pas déclarer #ToDo voir comment corrigé ça...
		break;
	case -1000:
		Serial.print("Problem au moment du parse\n");	
		DureeSommeille = 10;
		//HTTPMonEsp(&ReponseBrute,"command&param=addlogmessage&message=!!!'Poule': PB Parse JSON");					//ReponseBrute est pas déclarer #ToDo voir comment corrigé ça...
		break;	
	default:
		Serial.printf("!!! Erreur inconnue !!!\n");
		DureeSommeille = 10;
		//HTTPMonEsp(&ReponseBrute,"command&param=addlogmessage&message=!!!'Poule': Erreur inconnue");				//ReponseBrute est pas déclarer #ToDo voir comment corrigé ça...
		break;		
}
Serial.printf("LE SYSTEM VA PASSER EN MODE SOMEILLE POUR %dMN\nCode Erreur: ",DureeSommeille);		
Serial.println(CodeErr);
Serial.printf("\n!!!!!!!! ERREUR !!!!!!!!\n\n\n\n");
yield();		//C'est pour donner la main a la couche TCPIP avant de s'endormir.				
ESP.deepSleep(DureeSommeille*60*1000000,WAKE_RF_DEFAULT);	//l'ESP passe en mode someille profond pendant X µsec (exp 300 000 000 = 5mn) et reboot
}

/*Serial.printf("Taille de la pile: ");
Serial.println(ESP.getFreeHeap());
Serial.printf("Force du signal: %d dBm\n",WiFi.RSSI());
*/