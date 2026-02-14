#include <WiFi.h>
#include <WiFiClientSecure.h>

#define IR_PIN D0              
#define COOLDOWN_MS 15000

const char* ssid     = "YOUR_WiFi_SSID";
const char* password = "YOUR_WiFi_PASSWORD";
const char* apiKey   = "YPUR_API_KEY";

const char* host = "www.circuitdigest.cloud";
const int httpsPort = 443;

WiFiClientSecure client;

unsigned long lastSentTime = 0;

void sendWhatsApp()
{
  if (!client.connect(host, httpsPort)) {
    Serial.println("HTTPS connection failed");
    return;
  }

  String payload =
    "{"
    "\"phone_number\":\"919876543210\","
    "\"template_id\":\"threshold_violation_alert\","
    "\"variables\":{"
      "\"device_name\":\"Living Room Motion Node\","
      "\"parameter\":\"Motion\","
      "\"measured_value\":\"Motion Detected\","
      "\"limit\":\"N/A\","
      "\"location\":\"Living Room\""
    "}"
    "}";

  client.println("POST /api/v1/whatsapp/send HTTP/1.1");
  client.println("Host: www.circuitdigest.cloud");
  client.println("Authorization: " + String(apiKey));
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(payload.length());
  client.println();
  client.println(payload);

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }

  Serial.println(client.readString());
  client.stop();
}

void setup()
{
  Serial.begin(115200);

  pinMode(IR_PIN, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");

  client.setInsecure();   // acceptable only for testing
}

void loop()
{
  int motionState = digitalRead(IR_PIN);
  unsigned long now = millis();

  if (motionState == LOW) {
    Serial.println("Motion Detected");

    if (now - lastSentTime > COOLDOWN_MS) {
      Serial.println("Sending WhatsApp Alert");
      sendWhatsApp();
      lastSentTime = now;
    }
  } else {
    Serial.println("No Motion");
  }

  delay(500);
}
