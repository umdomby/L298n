const char* ssid = "Robolab2";
const char* password = "wifi123123123";
const char* idSocket = "995511";
int speed = 85;

#include <Arduino.h>
#include "PinDefinitionsAndMore.h"
#include "ServoEasing.hpp"
ServoEasing Servo1;
ServoEasing Servo2;

#define ONOFF D0
// #define STOP D1

#define LPWM D6
#define FBL D2
//#define FBLL D7

#define RPWM D5
#define FBR D3
//#define FBRR D8

#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>


#define START_DEGREE_VALUE  90 


unsigned long messageInterval = 1000;
bool connected = false;
const char* websockets_server_host = "servicerobot.pro"; // Enter server adress
//const char* websockets_server_host = "192.168.0.101"; // Enter server adress
//const char* websockets_server_host = "93.125.10.70"; // Enter server adress
const uint16_t websockets_server_port = 8081; // Enter server port

using namespace websockets;

unsigned long lastUpdate = millis();
unsigned long lastUpdate2 = millis();
unsigned long lastUpdate22 = millis();
unsigned long lastUpdate10 = millis();
unsigned long lastUpdate15 = millis();

String output;

unsigned long timing;

StaticJsonDocument<200> doc;
StaticJsonDocument<512> doc2;

int stop = 0;
const char* method = "";
const char* username = "";
boolean messageOnOff = true;
boolean messageStop = true;
boolean messageFBL = true;
boolean messageFBR = true;
float messageL = 0;
float messageR = 0;
float messageLL = 0;
float messageRR = 0;
float messageLT = 0;
float messageRT = 0;

#define saddleUPpin D1
#define saddleDOWNpin D4
boolean  saddleUP = true;
boolean  saddleDOWN = true;

// #define LIGHT_PIN D4
//boolean light = false;

int accel = 1;
boolean connectByte = false;

WebsocketsClient client;

void onMessageCallback(WebsocketsMessage messageSocket) {
    //Serial.print("Got Message: ");
    //Serial.println(messageSocket.data());

    DeserializationError error = deserializeJson(doc, messageSocket.data());
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    };

    method = doc["method"];

    if(String(method) == "connection"){
        Serial.printf("[WSc] WStype_CONNECTED\n");
    };

    if(String(method) == "messages"){
        connectByte = doc["connectByte"];
        lastUpdate2 = millis();
        lastUpdate22 = millis();
        connectByte = false;
        stop = doc["stop"];
        accel = doc["accel"];
    };

    if(String(method) == "messagesLTRT"){
        digitalWrite(ONOFF, LOW);
        messageL = doc["messageL"];
        messageR = doc["messageR"];
        lastUpdate10 = millis();
        analogWrite(LPWM, messageL);
        analogWrite(RPWM, messageR);
        Serial.printf("LT = %s\n", String(messageL));
        Serial.printf("RT = %s\n", String(messageR));
    };

    if(String(method) == "messageFBLL"){
        messageLL = doc["messageLL"];
        Servo1.easeTo(messageLL,450);
        Serial.printf("LL = %s\n", String(messageLL));
    };

    if(String(method) == "messageFBRR"){
        messageRR = doc["messageRR"];
        Servo2.easeTo(messageRR, 450);
        Serial.printf("RR = %s\n", String(messageRR));
    };

    if(String(method) == "messagesL"){
        stop = doc["stop"];
        accel = doc["accel"];
        digitalWrite(ONOFF, LOW);
        lastUpdate10 = millis();
        messageL = doc["messageL"];
        // if(messageL < 0){
        //     messageL = (messageL) * -1;
        // }else{messageL = messageL * speed;}
        //messageR = doc["messageR"];
        Serial.printf("Left = %s\n", String(messageL));
        //Serial.printf("Right = %s\n", String(messageR));
        doc2["method"] = "messagesL";
        doc2["messageL"] = messageL;
        //doc2["messageR"] = messageR;
        String output = doc2.as<String>();
        client.send(output);
    };

    if(String(method) == "messagesR"){
        stop = doc["stop"];
        accel = doc["accel"];
        //messageL = doc["messageL"];
        digitalWrite(ONOFF, LOW);
        lastUpdate10 = millis();
        messageR = doc["messageR"];
        // if(messageR < 0){
        //     messageR = (messageR * speed) * -1;
        // }else{messageR = messageR * speed;}
        //Serial.printf("Left = %s\n", String(messageL));
        Serial.printf("Right = %s\n", String(messageR));
        doc2["method"] = "messagesR";
        //doc2["messageL"] = messageL;
        doc2["messageR"] = messageR;
        String output = doc2.as<String>();
        client.send(output);
    };

    if(String(method) == "messagesOnOff"){
        messageOnOff = doc["messageOnOff"];
        doc2["method"] = "messagesOnOff";
        doc2["messageOnOff"] = messageOnOff;
        Serial.printf("OnOff = %s\n", String(messageOnOff));
        String output = doc2.as<String>();
        client.send(output);
        messageL = 0;
        messageR = 0;
        digitalWrite(ONOFF, messageOnOff);
    };
    // if(String(method) == "messagesStop"){
    //     messageStop = doc["messageStop"];
    //     doc2["method"] = "messagesStop";
    //     doc2["messageStop"] = messageStop;
    //     Serial.printf("Stop = %s\n", String(messageStop));
    //     String output = doc2.as<String>();
    //     client.send(output);
    //     digitalWrite(STOP, messageStop);
    // };
    if(String(method) == "messagesFBLR"){
        messageFBL = doc["messageFBL"];
        messageFBR = doc["messageFBR"];
        doc2["method"] = "messagesFBLR";
        doc2["messageFBL"] = messageFBL;
        doc2["messageFBR"] = messageFBR;
        Serial.printf("UpDownLeft = %s", String(messageFBL));
        Serial.printf(" UpDownRight = %s\n", String(messageFBR));
        String output = doc2.as<String>();
        client.send(output);
        digitalWrite(FBL, messageFBL);
        digitalWrite(FBR, messageFBR);
    };

        if(String(method) == "saddleUP"){
        saddleUP = doc["message"];
        doc2["method"] = "saddleUP";
        doc2["saddleUP"] = saddleUP;
        Serial.printf("sanddleUP = %s\n", String(saddleUP));
        String output = doc2.as<String>();
        client.send(output);
        if(digitalRead(saddleDOWNpin) != LOW){
            digitalWrite(saddleUPpin, saddleUP);
        }
    };

    if(String(method) == "saddleDOWN"){
        saddleDOWN = doc["message"];
        doc2["method"] = "saddleDOWN";
        doc2["saddleDOWN"] = saddleDOWN;
        Serial.printf("saddleDOWN = %s\n", String(saddleDOWN));
        String output = doc2.as<String>();
        client.send(output);
        if(digitalRead(saddleUPpin) != LOW){
            digitalWrite(saddleDOWNpin, saddleDOWN);
        }
    };

    if(String(method) == "light"){
        // light = doc["message"];
        // doc2["method"] = "light";
        // doc2["light"] = light;
        // Serial.printf("light = %s\n", String(light));
        // String output = doc2.as<String>();
        // client.send(output);
        // digitalWrite(LIGHT_PIN, light);
    };

};

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        connected = true;
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        connected = false;
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    };
}


void socketSetup(){
    // run callback when messages are received
    client.onMessage(onMessageCallback);
    // run callback when events are occuring
    client.onEvent(onEventsCallback);
    // Connect to server
    client.connect(websockets_server_host, websockets_server_port, "/");
    // Send a message
    //client.send("Hello Server");
    doc2["method"] = "connection";
    //doc2["id"] = "b078167f69934795e54a54dc831acea8|a46d12213abfad52db817c17e1fec1ae";
    doc2["id"] = idSocket;
    output = doc2.as<String>();
    client.send(output);
    // Send a ping
    client.ping();
};

void setup() {


    pinMode(RPWM,OUTPUT);
    pinMode(LPWM,OUTPUT);
    analogWrite(RPWM, 0);   
    analogWrite(LPWM, 0);

    pinMode(FBL,OUTPUT);
    pinMode(FBR,OUTPUT);
    digitalWrite(FBL, HIGH);
    digitalWrite(FBR, HIGH);

    pinMode(saddleUPpin, OUTPUT);
    digitalWrite(saddleUPpin, true);
    pinMode(saddleDOWNpin, OUTPUT);
    digitalWrite(saddleDOWNpin, true);

    // pinMode(LIGHT_PIN,OUTPUT);
    // digitalWrite(LIGHT_PIN, false);

    // pinMode(FBLL,OUTPUT);
    // pinMode(FBRR,OUTPUT);
    // analogWrite(FBLL, 0);
    // analogWrite(FBRR, 0);


    pinMode(ONOFF, OUTPUT);
    digitalWrite(ONOFF, HIGH);
    // pinMode(STOP, OUTPUT);
    // digitalWrite(STOP, messageStop);

    Serial.begin(115200);
    // Connect to wifi
    WiFi.begin(ssid, password);

    // Wait some time to connect to wifi
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    };

    socketSetup();

    //servo
    #if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL)  || defined(ARDUINO_attiny3217)
    delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
    #endif

    if (Servo1.attach(SERVO1_PIN, START_DEGREE_VALUE) == INVALID_SERVO) {
        Serial.println(F("Error attaching servo"));
    }
	if (Servo2.attach(SERVO2_PIN, START_DEGREE_VALUE) == INVALID_SERVO) {
        Serial.println(F("Error attaching servo"));
    }
}

// void socetConnected(){
//     client.connect(websockets_server_host, websockets_server_port, "/");
//     Serial.println("[WSc] SENT: Simple js client message!!");
//     doc2["method"] = "connection";
//     doc2["id"] = "b078167f69934795e54a54dc831acea8|a46d12213abfad52db817c17e1fec1ae";
//     output = doc2.as<String>();
//     client.send(output);  
// };

void loop(){

    client.poll();

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi.status()-----------------------------------");
        // Serial.println("WiFi.reconnect()-----------------------------------");
        //WiFi.disconnect();
        //WiFi.reconnect();

        digitalWrite(ONOFF, HIGH); 
        analogWrite(LPWM, 0);
        analogWrite(RPWM, 0);
        digitalWrite(saddleUPpin, true);
        digitalWrite(saddleDOWNpin, true);
        ESP.restart();
        socketSetup();

        // ESP.reset(); 
        // WiFi.disconnect();
        // WiFi.reconnect();
    };

    // digitalWrite(STOP, messageStop);

    if (lastUpdate + messageInterval < millis()){
        if (connected == false){
            digitalWrite(ONOFF, HIGH); 
            analogWrite(LPWM, 0);
            analogWrite(RPWM, 0);
            digitalWrite(saddleUPpin, true);
            digitalWrite(saddleDOWNpin, true);
            Serial.printf(", Socket RESTART =================================== %s\n", String(connected));
            ESP.restart();
            socketSetup();
        };
        lastUpdate = millis();
    };


    if (lastUpdate2 + 1200 < millis()){ 
        analogWrite(LPWM, 0);
        analogWrite(RPWM, 0);
        digitalWrite(saddleUPpin, true);
        digitalWrite(saddleDOWNpin, true);
        Serial.println("lastUpdate2");
        // ESP.reset(); 
        // WiFi.disconnect();
        // WiFi.reconnect();
        lastUpdate2 = millis();
    };

    if (lastUpdate22 + 8000 < millis()){ 
        analogWrite(LPWM, 0);
        analogWrite(RPWM, 0);
        digitalWrite(saddleUPpin, true);
        digitalWrite(saddleDOWNpin, true);
        Serial.println("lastUpdate22 + 5000, ESP.restart()");
        ESP.restart();
        lastUpdate22 = millis();
    };

    if (lastUpdate10 + 10000 < millis()){
        digitalWrite(ONOFF, HIGH); 
        messageL = 0;
        messageR = 0;
        Serial.println("lastUpdate10");
        lastUpdate10 = millis();
    };

    //messageOnOff = doc["messageOnOff"];

    //NEED
    if (lastUpdate15 + 10000 < millis()){
        Serial.printf("millis() = %s", String(millis()));
        Serial.printf(", WiFi.status() = %s", String(WiFi.status()));
        Serial.printf(", WL_CONNECTED = %s", String(WL_CONNECTED));
        Serial.printf(", connected = %s", String(connected));
        Serial.printf(", connectByte = %s\n", String(connectByte));

    //     doc2["method"] = "messages";
    //     doc2["id"] = idSocket;
    //     doc2["messageL"] = String(messageL);
    //     doc2["messageR"] = String(messageR);
    //     output = doc2.as<String>();
    //     client.send(output);

    //     doc2["method"] = "messagesFBL";
    //     doc2["messageFBL"] = messageFBL;
    //     client.send(doc2.as<String>());

    //     doc2["method"] = "messagesFBR";
    //     doc2["messageFBR"] = messageFBR;
    //     client.send(doc2.as<String>());

    //     doc2["method"] = "messagesOnOff";
    //     doc2["messageOnOff"] = messageOnOff;
    //     client.send(doc2.as<String>());

         lastUpdate15 = millis();
    };

    // analogWrite(LPWM, messageL);
    // analogWrite(RPWM, messageR);
    // Serial.printf(", messageL = %s\n", String(messageL));
    // Serial.printf(", messageR = %s\n", String(messageR));
    // Servo1.easeTo(messageLL,450);
    // Servo2.easeTo(messageRR,450);

    // analogWrite(FBLL, messageLL);
    // analogWrite(FBRR, messageRR);

}
