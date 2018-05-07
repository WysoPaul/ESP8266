/*
Paul le 15/04/2018
GerErreurs.h

Librairie pour traiter les différentes problèmes / erreurs rencontré

Le fichier .cpp contient le code source de ma librairie
Le fichier .h est le fichier d'entête (pour décalrer les prototypes ou d'autre define ou cte)
Pour rappel: a la compiltation "#include <ID-PSW.h>" est remplacé par le contenu de ID-PSW.h, le fichier ".ccp" doit être rajouté au projet (via IDE ou commande make)
Le fichier "keywords.txt" est dédié à l'aide contexctuel de l'IDE d'Arduino
*/

#ifndef GerErreurs_h			// Compilation condtionnelle: si la lib a déjà ete inclus le compilo skip la définition (pour éviter qu'elle soit faite plusieurs fois)
#define GerErreurs_h
void GerErreurs(int CodeErr);
#endif