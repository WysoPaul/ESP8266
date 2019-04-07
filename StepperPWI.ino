/*
Paul le 13/04/2018
StepperPWI.cpp

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
//NOTE avec mon mmoteur il faut 4095 steps pour faire un tour
//NOTE Un vitesse de rotation (STEPTIME) de 900 (µsec) entre deux steps marche bien

#include <ESP8266WiFi.h>
#include "StepperPWI.h"

int ArrayDefault[4] = {LOW, LOW, LOW, LOW};
int StepsMatrix[8][4] = {
	{LOW, LOW, LOW, HIGH},
	{LOW, LOW, HIGH, HIGH},
	{LOW, LOW, HIGH, LOW},
	{LOW, HIGH, HIGH, LOW},
	{LOW, HIGH, LOW, LOW},
	{HIGH, HIGH, LOW, LOW},
	{HIGH, LOW, LOW, LOW},
	{HIGH, LOW, LOW, HIGH},
};
int Step = 0;
unsigned long LastTime = 0;
unsigned long Temps = 0;
unsigned long CurrentMicros;
int StepsLeft;

void TournerMoteur(boolean Cw,int NbSteps, int Speed){
StepsLeft = NbSteps;
LastTime = micros();
Temps=0;
while (StepsLeft > 0) {
	CurrentMicros = micros();
	if (CurrentMicros - LastTime >= Speed) {
		if ((Step >= 0) && (Step < 8)) {
		WriteStep(StepsMatrix[Step]);
		} else {
		WriteStep(ArrayDefault);
		}
		(Cw == true) ? (Step++) : (Step--);		//#ToDo à écrire en mode standard if(...) ...
		if (Step > 7) {
			Step = 0;
		} else if (Step < 0) {
			Step = 7;
		}
		Temps += micros() - LastTime;
		LastTime = micros();
		StepsLeft--;
    }
    yield();
  }
WriteStep(ArrayDefault);
}

void WriteStep(int OutArray[4]) {
  digitalWrite(IO1, OutArray[0]);
  digitalWrite(IO2, OutArray[1]);
  digitalWrite(IO3, OutArray[2]);
  digitalWrite(IO4, OutArray[3]);
}