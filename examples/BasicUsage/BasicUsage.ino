#include <FINS.h>
#include <ESP8266WiFi.h>

const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

IPAddress local_ip(192, 168, 250, 100);
IPAddress plc_ip(192, 168, 250, 1);

FINS fins(local_ip, plc_ip);

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  if (fins.init()) {
    Serial.println("FINS initialized successfully");
  }
}

void loop() {
  int value = fins.readDM(100);
  if (value >= 0) {
    Serial.print("DM100 value: ");
    Serial.println(value);
  }
  
  fins.writeDM(101, 1234);
  delay(1000);
}