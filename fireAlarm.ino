#include <ESP8266HTTPClient.h>
//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include "FirebaseESP8266.h" //libreria de firebase
#include <ESP8266WiFi.h>


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//------Fin de Librerías----------//
 //nombre y contraseña de wifi
#define ssid "TP-Link_73CA"   //SSID
#define password "96508886"  //Contrseña de la red
#define host "192.168.0.109"  //Mi IP
//------------
#define FIREBASE_HOST "iot-proyecto-f7199-default-rtdb.firebaseio.com" //NOMBRE DE LA BASE DE DATOS EN FIREBASE
#define FIREBASE_AUTH "i2iAXYP07byjoOrIyh514oD1hxNvK7KaqFMmmBTE" //SECRETO DE FIREBASE

//DEFINIR EL OBJETO DE LA PLACA
FirebaseData firebaseData;
// Set up the server object
ESP8266WebServer server(80);   //PUERTO DEL LOCALHOST

FirebaseJson json;
 //--------INICIALIZACIÓN DE LAS VARIABLES 
void printResult(FirebaseData &data);
const int TMP = 0; //sensor de temperatura
const int Ledverde = D5;  //LED VERDE
const int Ledrojo = D6; //leds
int valor=0;
float voltValue=0.0; 
float temp= 0.0;
int frecuencia = 500; //frequencia do buzzer
float vref = 3.3;

void setup(){
 
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Conectando ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
//+++++++++++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++++++++    
  }
  
  server.begin();
  Serial.println("Servidor Iniciado");
  Serial.println("");
  Serial.println("Conectado a WIFI");
  Serial.print("dirección IP: ");
  Serial.println(WiFi.localIP());
//float temp = 100.0*(3.3*analogRead(A0)/1024);
// Configure the server's routes

  pinMode(A0, INPUT);
  pinMode(Ledverde, OUTPUT);

  pinMode(Ledrojo, OUTPUT);
  pinMode(D2, OUTPUT); //buzzer
  //Serial.begin(115200);

server.on("/",handleIndex); // use the top root path to report the last sensor valuer //S1
server.on("/update",handleUpdate); // use this route to update the sensor value //S2
server.begin();
   Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

   //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData.setBSSLBufferSize(1024, 1024);


  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData.setResponseSize(1024);


  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");


  Serial.println("------------------------------------");
  Serial.println("Updated temperature...");
 
}
//----------------------------+++++  HTML  +++++----------------------------//
void handleIndex(){ 
float temp = 100.0*(3.3*analogRead(A0)/1024); //Operación para el sensor
String Website = "<!DOCTYPE HTML>\r\n \r\n<html><head><title>FIRE ALARM</title><meta http-equiv=\"refresh\" content=\"2\"></head><body ><center> <h1> FIRE ALARM </h1> La temperatura es: "+ String(temp)+"C <br><center> </body></html>\n";

server.send(200,"text/html",Website); // we'll need to refresh the page for getting the lastest value //S1


 //server.send(200," text/html",Website);

 }
void handleUpdate(){

// The value will be passed as a URL argument //S2
 temp = server.arg("value").toFloat();
 Serial.println( temp);
 server.send(200,"text/plain","updated");
}
void printResult(FirebaseData &data)
{
Serial.println();
  FirebaseJson &json = data.jsonObject();
  //Print all object data
  Serial.println("Pretty printed JSON data:");
  String jsonStr;
  json.toString(jsonStr, true);
  Serial.println(jsonStr);
  Serial.println();
  Serial.println("Iterate JSON data:");
  //Serial.println();
}
 
void loop() {
  //+++++++++++++++++++++++++}
float temp = 100.0*(3.3*analogRead(A0)/1024);
Serial.print("connecting to ");
    Serial.println(host);
  
 Serial.print("Connected with IP: ");
 Serial.println(WiFi.localIP());
 server.handleClient();
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }
//BASE LOCAL

//float temp = 100.0*(3.3*analogRead(A0)/1024);
    // This will send the request to the server
 client.print(String("POST http://localhost/equipo4/proyecto.php?") + 
                          ("&temperatura=") + temp +               
                          " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 1000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
        
    }

    Serial.println();
    Serial.println("closing connection");
    
  delay(1000);
//lectura temperatura
// float temp = 100.0*(3.3*analogRead(A0)/1024);

String path = "/Test";

  json.set("TempereraturaCristian" + String(1), temp);

  if (isnan(temp)) {
    Serial.println("Ocurrio un error de lectura");
    client.print("Ocurrio un error de lectura");
    return;
  }
//-----Condición baja de temperatura (ENCIENDE LED VERDE)--------------///
   if (temp <= 24.00)    
  {
   delay(1000);
   digitalWrite(Ledverde, HIGH);
   digitalWrite(Ledrojo, LOW);
   
   Serial.print("Temperatura: ");
   Serial.print(temp);
   Serial.print("\t");
   Serial.println("Temperatura Normal"); //Mensaje de normalización de la temperatura
  }
 //Si no, la condición donde (Enciende LED ROJO) si es mayor 
  else if (temp >= 26.01)
  {
   delay(500); 
  
   digitalWrite(Ledverde, LOW);
   digitalWrite(Ledrojo, HIGH);
   tone(D2, frecuencia);
  
   delay(500);
   noTone(D2); 
   digitalWrite(Ledrojo, LOW);
     
   Serial.print("Temperatura: ");
   Serial.print(temp);
   Serial.print("\t");
   Serial.println("Peligro");  //Mensaje de peligro al estar en un área muy elevada
  }
  if (Firebase.updateNode(firebaseData, path + "/float", json))
  {
    //Serial.print("VALUE: ");
    printResult(firebaseData);
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  delay(1000);
}
