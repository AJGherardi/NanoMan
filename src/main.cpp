#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

const int sdCS = 10; // TODO: Set CS pin for SD card

unsigned long previousTime = 0;
unsigned long currentTime = 0;

String dataBuffer;
File dataFile;
const char filename[] = "datalog.txt";

void setup()
{
  // TODO: Initialize sensor
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
    int16_t value = 32767; // TODO: Get value from sensor
    // Add to buffer
    dataBuffer += value;
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