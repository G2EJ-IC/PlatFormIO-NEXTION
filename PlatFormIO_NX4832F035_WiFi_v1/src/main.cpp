//*************************************************************************************************//
//*************************************************************************************************//
// Universidad Distrital Francisco José de Caldas
// Facultad Tecnológica | Tecnología en Construcciones Civiles
// Maestria en Ingenieria Civil | 2023-I | Ernesto J. Guerrero G. | ejguerrerog@udistrital.edu.co
// Monitoreo AIoT
// Microcontroladores.: NodeMCU ESP32
//*************************************************************************************************//
//*************************************************************************************************//
#include <Arduino.h>
#define PinLED 2

String endChar = String (char (0xff)) + String (char (0xff)) + String (char (0xff));
String dfd = ""; // data fron display

unsigned long asyncDelay = 0;
int delayLength = 10000;

#include <WiFi.h>
String SSIDname = "NoName";
String SSIDpassword = "NoPassword";
int SSIDcontador = 0;

//*************************************************************************************************//
void inputFunction(void);
void delayFunction(void);
void WiFi_Init(void);
void Function_GET(void);
void Function_CON(String);
void Function_NUM(String);
//*************************************************************************************************//

void setup() {
  Serial.begin (115200);
  Serial2.begin (115200);
  pinMode (PinLED, OUTPUT);
}

void loop() {
  // NOTE : COLLECT CHARACTERS FROM NEXTION DISPLAY
  // NOTA : RECOGE CARACTERES DE LA PANTALLA NEXTION
  if (Serial2.available()) {
    inputFunction();
  }
  // NOTE : ASYNC DELAY
  // NOTA : RETARDO ASÍNCRO
  if(millis() > asyncDelay) {
    asyncDelay+=delayLength;
    delayFunction();
  }
  //NOTE : SOMETHING SENT FROM NEXTION I.E. GET REQEVEST
  //NOTA : ALGO ENVIADO DESDE NEXTION, ES DECIR. OBTENER SOLICITUD
  if(dfd.endsWith(endChar)) {
    Serial.println(dfd);
    Serial.println("error");
    dfd = ""; // data fron display
  }
}

//*************************************************************************************************//
//*************************************************************************************************//
void inputFunction(void) {
  dfd += char(Serial2.read()); // data fron display
  if(dfd.length() > 3 && dfd.substring(0,3) != "C:C") {
    dfd = ""; // data fron display
  }
  else {
    if(dfd.substring((dfd.length()-1),dfd.length()) == "?") {
      String command = dfd.substring(3,6);
      String value = dfd.substring(6,dfd.length() - 1);
      // NOTE: FOR TESTING
      Serial.println(command + " : " + value);
      // NOTE: TEXT COMMAND GET
      // NOTE: TEXT COMMAND CON
      // NOTE: TEXT COMMAND NUM
      if(command == "GET") Function_GET();
      if(command == "CON") Function_CON(value);
      if(command == "NUM") Function_NUM(value);
      dfd = "";
    }
  } 
}

void delayFunction(void) {
  digitalWrite(PinLED, !digitalRead(PinLED));
  // NOTE : STUFF HERE
  Serial.println("==============================================================================");
  Serial.println(SSIDname + " : " + SSIDpassword);
  Serial.println("Conectado a la red WiFi.: " + WiFi.SSID());
  Serial.println("WiFi STATUS    .: " + String(WiFi.status()));
  // 266:	  WL_NO_SHIELD
  // 0:	  	WL_IDLE_STATUS 
  // 1:	  	WL_NO_SSID_AVAIL
  // 2:	  	WL_SCAN_COMPLETED
  // 3:	  	WL_CONNECTED
  // 4:	  	WL_CONNECT_FAILED
  // 5:   	WL_CONNECTION_LOST
  // 6:	  	WL_DISCONNECTED
  // if(WiFi.status == something)
  Serial.println("Dirección IP   .: " + WiFi.localIP().toString());
  Serial.println("Dirección DNS  .: " + WiFi.dnsIP().toString());
  Serial.println("Dirección MAC  .: " + WiFi.macAddress());
  if(WiFi.status() == 3) {
    // Page 1
    Serial2.print("page1.r0.pco=1024" + endChar);
    Serial2.print("page1.r0.bco=1024" + endChar);
    // Page 2
    Serial2.print("page2.r0.pco=1024" + endChar);
    Serial2.print("page2.r0.bco=1024" + endChar);
    // SSID, IP, DNS, MAC
    Serial2.print("page2.t4.txt=\"" + WiFi.SSID() + "\"" + endChar); // SSID.
    Serial2.print("page2.t5.txt=\"" + WiFi.localIP().toString() + "\"" + endChar); // IP.
    Serial2.print("page2.t6.txt=\"" + WiFi.dnsIP().toString() + "\"" + endChar); // DNS.
    Serial2.print("page2.t7.txt=\"" + WiFi.macAddress() + "\"" + endChar); // MAC.
  }
  else {
    // Page 1
    Serial2.print("page1.r0.pco=63488" + endChar);
    Serial2.print("page1.r0.bco=63488" + endChar);
    // Page 2
    Serial2.print("page2.r0.pco=63488" + endChar);
    Serial2.print("page2.r0.bco=63488" + endChar);
    // SSID, IP, DNS, MAC
    Serial2.print("page2.t4.txt=\"SSID\"" + endChar); // SSID.
    Serial2.print("page2.t5.txt=\"xxx:xxx:xxx:xxx\"" + endChar); // IP.
    Serial2.print("page2.t6.txt=\"xxx:xxx:xxx:xxx\"" + endChar); // DNS.
    Serial2.print("page2.t7.txt=\"xx:xx:xx:xx:xx:xx\"" + endChar); // MAC.
  }
}

/**
 * @brief Initialize the WiFi mode to station mode and disconnect if connected
 * @param  none
 */
void WiFi_Init(void)
{
  WiFi.mode(WIFI_STA);  
  WiFi.disconnect();
}

/**
 * @brief Initialize the Function GET
 * @param  none
 */
void Function_GET(void){
  Serial2.print("page1.bt0.txt=\"SCANNING...\"" + endChar);
  Serial2.print("page1.t0.txt=\"WiFi SSID\"" + endChar);
  Serial.println("==============================================================================");
  Serial.println("Start Scanning");
  int n = WiFi.scanNetworks();
  Serial2.print("page1.va2.val=" + String(n - 1) + endChar);
  if(n == 0) {
    Serial.println("No Networks Found");
  }
  else {
    for(int i = 0; i < n; ++i) {
      Serial.println(WiFi.SSID(i));
    }
    Serial2.print("page1.bt0.txt=\"" + WiFi.SSID(0) + "\"" + endChar);
    Serial2.print("page1.t0.txt=\"WiFi SSID (" + String(0) + ")" + "\"" + endChar);
  }
  Serial.println("Scanning Completed");
  Serial.println("==============================================================================");
}

/**
 * @brief Initialize the Function CON
 * @param  String value
 */
void Function_CON(String value){
  int colonLoc = value.indexOf(":");
  SSIDname = value.substring(0,colonLoc);
  SSIDpassword = value.substring(colonLoc + 1);
  Serial.println("==============================================================================");
  WiFi.begin(SSIDname.c_str(),SSIDpassword.c_str());
  Serial.println("Dirección IP   .: " + WiFi.localIP().toString());
}

/**
 * @brief Initialize the Function NUM
 * @param  String value
 */
void Function_NUM(String value){
  int colonLoc = value.indexOf(":");
  SSIDcontador = value.substring(0,colonLoc).toInt();
  Serial.println("==============================================================================");
  Serial.println("SSIDcontador .: " + String(SSIDcontador));
  Serial2.print("page1.t0.txt=\"WiFi SSID (" + String(SSIDcontador) + ")" + "\"" + endChar);
  Serial2.print("page1.bt0.txt=\"" + WiFi.SSID(SSIDcontador) + "\"" + endChar);
}
//*************************************************************************************************//