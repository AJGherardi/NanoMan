#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
// #include <Wire.h>
// #include "SparkFun_LIS331.h"

// LIS331 xl;

const int sdCS = 9;
const int sdCD = 8;
unsigned long count = 0;
unsigned long previousTime = 0;
unsigned long currentTime = 0;

String dataBuffer;
File dataFile;
const char filename[] = "data.txt";

void setup()
{
  // Initialize sensor
  // Wire.begin();
  // xl.setI2CAddr(0x19);
  // xl.begin(LIS331::USE_I2C);
  // xl.setPowerMode(LIS331::NORMAL);
  // xl.setFullScale(LIS331::MED_RANGE);
  // xl.setODR(LIS331::DR_100HZ);
  // Initialize SD card
  pinMode(sdCD, INPUT);
  pinMode(sdCS, OUTPUT);
  // Wait for SD card to be detected
  while (!digitalRead(sdCD))
    ;
  delay(250); // 'Debounce insertion'

  if (!SD.begin(sdCS))
  {
    while (true)
      ;
  }
  // Initlize data buffer
  dataBuffer.reserve(1536);

  SD.remove(filename);
  delay(200);
  // Open file
  dataFile = SD.open(filename, FILE_WRITE);
  if (!dataFile)
  {
    // Serial.print("Error Opening File");
    while (1)
      ;
  }
  // Leave a second for sd card to be ready
  delay(1000);
  // Set initial time
  previousTime = micros();
}

void loop()
{
  // Get time
  unsigned long currentTime = micros();
  // Get time since last sample
  unsigned long timeElapsed = currentTime - previousTime;
  // Ensure 10 milliseconds has passed since last sample
  if (timeElapsed >= 12000)
  {
    // Record value
    int16_t x, y, z;
    // xl.readAxes(x, y, z);
    // float xG = xl.convertToG(200, x);
    // Add to buffer
    dataBuffer += String(count, DEC);
    dataBuffer += ",";
    dataBuffer += String(currentTime, DEC);
    dataBuffer += ",";
    dataBuffer += dataBuffer.length();
    dataBuffer += ",123456789";
    dataBuffer += "\n"; // This creates a new line
    // Mark sample time
    previousTime = currentTime;
    count++;
  }

  // Write to file if it is available
  unsigned int chunkSize = dataFile.availableForWrite();
  if (chunkSize && dataBuffer.length() >= chunkSize)
  {
    // Write to file
    dataFile.write(dataBuffer.c_str(), chunkSize);
    dataFile.flush();
    // Remove data from buffer
    dataBuffer.remove(0, chunkSize);
  }
}