#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define wifi_ssid "Savelberg"
#define wifi_password "777888991"

#define mqtt_server "mqtt.thingspeak.com"
#define mqtt_user " waithiageni"
#define mqtt_password "Nim@311905@bri"



const int sensorIn = A0;
int mVperAmp = 100; // use 100 for 20A Module and 66 for 30A Module and 185 for 5A Module

double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup(){ 
 Serial.begin(9600);
 setup_wifi();
 client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
       if (client.connect("ESP8266Client")) {
    // if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop(){
 if (!client.connected()) {
    reconnect();
  }
 client.loop();

 Voltage = getVPP();
 VRMS = (Voltage/2.0) *0.707;  
 AmpsRMS = (VRMS * 1000)/mVperAmp;
 Serial.print(AmpsRMS);
 Serial.println(" Amps RMS");
 client.publish(String(AmpsRMS).c_str(), true);
}

float getVPP()
{
  float result;
  int readValue;             
  int maxValue = 0;          
  int minValue = 1024;         
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;
      
   return result;
 }
