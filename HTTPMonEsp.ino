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



int HTTPMonEsp(){
/*
String Url;
String Line;
String NomIdx;
String Requette="devices&rid=24";    //La requette vers Domoticz pour demaner l'état du switch IDX 24
unsigned long Timeout;

//CONNECTION AU SERVEUR
if (!client.connect(DOMOTICS,PORT)) {   //On réalise la connection au serveur:port, si elle est réussi on poursuit
      client.stop();
      return -10;   // On quite la fonction avec une erreur
  }
Serial.printf("Le serveur est disponible :-) \n");

//ENVOIS de la commande HTTP
//Constitution de la requette. Si j'utilise pas la fonction String("") j'ai une erreur de compilation ...
Url= String("GET /json.htm?type=") + Requette + " HTTP/1.1\r\n" +
"Host: " + DOMOTICS + "\r\n" +					//#TODO je devrais pouvoir remplacer l'adresse IP par la constante DOMOTICS
"Connection: Close\r\n" +
"\r\n";

Serial.printf("\nEnvois a \"%s\" la requette:\n",DOMOTICS);
Serial.println(Url);                                //#TODO Je voulais fusionner ces deux lignes (printf et println) en utilisant %c ou %s mais j'ai des erreurs de compilation => ????
client.print(Url);

//On attend la réponse du serveur
Timeout = millis();
while (client.available() == 0) {
	yield();
	if (millis() - Timeout > 5000) {		//Timeout 5sec venant d'un code du web ...
		client.stop();
		return -20;
    }
}
Serial.printf("Le serveur a repondu :-)\n");

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
    return -30;
*/
}
