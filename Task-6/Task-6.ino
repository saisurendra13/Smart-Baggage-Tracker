#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

const char* server = "baggage-tracker-server.com"; // Replace with your server address

static const int RXPin = 5, TXPin = 4;   // Connect GPS Tx to D4, GPS Rx to D5
static const uint32_t GPSBaud = 9600;    // GPS baud rate

TinyGPSPlus gps; // GPS object

WiFiClient client;

SoftwareSerial gpsSerial(RXPin, TXPin);  // GPS serial connection

char ssid[] = "YourWiFiSSID";           // Your WiFi SSID
char pass[] = "YourWiFiPassword";       // Your WiFi Password

void setup() {
  Serial.begin(9600);     // Start Serial for debugging
  gpsSerial.begin(GPSBaud); // Start GPS Serial

  // Connect to WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void loop() {
  if (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      sendLocation();
    }
  }
}

void sendLocation() {
  if (gps.location.isValid()) {
    float latitude = gps.location.lat();
    float longitude = gps.location.lng();

    Serial.print("Latitude: ");
    Serial.println(latitude, 6);
    Serial.print("Longitude: ");
    Serial.println(longitude, 6);

    if (WiFi.status() == WL_CONNECTED) {
      if (client.connect(server, 80)) {
        String postData = "lat=" + String(latitude, 6) + "&lon=" + String(longitude, 6);
        client.print("POST /updateLocation HTTP/1.1\r\n");
        client.print("Host: ");
        client.print(server);
        client.print("\r\n");
        client.print("Content-Type: application/x-www-form-urlencoded\r\n");
        client.print("Content-Length: ");
        client.print(postData.length());
        client.print("\r\n\r\n");
        client.print(postData);
        client.print("\r\n");
      }
      client.stop();
    }
  }
  delay(30000); // Delay for 30 seconds before sending the next location
}
