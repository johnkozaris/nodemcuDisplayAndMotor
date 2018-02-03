/*Η Βιβλιοθήκη	αυτή έχει σκοπό να διευκολύνει την σύνδεση των συσκευών Esp8266
*με ένα τοπικό δίκτυο Wi-Fi, και με έναν Μεσίτη MQTT.
*Μετατρέπει τον Esp8266 se AcessPoint WebServer και δείνει την δυνατότητα στον
*χρήστη να καταχωρήσει τα απαραίτητα στοιχεία μέσω μιάς Web Φόρμας
*Οι Μεταβλητές αποθηκευονται στην ROM και μπορούν να διαβαστούν μέσω των
*μεθόδων EspNetworkManager.ReadSSID,ReadPASSWORD κτλπ
*
*Author: Kozaris Ioannis 2017
*/

#ifndef EspNetworkManager_h
#define EspNetworkManager_h
                              /*Απαραίτητες βιβλιοθήκες ESP8266*/
#include <EEPROM.h>
//#include <ESP8266WiFiType.h>
#include <ESP8266WebServer.h>
#include <Arduino.h>
#include <stdint.h>
#include <dummy.h>
#include <WiFiUdp.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFi.h>
#include <HardwareSerial.h>
extern "C" {
  #include "user_interface.h"
  #include "Esp.h"
}
                            /*Κώδικας HTML και Javascript*/
//Αποθηκευεται στην μήμη προγράμματος για να χρησιμοποιηθεί απο τον Esp WebServer
//Τα παρακάτω string περιέχουν 3 σελίδες HTML (Κεντρική,SSID,Ρυθμίσεων MQTT)

//Header που περιέχει τα css Styles
const char HTML_GENERAL_HEADER_STYLE[] PROGMEM = "<!DOCTYPE html><html><head><meta name=\"viewport \" content=\"width=device-width, initial-scale=1, user-scalable=no \"/><style>.myButton{display: inline-block;-webkit-box-sizing: content-box;-moz-box-sizing: content-box;box-sizing: content-box;cursor: pointer;padding: 10px 20px;border: 1px solid #018dc4;-webkit-border-radius: 3px;border-radius: 3px;font: normal medium/normal Arial, Helvetica, sans-serif;color: rgba(255,255,255,0.9);-o-text-overflow: clip;text-overflow: clip;background: #0199d9;-webkit-box-shadow: 2px 2px 2px 0 rgba(0,0,0,0.2) ;box-shadow: 2px 2px 2px 0 rgba(0,0,0,0.2) ;text-shadow: -1px -1px 0 rgba(15,73,168,0.66) ;-webkit-transition: all 300ms cubic-bezier(0.42, 0, 0.58, 1);-moz-transition: all 300ms cubic-bezier(0.42, 0, 0.58, 1);-o-transition: all 300ms cubic-bezier(0.42, 0, 0.58, 1);transition: all 300ms cubic-bezier(0.42, 0, 0.58, 1);}.myButton:hover{border: 1px solid #007cad;background: rgba(0,142,198,1);-webkit-box-shadow: 2px 2px 2px 0 rgba(0,0,0,0.3) ;box-shadow: 2px 2px 2px 0 rgba(0,0,0,0.3) ;-webkit-transition: all 200ms cubic-bezier(0.42, 0, 0.58, 1);-moz-transition: all 200ms cubic-bezier(0.42, 0, 0.58, 1);-o-transition: all 200ms cubic-bezier(0.42, 0, 0.58, 1);transition: all 200ms cubic-bezier(0.42, 0, 0.58, 1);}.myButton:active{border: 1px solid #018dc4;background: #00a6e8;-webkit-box-shadow: 0 1px 4px 0 rgba(0,0,0,0.2) inset;box-shadow: 0 1px 4px 0 rgba(0,0,0,0.2) inset;text-shadow: none;-webkit-transition: all 50ms cubic-bezier(0.42, 0, 0.58, 1);-moz-transition: all 50ms cubic-bezier(0.42, 0, 0.58, 1);-o-transition: all 50ms cubic-bezier(0.42, 0, 0.58, 1);transition: all 50ms cubic-bezier(0.42, 0, 0.58, 1);}.myInput{display: inline-block;-webkit-box-sizing: content-box;-moz-box-sizing: content-box;box-sizing: content-box;padding: 6px 20px;border: none;-webkit-border-radius: 32px / 54px;border-radius: 32px / 54px;font: normal 18px/normal \"Coda\", Helvetica, sans-serif;color: rgba(0,142,198,1);-o-text-overflow: ellipsis;text-overflow: ellipsis;background: rgba(252,252,252,1);-webkit-box-shadow: 0 1px 2px 0 rgba(0,0,0,0.2) inset;box-shadow: 0 1px 2px 0 rgba(0,0,0,0.2) inset;text-shadow: 1px 1px 0 rgba(255,255,255,0.66) ;-webkit-transition: all 200ms cubic-bezier(0.42, 0, 0.58, 1);-moz-transition: all 200ms cubic-bezier(0.42, 0, 0.58, 1);-o-transition: all 200ms cubic-bezier(0.42, 0, 0.58, 1);transition: all 200ms cubic-bezier(0.42, 0, 0.58, 1);}.myInput:hover{background: rgba(232,249,255,1);-webkit-box-shadow: 0 2px 2px 0 rgba(90,90,90,0.2) inset;box-shadow: 0 2px 2px 0 rgba(90,90,90,0.2) inset;}.myInput:focus{background: rgba(255,253,232,1);}.listItem{-webkit-box-sizing: content-box;-moz-box-sizing: content-box;box-sizing: content-box;width: 106px;cursor: pointer;padding: 5px 15px;border: none;-webkit-border-radius: 26px;border-radius: 26px;font: normal 13px/normal \"times new roman\", times, serif;color: rgb(255, 255, 255);text-align: center;-o-text-overflow: clip;text-overflow: clip;background: rgb(20, 128, 68);}</style>";
//Javascript σελίδας ssid
const char HTML_SSID_SCRIPT[] PROGMEM = "<script type='text/javascript'>window.onload=function (){document.getElementById(\"btnconnect\").onclick=conn; var list=document.getElementById(\"list\");if (list){var links=list.getElementsByTagName(\"a\"), i;for (i=0; i < links.length; i++){links[i].onclick=function (){document.getElementById(\"txtssid\").value=this.id.toString(); window.scrollTo(0,0);return false;};}}function conn(){var ssidb=document.getElementById(\"txtssid\");var passb=document.getElementById(\"txtpass\");if (ssidb.value===''){alert(\"No SSID: Choose From list or input Manually\");return false;}else{var url='?' + 'ssid=' + encodeURIComponent(ssidb.value) + '&password=' + encodeURIComponent(passb.value);window.location.href=url;if (passb.value===''){alert(\"Saved. Esp8266 is Restarting\");}else{alert(\"Saved. Esp8266 is Restarting\");}return false;}}}</script></head>";
//HTML κώδικας σελίδας ssid
const char HTML_SSID_BODY_1[] PROGMEM = "<body><h1 style=\"text-align: center;\">&Epsilon;&pi;&iota;&lambda;&omicron;&gamma;&eta;&nbsp;&Delta;&iota;&kappa;&tau;&upsilon;&omicron;&upsilon; Wifi</h1><hr/><p style=\"text-align: center; line-height: 3;\"><input class=\"myInput\" id=\"txtssid\" placeholder=\"SSID\" type=\"text\"/> &nbsp;<input class=\"myInput\" id=\"txtpass\" placeholder=\"Password\" type=\"text\"/><br/><a class=\"myButton\" id=\"btnconnect\" >&Sigma;&upsilon;&nu;&delta;&epsilon;&sigma;&eta;</a></p><hr/><p style=\"text-align: center;\">&Delta;&iota;&alpha;&theta;&epsilon;&sigma;&iota;&mu;&alpha; &Delta;&iota;&kappa;&tau;&upsilon;&alpha;</p><ul id=\"list\"style=\"line-height: 2;\">";
//Javascript Αρχικής σελίδας
const char HTML_MAIN_SCRIPT[] PROGMEM = "<script>window.onload=function (){document.getElementById(\"btnwifi\").onclick=clink; document.getElementById(\"btnopt\").onclick=clink2; function clink(){window.location.href='/wifi'; return false;}function clink2(){window.location.href='/options'; return false;}}</script></head>";
//HTML κώδικας Αρχικής σελίδας μέρος 1
const char HTML_MAIN_BODY_1[] PROGMEM = "<body><p style=\"text-align: center;\"><img alt=\"embedded pns icon\" height=\"50\" src=\"data:image/gif;base64,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\" width=\"214\"/></p><hr/><h2 style=\"text-align: center;\">&Rho;&upsilon;&theta;&mu;&iota;&sigma;&epsilon;&iota;&sigmaf; &Sigma;&upsilon;&sigma;&kappa;&epsilon;&upsilon;&eta;&sigmaf;</h2><p style=\"text-align: center;\"><span style=\"color:rgb(165, 48, 33);\"><em><strong>ID: ESP";
//HTML κώδικας Αρχικής σελίδας μέρος 2
const char HTML_MAIN_BODY_2[] PROGMEM = "&nbsp;</strong></em></span></p><p style=\"text-align: center; line-height: 3;\"><a class=\"myButton\" href=\"/wifi\" id=\"btnwifi\"style=\"text-decoration: none\">&Epsilon;&pi;&iota;&lambda;&omicron;&gamma;&eta; &Delta;&iota;&kappa;&tau;&upsilon;&omicron;&upsilon; WiFi</a>&nbsp;<a class=\"myButton\" href=\"/options\" id=\"btnopt\"style=\"text-decoration: none\">&Rho;&upsilon;&theta;&mu;&iota;&sigma;&epsilon;&iota;&sigmaf; MQTT</a></p>";
//Javascript σελίδας ρυθμίσεων MQTT
const char HTML_OPTIONS_SCRIPT_N_BODY[] PROGMEM = "<script type='text/javascript'>window.onload=function (){document.getElementById(\"refsave\").onclick=flink;function flink(){var desc=document.getElementById(\"txtdesc\").value;var brokeraddr=document.getElementById(\"txtbrokeraddr\").value;var brokerUsername=document.getElementById(\"txtbrokerusername\").value;var brokerPassword=document.getElementById(\"txtbrokerpass\").value;var url='?' + 'txtdesc=' + encodeURIComponent(desc) + '&txtbrokeraddr=' + encodeURIComponent(brokeraddr)+ '&txtbrokerusername=' + encodeURIComponent(brokerUsername)+ '&txtbrokerpass=' + encodeURIComponent(brokerPassword);alert(\"Values saved in ROM\");window.location.href=url;return false;}}</script></head><body><h1 style=\"text-align: center;\">&Rho;&upsilon;&theta;&mu;&iota;&sigma;&epsilon;&iota;&sigmaf; MQTT</h1><hr/><p style=\"text-align: center;\">&Omicron;&nu;&omicron;&mu;&alpha; &Sigma;&upsilon;&sigma;&kappa;&epsilon;&upsilon;&eta;&sigmaf;: &nbsp;<input id=\"txtdesc\" maxlength=\"20\" type=\"text\" placeholder=\"Name\" class=\"myInput\"/>&nbsp; &nbsp;</p><hr/><p style=\"text-align: center;\">&Delta;&iota;&epsilon;&upsilon;&theta;&upsilon;&nu;&sigma;&eta; &Mu;&epsilon;&sigma;&iota;&tau;&eta;: &nbsp;<input id=\"txtbrokeraddr\" maxlength=\"25\" type=\"text\" class=\"myInput\" placeholder=\"192.168.1.1 \"/>&nbsp; &nbsp;</p><p style=\"text-align: center;\"><input id=\"txtbrokerusername\" maxlength=\"15\" type=\"text\" class=\"myInput\" placeholder=\"Username &Mu;&epsilon;&sigma;&iota;&tau;&eta;\"/>&nbsp; &nbsp;<input id=\"txtbrokerpass\" maxlength=\"12\" type=\"text\" class=\"myInput\" placeholder=\"Password &Mu;&epsilon;&sigma;&iota;&tau;&eta;\"/>&nbsp; &nbsp;</p><hr/><p style=\"text-align: center;\"><a class=\"myButton\" id=\"refsave\">&Alpha;&pi;&omicron;&theta;&eta;&kappa;&epsilon;&upsilon;&sigma;&eta;</a>&nbsp;</p>";
//Τελικος HTML κώδικας(κοινος για ολες τις σελιδες)
const char HTML_END_SIGN[] PROGMEM = "<hr/><h3 style=\"color: #aaa; font-style: italic;\"><span style=\"font-size: 14px;\"><span style=\"color: #696969;\"><tt><span style=\"font-family: times new roman, times, serif;\"><em>Created by: </em></span></tt></span><span style=\"color: #000000;\"><tt><span style=\"font-family: times new roman, times, serif;\"><em><strong>Kozaris Ioannis</strong></em></span></tt></span><span style=\"color: #008080;\"><tt><span style=\"font-family: times new roman, times, serif;\"><em> </em></span></tt></span><span style=\"color: #696969;\"><tt><span style=\"font-family: times new roman, times, serif;\"><em>(2016)</em></span></tt></span></span></h3></body></html>";


class EspNetworkManager{          /* Βιβλιοθήκη EspNetworkManager */
public://Δημόσιες μέθοδοι
EspNetworkManager(String chipID);//Μέθοδος αρχικοποίησης αντικειμένου EspNetworkManager

void begin();//Πρεπει να εκτελεστεί πρώτη για την αρχικοποίηση των απαιρετητων μεταβλητω
boolean runManager();//Επιστρέφει True αν η κατάσταση Ρυθμισης εκτελείτε κανονικά,False αν η εκτέλεση αποτύχει
String ReadSSID();//Διαβασμα του αποθηκευμενου SSID απο την ROM
String ReadPASSWORD();//Διαβασμα του αποθηκευμενου κωδικου δικτυου απο την ROM
String ReadDescription();//Διαβασμα της αποθηκευμενης Περιγραφής Συσκευής απο την ROM
String ReadBrokerAddress();//Διαβασμα της αποθηκευμενης Διευθυνσης Μεσίτη απο την ROM
String ReadBrokerUsername();//Διαβασμα του αποθηκευμενου Ονοματος χρήστη Μεσίτη απο την ROM
String ReadBrokerPassword();//Διαβασμα του αποθηκευμενου Κωδικού Μεσίτη απο την ROM

private://Μεταβλητές-Μέθοδοι για εσωτερική κατανάλωση
ESP8266WebServer server;//Αντικείμενο που δημιουργει εναν WebServer
String moduleID ="";//Αναγνωρηστικο συσκευής
const char* ssid = "ESP";//Ονομα δικτύου που δημιουργεί ο  ESP8266
IPAddress local_IP;//IP δικτύου που δημιουργεί ο  ESP8266
IPAddress gateway;//Gateway δικτύου που δημιουργεί ο  ESP8266
IPAddress subnet;//Subnet Mask δικτύου που δημιουργεί ο  ESP8266

String content;//Περιεχόμνεο προς προβολή μέσω του ESP8266WebServer(HTML σελίδες)
String st;

String BrokerAddress;//Διευθυνση μεσίτης που καταχωρήθηκε απο τον χρήστη
String Module_Description;//Περιγραφή συσκευής που καταχωρήθηκε απο τον χρήστη
String BrokerUsername;//Ονομα χρήστη Μεσίτη που καταχωρήθηκε απο τον χρήστη
String BrokerPassword;//Κωδικός Μεσίτη που καταχωρήθηκε απο τον χρήστη
String WIFI_SSID;//SSID δικτύου Wi-Fi που καταχωρήθηκε απο τον χρήστη
String WIFI_PASS;//Κωδικός δικτύου Wi-Fi  που καταχωρήθηκε απο τον χρήστη

boolean managerFinished = false;//True η καταχώρηση στοιχείων ολοκληρώθηκε,False(προεπιλογή)

void initializeAP();//Ρυθμιση του Esp8266 ως AccessPoint
void initializeWebServer();//Ρυθμιση του Esp8266 ως WebServer
void WriteEeprom(int);//Εγγραφή στην ROM(int=Αναγωριστικό ανάγνωσης:1-6)
//1=Θέση Wi-Fi SSID
//2=Θέση Wi-Fi Κωδικός
//3=Θέση Περιγραφή
//4=Θέση Διευθυνση Μεσίτη
//5=Θέση Ονομα Χρήστη μεσίτη
//6=Θέση Κωδικος Πρόσβασης Μεσίτη
void ClearEeprom(int);//Εκαθάριση ROM
String ReadEeprom(int);//Διαβασμα ROM

};
#endif
