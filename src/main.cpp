#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "SparkFun_LIS331.h"

LIS331 xl;

const int sdCS = 10; // TODO: Set CS pin for SD card

unsigned long previousTime = 0;
unsigned long currentTime = 0;

String dataBuffer;
File dataFile;
const char filename[] = "datalog.txt";

void setup()
{
  // Initialize sensor
  Wire.begin();
  xl.setI2CAddr(0x19);
  xl.begin(LIS331::USE_I2C);
  xl.setPowerMode(LIS331::NORMAL);
  xl.setFullScale(LIS331::MED_RANGE);
  xl.setODR(LIS331::DR_100HZ);
  // TODO: Initialize SD card
  Serial.begin(38400);
  // Open file
  dataFile = SD.open(filename, FILE_WRITE);
  if (!dataFile)
  {
    Serial.print("Error Opening File");
    while (1)
      ;
  }
  // Save template for spreadsheet
  dataFile.println("millis,accel\n");
  // Set initial time
  previousTime = micros();
}

void loop()
{
  // Get time
  unsigned long now = micros();
  // Get time since last sample
  unsigned long timeElapsed = currentTime - previousTime;
  // Ensure 10 milliseconds has passed since last sample
  if (timeElapsed >= 10000)
  {
    // Record value
    int16_t x, y,z ; 
    xl.readAxes(x, y, z);
    float xG = xl.convertToG(200, x);
    // Add to buffer
    dataBuffer += xG;
    dataBuffer += ",";
    dataBuffer += timeElapsed;
    dataBuffer += "\n"; // This creates a new line
    // Mark sample time
    previousTime = now;
    Serial.println(dataBuffer);
    dataBuffer = ""; // TODO:: Remove currently being zeroed to allow prints to serial console
  }

  // Write to file if it is available
  unsigned int chunkSize = dataFile.availableForWrite();
  if (chunkSize && dataBuffer.length() >= chunkSize)
  {
    // Write to file
    dataFile.write(dataBuffer.c_str(), chunkSize);
    // Remove data from buffer
    dataBuffer.remove(0, chunkSize);
  }
}