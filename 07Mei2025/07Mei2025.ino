#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// WiFi Credentials
const char* ssid = "OutNest3 -2.4G";
const char* password = "Zidan1012";

// Server Configuration
const char* serverUrl = "https://happykoala.infy.uk/api.php";
const char* root_ca = \ 
"-----BEGIN CERTIFICATE-----\n" \
"MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n" \
"RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n" \
"VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n" \
"DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n" \
"ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n" \
"VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n" \
"mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n" \
"IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMb6HKLm+3Y\n" \
"7fTmg+T0Hmr9Ggm4b6g1IfRZY9gC5v94S1JyKYDaV4K3n8fQf+7z7Aq4jxqy6l+6\n" \
"Z6OemJqaX7I/MTKGemR6dX/7Jf9q5J6+YmOxl8qgpYp3hA8L2GuQpZ6Yq4ZQnL3q\n" \
"4OY9S6Ztn1h5Tm7Wmw9F7Ql+ZwPdJY1wKNz4lSqKP5Oe6Yd6r5X9kCAwEAAaNFME\n" \
"UwQwYDVR0fBDwwOjA4oDagNIYyaHR0cDovL2NybC5jb21vZG9jYS5jb20vQmFsdG\n" \
"ltb3JlQ3liZXJUcnVzdFJvb3QuY3JsMA0GCSqGSIb3DQEBBQUAA4IBAQCFDFUulf\n" \
"X6z2p7s9hQ8Z0qDGXDJ1+5QY7mJm+4X5Z1L6bYVJ7JvXQZxq9L6w7zJ8Q9J6J8Q\n" \
"9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6\n" \
"J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q\n" \
"9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6J8Q9J6\n" \
"-----END CERTIFICATE-----\n";

WebServer server(80);
bool useWebsocket = false;

float flowRate = 0.0;
float totalWater = 0.0;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  if (useWebsocket) {
    server.on("/flow", handleFlowData);
    server.begin();
  }
}

void loop() {
  if (useWebsocket) {
    server.handleClient();
  }

  // Simulate sensor data
  flowRate = random(1, 70) / 10.0;
  totalWater += flowRate / 60.0 * 2.0;

  sendToDashboard();
  delay(2000);
}

void sendToDashboard() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected!");
    return;
  }

  DynamicJsonDocument doc(128);
  doc["flow"] = flowRate;
  doc["total"] = totalWater;
  String jsonPayLoad;
  serializeJson(doc, jsonPayLoad);

  if (!useWebsocket) {
    HTTPClient http;
    http.begin(serverUrl); // Added root certificate
    http.addHeader("Content-Type", "application/json");
    
    Serial.println("Sending: " + jsonPayLoad);

    int httpCode = http.POST(jsonPayLoad);
    if (httpCode > 0) {
      Serial.printf("HTTP Code: %d\n", httpCode);
      Serial.print("Response: ");
      Serial.println(http.getString());
    } else {
      Serial.printf("HTTP Error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.println("WebSocket Data: " + jsonPayLoad);
  }
}

void handleFlowData() {
  DynamicJsonDocument doc(128);
  doc["flow"] = flowRate;
  doc["total"] = totalWater;
  String jsonPayLoad;
  serializeJson(doc, jsonPayLoad);

  server.send(200, "application/json", jsonPayLoad);
  Serial.println("WebSocket Sent: " + jsonPayLoad);
}