#include <PubSubClient.h>
#include <GSM.h>

// PIN number if necessary
#define PINNUMBER ""

// APN information obrained from your network provider
#define GPRS_APN "data.dna.fi" // replace with your GPRS APN
#define GPRS_LOGIN "" // replace with your GPRS login
#define GPRS_PASSWORD "" // replace with your GPRS password

// initialize the library instances
GSMClient client;
GPRS gprs;
GSM gsmAccess;

//const char* server = "m13.cloudmqtt.com";
PubSubClient mqttClient(client);

void callback(char* topic, byte* payload, unsigned int length) {
  
  Serial.print("message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0]!=NULL)
    digitalWrite(10, HIGH);
}

void setup()
{
  Serial.begin(9600);
  StartGSM(); 
  connect_broker();
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  delay(2000);
  digitalWrite(10, LOW);
}
bool has_read = false;

void loop() {
//bool has_read = false;

String Incoming = "";

 if (Serial.available() > 0 || has_read==false) {
  if (Serial.read() == '{') {
     Incoming = '{' + Serial.readString(); 
     Incoming.trim();
     has_read=true;    
     digitalWrite(10, LOW);
    }
  }




  
  if (!mqttClient.connected()) {
    connect_broker();
  }
  
  if (has_read==true) {
    publish_message(Incoming);
    mqttClient.loop();
    has_read=false;
  }
}

void StartGSM() {

 Serial.println("starting GSM web client.");
 // connection state
 boolean notConnected = true;

 // Start GSM shield
 // pass the PIN of your SIM as a parameter of gsmAccess.begin()
  while(notConnected) {
    if((gsmAccess.begin(PINNUMBER)==GSM_READY) &
      (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY))
      notConnected = false;
    else {
    Serial.println("not connected");
    delay(1000);
    }
  }
}

void connect_broker() {
  char server[] = "m13.cloudmqtt.com"; // the broker URL
  int port = 15840; // the port
  Serial.print("connecting to broker...");
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);

  while(!mqttClient.connected()) {
    if(mqttClient.connect("ArduinoGSMid", "vesi", "vesimittaukset")) { //temporary client id from HiveMQ dashboard
      Serial.println(" connected");
      // connection succeeded
      mqttClient.subscribe("mittaukset/hello");
    } else {
    // connection failed
    Serial.print("broker connection failed, rc=");
    Serial.print(mqttClient.state());// will provide more information on failure
    Serial.println(" try again in 5 seconds");
    delay(5000);
    }
  }
}

void publish_message(String data_string_) {
    char data_string[100];
    data_string_.toCharArray(data_string, data_string_.length() + 1);
    Serial.println("publish_message");
    Serial.println(data_string);
  if(mqttClient.connect("ArduinoGSMid")) {
    mqttClient.publish("mittaukset/hello", data_string);

    delay(3000);
  }
}

  

