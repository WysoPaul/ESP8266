/*
Paul le 15/04/2018
HTTPMonEsp.h

Librairie pour traiter les échanges HTTP avec Domotics et parser les réponses JSON

Le fichier .cpp contient le code source de ma librairie
Le fichier .h est le fichier d'entête (pour décalrer les prototypes ou d'autre define ou cte)
Pour rappel: a la compiltation "#include <ID-PSW.h>" est remplacé par le contenu de ID-PSW.h, le fichier ".ccp" doit être rajouté au projet (via IDE ou commande make)
Le fichier "keywords.txt" est dédié à l'aide contexctuel de l'IDE d'Arduino
*/

#ifndef HTTPMonEsp_h
#define HTTPMonEsp_h
String ParseJson(String *, String, String);
int HTTPMonEsp();
#endif