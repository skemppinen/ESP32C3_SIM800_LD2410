//Select your modem
//SSL/TLS is currently supported only with SIM8xx series
#define TINY_GSM_MODEM_SIM800

//Increase RX buffer
#define TINY_GSM_RX_BUFFER 256

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
#include <TinyGsmClient.h> //https://github.com/vshymanskyy/TinyGSM
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "prepaid.dna.fi";
char user[] = "";
char pass[] = "";
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
//GSM Module RX pin to ESP32 2
//GSM Module TX pin to ESP32 4
#define rxPin 0
#define txPin 1
HardwareSerial sim800(1);
TinyGsm modem(sim800);
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
TinyGsmClientSecure gsm_client_secure_modem(modem, 0);
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN


//LD2410 module RX & TX PIN //
//#define MONITOR_SERIAL Serial
//#define RADAR_SERIAL Serial2
#define RADAR_RX_PIN 2
#define RADAR_TX_PIN 3
#include <ld2410.h>
ld2410 radar;
uint32_t lastReading = 0;
bool radarConnected = false;

void loop(){
// SIM //
 if (sim800.available())
 Serial.write(sim800.read());

 if (Serial.available()) {
 while(Serial.available()){
 sim800.write(Serial.read());
 }
 sim800.println();
 }
  else
  {
    sim800.println(F("not connected"));
  }
// RADAR //
 radar.read();
 if(radar.isConnected() && millis() - lastReading > 1000)  //Report every 1000ms
 {
 lastReading = millis();
 if(radar.presenceDetected())
 {
 if(radar.stationaryTargetDetected())
 {
 sim800.print(F("Stationary target: "));
 sim800.print(radar.stationaryTargetDistance());
 sim800.print(F("cm energy:"));
 sim800.print(radar.stationaryTargetEnergy());
 sim800.print(' ');
 }
 if(radar.movingTargetDetected())
 {
 sim800.print(F("Moving target: "));
 sim800.print(radar.movingTargetDistance());
 sim800.print(F("cm energy:"));
 sim800.print(radar.movingTargetEnergy());
 }
 sim800.println();
 }
 else
 {
 sim800.println(F("No target"));
 }
 } else {
  sim800.println(F("No target"));
 }

}

//**************************************************************************************************//
void setup() {
  Serial.begin(115200);
  Serial.println("esp32 serial initialize");
  
  sim800.begin(9600, SERIAL_8N1, rxPin, txPin);
  Serial.println("SIM800L serial initialize");

  //Restart takes quite some time. To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);  // not displaying

  //radar.debug(MONITOR_SERIAL); //Uncomment to show debug information from the library on the Serial Monitor. By default this does not show sensor reads as they are very frequent.
  //RADAR_SERIAL.begin(256000, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN); //UART for monitoring the radar
  delay(500);
  Serial.print(F("\nConnect LD2410 radar TX to GPIO:"));
  Serial.println(RADAR_RX_PIN);
  Serial.print(F("Connect LD2410 radar RX to GPIO:"));
  Serial.println(RADAR_TX_PIN);
  Serial.print(F("LD2410 radar sensor initialising: "));
  if(radar.begin(sim800))
  {
    Serial.println(F("OK"));
    Serial.print(F("LD2410 firmware version: "));
    Serial.print(radar.firmware_major_version);
    Serial.print('.');
    Serial.print(radar.firmware_minor_version);
    Serial.print('.');
    Serial.println(radar.firmware_bugfix_version, HEX);
  }
  else
  {
    Serial.println(F("not connected"));
  }
}
