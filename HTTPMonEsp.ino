/*
Paul le 15/04/2018
HTTPMonEsp.cpp

Librairie pour traiter les échanges HTTP avec Domotics et parser les réponses JSON

Le fichier .cpp contient le code source de ma librairie
Le fichier .h est le fichier d'entête (pour décalrer les prototypes ou d'autre define ou cte)
Pour rappel: a la compiltation "#include <ID-PSW.h>" est remplacé par le contenu de ID-PSW.h, le fichier ".ccp" doit être rajouté au projet (via IDE ou commande make)
Le fichier "keywords.txt" est dédié à l'aide contexctuel de l'IDE d'Arduino
*/

#include <ESP8266WiFi.h>
#include "GerErreurs.h"

//
//La fonction vérifie d'abord si la réponse correctpond à l'IDX demandé
//puis va cherché la valeur de "Champ" et la retour
//

String ParseJson(String *ChaineAParser, String Idx, String Champ){
int IndexDebut, IndexFin;
yield();
IndexDebut=ChaineAParser->indexOf("idx");
IndexDebut=ChaineAParser->indexOf("\"",IndexDebut+1);
IndexDebut=ChaineAParser->indexOf("\"",IndexDebut+1);
IndexFin=ChaineAParser->indexOf("\"",IndexDebut+1);
yield();
if (ChaineAParser->substring(IndexDebut+1,IndexFin)!=Idx)
	GerErreurs(-1000);
IndexDebut=ChaineAParser->indexOf(Champ);
IndexDebut=ChaineAParser->indexOf("\"",IndexDebut+1);
IndexDebut=ChaineAParser->indexOf("\"",IndexDebut+1);
IndexFin=ChaineAParser->indexOf("\"",IndexDebut+1);
yield();
return ChaineAParser->substring(IndexDebut+1,IndexFin);
}



void HTTPMonEsp(String *ReponseBrute, String Requette){
Serial.printf("\nPause de 10sec.\n");
delay(10000);

WiFiClient client;    				// Cré un client wifi (je sais pas ce que ça veut dire ...)
String Url;
unsigned long Timeout;
//Toutes le variables pour le parse et la requète HTTP :-/
char status[32] = {0};				// C'est là qu'on va stoqué l'info que le serveur à bien compris la question #ToDo ça dois pouvoir être optimisé
char RepDomotics[2000]={0};			// Variable Char stockant la réponse HTTP du serveur

//CONNECTION DOMOTICS
Serial.printf("Tentative de connexion a %s\n",DOMOTICS);
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

Serial.printf("\nEnvois a \"%s\" la requette: [...] ",DOMOTICS);
Serial.println(Requette);                                //#TODO Je voulais fusionner ces deux lignes (printf et println) en utilisant %c ou %s mais j'ai des erreurs de compilation => ????
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
if (!client.find("\r\n\r\n"))
	GerErreurs(-30);
Serial.printf("Le serveur a repondu :-)\n");

//LECTURE DU BUFFER HTTP
client.readBytesUntil('\r',RepDomotics,sizeof(RepDomotics)); //A noter qu'il n'y a jamais de \r, ça doit s'arreter au timeout ...
client.stop();
*ReponseBrute=String(RepDomotics);	
}
