#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#define LDR1 D0
#define LDR2 D1
#define LDR3 D2
#define LDR4 D3

#define LED1 D8
#define LED2 D8
#define LED3 D8
#define LED4 D5
#define LED5 D5
#define LED6 D5

const char* ssid = "project1";    // Replace with your Wi-Fi SSID
const char* password = "123456789"; // Replace with your Wi-Fi password

// Circuit Digest API setup
const String apiKey = "45ghQC83RkWN";    // Replace with your API key from Circuit Digest

// SMS details (users can easily modify these)
const char* templateID = "107";                  // Template ID //moreinfo: https://circuitdigest.com/article/free-sms-api-for-arduino-esp32-esp8266-nodemcu-raspberry-pi
const char* mobileNumber = "916363176354";       // Mobile number (with country code)
const char* var1 = "Light failure";            // Variable 1
const char* var2 = "Street light's";         // Variable 2
WiFiClient client;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Set up LEDs as output
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);

  // Connect to WiFi
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  // Initially, no SMS has been sent, so we are ready to send
}

void sendSMS() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client; // Use WiFiClientSecure for HTTPS connections
    client.setInsecure();    // Skip certificate validation (not secure but works for development)
    HTTPClient http;

    // Build the API URL with the template ID
    String apiUrl = "https://www.circuitdigest.cloud/send_sms?ID=" + String(templateID);

    // Start the HTTPS connection with WiFiClientSecure
    http.begin(client, apiUrl);
    http.addHeader("Authorization", apiKey);
    http.addHeader("Content-Type", "application/json");

    // Create the JSON payload with SMS details
    String payload = "{\"mobiles\":\"" + String(mobileNumber) + "\",\"var1\":\"" + String(var1) + "\",\"var2\":\"" + String(var2) + "\"}";

    // Send POST request
    int httpResponseCode = http.POST(payload);

    // Check response
    if (httpResponseCode == 200) {
      Serial.println("SMS sent successfully!");
      Serial.println(http.getString());
    } else {
      Serial.print("Failed to send SMS. Error code: ");
      Serial.println(httpResponseCode);
      Serial.println("Response: " + http.getString());
    }

    http.end(); // End connection
  } else {
    Serial.println("WiFi not connected!");
  }
}

void loop() {
  int val1 = digitalRead(LDR1);
  int val2 = digitalRead(LDR2);
  int val3 = digitalRead(LDR3);
  int val4 = digitalRead(LDR4);

  // Condition 1: When LDR1 detects no light (e.g., value > threshold), turn on 3 LED lights
  if (val1 == HIGH) {  // Adjust threshold based on your environment
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, LOW);
    digitalWrite(LED6, LOW);
  }

  // Condition 2: If the first 3 LED lights are on, check LDR2, LDR3, LDR4
  if (digitalRead(LED1) == HIGH && digitalRead(LED2) == HIGH && digitalRead(LED3) == HIGH) {
    if (val2 == HIGH && val3 == HIGH && val4 == HIGH) {
      // All 3 LDR sensors failed to detect light, turn on the remaining 3 LEDs
      digitalWrite(LED4, HIGH);
      digitalWrite(LED5, HIGH);
      digitalWrite(LED6, HIGH);
      

      sendSMS();  // Send SMS to the user

    } else {
      digitalWrite(LED4, LOW);
      digitalWrite(LED5, LOW);
      digitalWrite(LED6, LOW);
    }
  }

  // Delay before the next reading
  delay(1000);
}
