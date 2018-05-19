/*
Paul le 13/04/2018

Librairie que j'ai créé pour piloter les moteurs stepper, car celle d'Arduino ne semblait pas fonctionner avec ESP8266.
Le code est issue du web: lien....

Le fichier .cpp contient le code source de ma librairie
Le fichier .h est le fichier d'entête (pour décalrer les prototypes ou d'autre define ou cte)
Pour rappel: a la compiltation "#include <ID-PSW.h>" est remplacé par le contenu de ID-PSW.h, le fichier ".ccp" doit être rajouté au projet (via IDE ou commande make)
Le fichier "keywords.txt" est dédié à l'aide contexctuel de l'IDE d'Arduino
> chaque ligne commence par le "mot_clef<TAB>KEYWORD1", si le mot-clef est une classe (cela le colorera en orange),
> ou KEYWORD2 si c'est une fonction (coloration en marron)
Enfin, un /s répertoir peut contenir des sketch d'exemple
*/

#include <ESP8266WiFi.h>
#ifndef StepperPWI_h
#define StepperPWI_h
//Déclaration des pins utilisé par le stepper
#define IN1 12
#define IN2 14
#define IN3 3
#define IN4 4

void TournerMoteur(boolean,int,int);	//Commenté car problème de compilation
void WriteStep(int OutArray[4]);		//Commenté car problème de compilation
#endif