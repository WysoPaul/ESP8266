/*
Paul le 13/04/2018
InitMonESPWifi.h

Librairie qui contient tout la séquence de paramétrage par mes défaut de l'ESP
et la connexion au Wifi

Le fichier .cpp contient le code source de ma librairie
Le fichier .h est le fichier d'entête (pour décalrer les prototypes ou d'autre define ou cte)
Pour rappel: a la compiltation "#include <ID-PSW.h>" est remplacé par le contenu de ID-PSW.h, le fichier ".ccp" doit être rajouté au projet (via IDE ou commande make)
Le fichier "keywords.txt" est dédié à l'aide contexctuel de l'IDE d'Arduino
*/

#ifndef InitMonESPWifi_h
#define InitMonESPWifi_h
void SerialPrintParamMonEsp();

//ATTENTION CETTE fonction devrait dans le ''.ino''
// #ToDo: Pas réussi à mettre dans le .cpp, car pb de compilation pour accéder au constantes: NOMWIFI, MDPWIFI, ...
int ConfigWifiMonEsp(){
int ConnectTentative=0;
//PARAMETRAGE Wifi
Serial.printf("Debut du parametrage Wifi et connexion ");
WiFi.disconnect(false);       //OPTIONELLE: on efface précédement sauvegardé, dans le cache, MdP et SSID
Serial.printf(".");
WiFi.setAutoConnect(true);   //OPTIONELLE: Si vrai le système, au boot, tente de se reconnecté sur le dernier SSID utilisé
Serial.printf(".");
WiFi.setAutoReconnect(true);  //OPTIONELLE: Si la connexion wifi est perdu le système tente automatique de se reconnecter (en tache de fond)
Serial.printf(".");
WiFi.begin(NOMWIFI, MDPWIFI);     //Format WiFi.begin(ssid, password, channel, MAC_Routeur, Initiate_Connect_True-False)
Serial.printf(".");
WiFi.config(IPSTATIC,PASSERELLE,MaskNet); //Méthode pour désactiver le DHCP. Format: WiFi.config(local_ip, gateway, subnet, dns1, dns2)
Serial.printf(".");

#ifdef BAVARD
	//CONNEXION Wifi
	Serial.printf("\nTentative de connexion a %s\n",NOMWIFI);
	//On boucle tant qu'on a pas réussi à se connecter, et on affiche tt les 200msec l'état courant de la connection
#endif

while (WiFi.status() != WL_CONNECTED) {
	delay(200);
	#ifdef BAVARD
		Serial.printf("%d ",WiFi.status());
	#endif
	if (ConnectTentative>50){			//50x200msec = 10sec
		return -1;
	}else{
		ConnectTentative++;
	} 
}
#ifdef BAVARD
	Serial.printf("\nConnexion etablie au reseau %s\nRouteur %s\nForce du signal: %d dBm\n", WiFi.SSID().c_str(),WiFi.BSSIDstr().c_str(),WiFi.RSSI());
	Serial.printf("Alias: %s\nAdress IP: %s\nNetMask: %s\nPasserelle: %s\n",WiFi.hostname().c_str(),WiFi.localIP().toString().c_str(),WiFi.subnetMask().toString().c_str(),WiFi.gatewayIP().toString().c_str());
	Serial.printf("\n\nLes parametres de configuration sont:\n");
	Serial.printf("\tAuto Connexion au Boot au dernier SSID utlise:%d\n",WiFi.getAutoConnect());
	Serial.printf("\tAdress MAC de l'ESP: %s\n",WiFi.macAddress().c_str());
	Serial.printf("\n\n\t\t Fin de la configuration du Wifi ....\n");
	Serial.printf("-----------------------------------------------------------------------\n\n\n");
#endif
return 1;
}
#endif