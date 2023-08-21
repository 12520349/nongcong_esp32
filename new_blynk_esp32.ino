/*************************************************************

  You’ll need:
   - Blynk IoT app (download from App Store or Google Play)
   - ESP32 board
   - Decide how to connect to Blynk
     (USB, Ethernet, Wi-Fi, Bluetooth, ...)

  There is a bunch of great example sketches included to show you how to get
  started. Think of them as LEGO bricks  and combine them as you wish.
  For example, take the Ethernet Shield sketch and combine it with the
  Servo example, or choose a USB sketch and add a code from SendData
  example.
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPLXe8rEHxw"
#define BLYNK_TEMPLATE_NAME "LEDCONTROL"
#define BLYNK_AUTH_TOKEN "DyGDVaSKYcwqPCN29U7N09YU66CPWAxM"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <WiFi.h>

#include <WiFiClient.h>

#include <BlynkSimpleEsp32.h>

#include <LiquidCrystal_I2C.h>

int totalColumns = 16;
int totalRows = 2;

LiquidCrystal_I2C lcd(0x27, totalColumns, totalRows);

const unsigned long watchdogTimeout = 60000; // Thời gian chờ của watchdog (60 giây)
unsigned long lastWatchdogReset = 0;

hw_timer_t * watchdogTimer = NULL;

#include "DHTesp.h"

#define DHTpin 15 //D15 of ESP32 DevKit

DHTesp dht;
const int analogPin = 36; // Analog input pin 0 (GPIO 36)
int max_khigas = 2000;
int max_nhietdo = 30;

byte degree[8] = {
  0B01110,
  0B01010,
  0B01110,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000
};

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "OKBABE";
char pass[] = "12345678A";

BlynkTimer timer;
//int led = 4;
int congtac1 = 4;
int congtac2 = 16;
int congtac3 = 17;
int congtac4 = 5;

BLYNK_WRITE(V1) {
  int trangthai = param.asInt();
  digitalWrite(congtac1, trangthai);
  lcd.clear();
  lcd.setCursor(0, 0);
  if (trangthai == 1) {
    lcd.print("Relay 1 => ON");
  } else {
    lcd.print("Relay 1 => OFF");

  }

}
BLYNK_WRITE(V2) {
  int trangthai = param.asInt();
  digitalWrite(congtac2, trangthai);
  lcd.clear();
  lcd.setCursor(0, 0);
  if (trangthai == 1) {
    lcd.print("Relay 0 => ON");
  } else {
    lcd.print("Relay 0 => OFF");

  }
}
BLYNK_WRITE(V3) {
  int trangthai = param.asInt();
  digitalWrite(congtac3, trangthai);
  lcd.clear();
  lcd.setCursor(0, 0);
  if (trangthai == 1) {
    lcd.print("Relay 3 => ON");
  } else {
    lcd.print("Relay 3 => OFF");

  }
}

BLYNK_WRITE(V4) {
  int trangthai = param.asInt();
  digitalWrite(congtac4, trangthai);
  lcd.clear();
  lcd.setCursor(0, 0);
  if (trangthai == 1) {
    lcd.print("Relay 2 => ON");
  } else {
    lcd.print("Relay 2 => OFF");

  }
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent() {
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}
void sendSensor() {

  //delay(dht.getMinimumSamplingPeriod());

  float h = dht.getHumidity();
  float t = dht.getTemperature();

  /*
  float h = dht.readHumidity();
// Read temperature as Celsius (the default)
float t = dht.readTemperature();
*/
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  } else {

    if (t > max_nhietdo) {
      digitalWrite(congtac1, 1);
    } else {
      digitalWrite(congtac1, 0);
    }

    lcd.clear();
    lcd.setCursor(0, 0); // Đặt vị trí con trỏ của màn hình LCD tại hàng 0, cột 0

    lcd.print("Nhiet do:");
    lcd.setCursor(0, 1);
    lcd.print("Do am:");

    lcd.createChar(1, degree);
    lcd.setCursor(10, 0);
    lcd.print(floor(round(t)));
    lcd.print(" ");
    lcd.write(1);
    lcd.print("C");

    lcd.setCursor(10, 1);
    lcd.print(floor(round(h)));
    lcd.print("%");
    delay(200);
  }

  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(h, 1);
  Serial.print("\t\t");
  Serial.print(t, 1);
  Serial.print("\t\t");

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);

}
void setup() {

  watchdogTimer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (1MHz)
  timerAttachInterrupt(watchdogTimer, & resetModule, true);
  timerAlarmWrite(watchdogTimer, watchdogTimeout * 1000, false); // Chuyển sang microgiây
  timerAlarmEnable(watchdogTimer);

  pinMode(congtac1, OUTPUT);
  pinMode(congtac2, OUTPUT);
  pinMode(congtac3, OUTPUT);
  pinMode(congtac4, OUTPUT);
  lcd.init();
  lcd.backlight(); // use to turn on and turn off LCD back light
  // Debug console

  dht.setup(DHTpin, DHTesp::DHT22); //for DHT22 Connect DHT sensor to GPIO 17

  // Debug console
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  //timer.setInterval(5*60*1000, connectionstatus);  // check every 5 minutes
  timer.setInterval(1000 L, sendSensor);
  timer.setInterval(1000 L, AnalogPinRead); // Run sensor scan 4 times a second
}
void AnalogPinRead() {
  int sensorValue = analogRead(analogPin); // Read the analog in value:
  Serial.print("sensor = "); // Print the results...
  Serial.println(sensorValue); // ...to the serial monitor:
  Blynk.virtualWrite(V7, sensorValue); // Send the results to Gauge Widget
  if (sensorValue > max_khigas) {
    Serial.println("Phat hien khi GAS"); // ...to the serial monitor:

    digitalWrite(congtac2, 1);
    lcd.print("Phat hien GAS");
    delay(1000);

  } else {
    digitalWrite(congtac2, 0);
    //lcd.print("GAS AN TOAN!");
    delay(1000);
  }

}

void loop() {
  Blynk.run();
  timer.run();

  if (millis() - lastWatchdogReset >= watchdogTimeout - 1000) {
    Serial.println("Reset timer");

    timerWrite(watchdogTimer, 0); // Reset timer
    lastWatchdogReset = millis();
  }
  delay(200);
}

void resetModule() {
  // Đặt lại ESP32
  ESP.restart();
}
