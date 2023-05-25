#include <Servo.h>  
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PIN A0

// Update these with values suitable for your network.
const char* ssid = "Universitas Mulawarman";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";

Servo myservo;
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];




void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("iot_unmul/iot_a_3/katup_manual");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan diterima: ");
  String data = ""; // variabel untuk menyimpan data yang berbentuk array char
  for (int i = 0; i < length; i++) {
    Serial.println((char)payload[i]);
    data += (char)payload[i]; // menyimpan kumpulan char kedalam string
  }
  int keran = data.toInt();
  if(keran == 0){
    close();        
  }   
  else if (keran == 1){
    open();
  }
}


void open(){
  myservo.write(280);
}
void close(){
  myservo.write(80);
}

void setup() {
  myservo.attach(D8); 
  Serial.begin(9600);  // sensor buart rate
  pinMode(PIN,INPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  int sensor = analogRead(PIN);  
  delay(500);                  
  snprintf (msg, MSG_BUFFER_SIZE, "%s", itoa(sensor,msg,10)); // itoa (konversi integer ke string)
  Serial.print("Ketinggian Air: ");
  Serial.println(msg);
  client.publish("iot_unmul/iot_a_3/air", msg); // fungsi untuk publish ke broker

}