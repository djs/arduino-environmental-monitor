#include <Arduino.h>
 
#include <dht.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"

RTC_DS3231 rtc;
dht DHT;

#define DHT11_PIN 4

const int chipSelect = 9;

String print_time(DateTime timestamp) {
  char message[120];

  int Year = timestamp.year();
  int Month = timestamp.month();
  int Day = timestamp.day();
  int Hour = timestamp.hour();
  int Minute = timestamp.minute();
  int Second= timestamp.second();

  sprintf(message, "%d-%d-%d %02d:%02d:%02d", Month,Day,Year,Hour,Minute,Second);
  
  return message;
}

void setup()
{
  Serial.begin(115200);
  
  Serial.print("DHT LIBRARY VERSION: ");
  Serial.println(DHT_LIB_VERSION);
  Serial.println();

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");
}

void loop()
{
  String dataString = "";

  // READ DATA
  Serial.print("DHT11, \t");
  DateTime now = rtc.now();
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
    Serial.print("OK,\t"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
    Serial.print("Checksum error,\t"); 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
    Serial.print("Time out error,\t"); 
    break;
    case DHTLIB_ERROR_CONNECT:
        Serial.print("Connect error,\t");
        break;
    case DHTLIB_ERROR_ACK_L:
        Serial.print("Ack Low error,\t");
        break;
    case DHTLIB_ERROR_ACK_H:
        Serial.print("Ack High error,\t");
        break;
    default: 
    Serial.print("Unknown error,\t"); 
    break;
  }
  // DISPLAY DATA
  Serial.print(DHT.humidity, 1);
  Serial.print(",\t");
  Serial.println(DHT.temperature, 1);
  dataString = print_time(now) + "," + String(DHT.humidity) + "," + String(DHT.temperature) + "\n";

  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

  delay(2000);
}
