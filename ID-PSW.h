#define NOMWIFI "Alice_Box" 	//(SSID de mon réseau Wifi)
#define MDPWIFI "evadenis"		//(Mot de passe de mon réseau WiFi)
#define DOMOTICS "192.168.1.27"	//(adresse IP sous format texte de mon serveur)
const uint16_t PORT = 8080;
IPAddress PASSERELLE(192,168,1,27);
IPAddress MaskNet(255,255,0,0);
IPAddress IPSTATIC(192,168,1,120);