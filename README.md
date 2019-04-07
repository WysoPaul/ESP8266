# ESP8266
L'ESP8266 pour interfacer avec ma domotique

!!! BROCHAGE DIFFERENT pour POULES et Maquette !!!!
Poules IO1=12, IO2=13, IO3=09, Io4=10
Maquette IO1 12, IO2 14, IO3 4, IO4 5

Paramètres à changer dans le StepperPWI.h
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


ToDo:
- Au boot vérifier la raison du Boot, si cause WatcheDog, faire un Sleep plus long et faire clignoter la LED
- Si c'est possibler d'identifier un Reset et/ou perte alim: dans le premier cas on peut suposer que l'utilisateur est là et donc lui permetre d'utiliser les switch diférement ...
- Modifier le code vérifiant que la commande a bien été prise en compte par Domotics (c'est un peu bourain)
- Rajouter le code pour faire clignoter la led
- Déplacer les configurations des pins dans un fichier paramètre (indépendaement du Code source)



