#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <HX711.h>
#include <Servo.h>
#define DOUT  D6
#define CLK  D7
HX711 scale(DOUT, CLK);

Servo myservo;

const char* ssid = "VIRUZ";
const char* password = "08121996";

const char* mqtt_server = " 192.168.43.39";

WiFiClient espClient;
PubSubClient client(espClient);

float calibration_factor = 142.20;
int GRAM;

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
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  scale.set_scale();
  scale.tare();
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
     Serial.println("SERVO TERBUKA");
    myservo.write(45);
  } 
 
 if ((char)payload[0] == '0') {
    Serial.println("SERVO TERTUTUP");
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
  scale.set_scale(calibration_factor);
  GRAM = scale.get_units(), 4;
  Serial.println(GRAM);
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance= duration*0.034/2;


    if (isnan(dsitance) || isnan(berat)) {
      Serial.println("Failed to read from sensor!");
      return;
    }

    static char ultra[7];
    dtostrf(distance, 6, 2, ultra);
    static char beban[7];
    dtostrf(GRAM, 6, 2, beban);
  

    client.publish("jarak", ultra);
    client.publish("beban", beban);

    Serial.print("Nilai Berat: ");
    Serial.println(GRAM);
    Serial.print("Jarak : ");
    Serial.print(distance);

  }
} 
