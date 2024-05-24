#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Define pins for GPS module
#define RXPin 4
#define TXPin 3

// Define pins for GSM module
#define GSM_RX 7
#define GSM_TX 8
#define GSM_PWR 9

// Define pin for the switch
#define switchPin 2

// Create instances of the software serial and GPS objects
SoftwareSerial gpsSerial(RXPin, TXPin);
SoftwareSerial gsmSerial(GSM_RX, GSM_TX);
TinyGPSPlus gps;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  gpsSerial.begin(9600);
  gsmSerial.begin(9600);

  // Set pin modes
  pinMode(GSM_PWR, OUTPUT);
  pinMode(switchPin, INPUT);

  // Power on the GSM module
  digitalWrite(GSM_PWR, HIGH);
  delay(1000);
  digitalWrite(GSM_PWR, LOW);
  delay(1000);
  digitalWrite(GSM_PWR, HIGH);
  delay(2000);

  // Wait for GSM module to initialize
  Serial.println("Initializing GSM module...");
  gsmSerial.println("AT");
  delay(1000);
  gsmSerial.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
  gsmSerial.println("AT+CSCS=\"GSM\""); // Select TE character set
  delay(1000);
}

void loop() {
  if (digitalRead(switchPin) == HIGH) {
    Serial.println("Switch is ON. Tracking location...");
    while (gpsSerial.available() > 0) {
      if (gps.encode(gpsSerial.read())) {
        if (gps.location.isUpdated()) {
          float latitude = gps.location.lat();
          float longitude = gps.location.lng();
          sendSMS(latitude, longitude);
        }
      }
    }
  } else {
    Serial.println("Switch is OFF. Waiting...");
  }
  delay(1000);
}

void sendSMS(float latitude, float longitude) {
  char message[160];
  sprintf(message, "Location: https://maps.google.com/?q=%f,%f", latitude, longitude);

  // Replace with your phone number
  char phoneNumber[] = "+1234567890";

  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(phoneNumber);
  gsmSerial.println("\"");
  delay(1000);
  gsmSerial.print(message);
  delay(100);
  gsmSerial.write(26); // Ctrl+Z ASCII code to send the SMS
  delay(1000);

  Serial.println("SMS sent:");
  Serial.println(message);
}
