#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Servo.h>


Servo myservo;
#define DHTTYPE DHT11   

const int ldr = A0;

const char* ssid = "VIRUZ";
const char* password = "08121996";

const char* mqtt_server = " 192.168.43.39";

WiFiClient espClient;
PubSubClient client(espClient);

const int trigPin = D1; 
const int echoPin = D2;

long duration;
float distance;

long now = millis();
long lastMeasure = 0;
long lastMsg= 0;
char msg[50];
int value = 0;

void setup() {
  myservo.attach(D4);
  myservo.write(0);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
Serial.begin(115200);
}
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

 void callback(char* topic, byte* payload, unsigned int length) {
  String string;
  Serial.print("Pesan Diterima [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    string+=((char)payload[i]);
  }
  Serial.println(string);
 
  if ((char)payload[0] == '1') {
     Serial.println("ATAP TERBUKA");
    myservo.write(90);
  } 
 
 if ((char)payload[0] == '0') {
    Serial.println("ATAP TERTUTUP");
    myservo.write(0); 
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
      client.subscribe("servo");
    } else {
      Serial.print("failed, rc= ");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(3000);
    }
  }
}



void loop() {
  if(!client.connected()) {
    reconnect();
  }
  
  if(!client.loop())
    client.connect("ESP8266Client");

  now = millis();
  if (now - lastMeasure > 2000) {
    lastMeasure = now;
   
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

digitalWrite(trigPin, HIGH);
delayMicroseconds(10);

digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
distance= duration*0.034/2;


//    if (isnan(dsitance) || isnan(berat)) {
 //     Serial.println("Failed to read from sensor!");
 //     return;
 //   }

    static char ultra[7];
    dtostrf(distance, 6, 2, ultra);
    

    client.publish("jarak", ultra);


    Serial.print("Jarak : ");
    Serial.print(distance);

  }
} 
