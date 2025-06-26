// --- Libraries ---
// #include <driver/i2s.h>       // For the microphone (not used yet, but fine to keep for future use)
#include <Wire.h>               // For I2C communication
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include "ADXL362.h"            // Your custom library for the ADXL362

// --- Pin Definitions ---

// I2C Pins (for BME280)
#define I2C_SDA 8
#define I2C_SCL 9

// SPI Pins (for ADXL362)
// FIX: The ADXL362 needs a Chip Select (CS) pin. The SPI library manages MISO, MOSI, and SCLK.
#define ADXL362_CS_PIN 10 // Let's use GPIO 10 for Chip Select

// --- Sensor Objects & Configuration ---
Adafruit_BME280 bme; // FIX: Uncommented this line
ADXL362 xl;

// --- Timing Control ---
// IMPROVEMENT: Use one timer to control when we read from all sensors.
unsigned long lastReadTime = 0;
const long readInterval = 2000; // Read all sensors every 2 seconds (2000 ms)

void setup() {
    // 1. Start Serial Communication
    Serial.begin(115200);
    while (!Serial); // Wait for Serial to be ready, good practice for some boards
    Serial.println("Beehive Monitor Startup...");

    // 2. Initialize I2C Bus and BME280 Sensor
    Wire.begin(I2C_SDA, I2C_SCL);
    Serial.println("I2C bus initialized.");

    if (!bme.begin(0x76)) { // 0x76 is the default I2C address
        Serial.println("FATAL: Could not find a valid BME280 sensor, check wiring!");
        while (1) delay(10); // Halt execution
    }
    Serial.println("BME280 sensor found and initialized.");

    // 3. Initialize SPI Bus and ADXL362 Sensor
    // The SPI bus is initialized automatically by the ADXL362 library's begin() call
    // FIX: Pass the correct Chip Select pin to the begin() function.
    xl.begin(ADXL362_CS_PIN);
    xl.beginMeasure(); // Switch ADXL362 to measurement mode
    Serial.println("ADXL362 sensor found and initialized.");

    Serial.println("\nSetup complete. Starting measurements...");
    delay(500);
}

void loop() {
    // IMPROVEMENT: Use one non-blocking timer to read and print all sensor data.
    if (millis() - lastReadTime > readInterval) {
        readAndPrintSensorData();
        lastReadTime = millis();
    }
}

// IMPROVEMENT: A single function to handle reading and printing for all sensors.
void readAndPrintSensorData() {
    Serial.println("\n---------------------------------");
    Serial.print("Timestamp: ");
    Serial.print(millis() / 1000);
    Serial.println(" seconds");
    Serial.println("---------------------------------");

    // --- Read and Print BME280 Data ---
    Serial.println("BME280 (Environment):");
    Serial.print("  Temperature: ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("  Pressure:    ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("  Humidity:    ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    // --- Read and Print ADXL362 Data ---
    int16_t x, y, z, temp;
    // read all three axis in burst to ensure all measurements correspond to same sample time
    xl.readXYZTData(x, y, z, temp);

    Serial.println("\nADXL362 (Vibration & Temp):");
    Serial.print("  X-Axis: ");
    Serial.print(x);
    Serial.print("\t  Y-Axis: ");
    Serial.print(y);
    Serial.print("\t  Z-Axis: ");
    Serial.println(z);
    Serial.print("  Sensor Temp: ");
    Serial.println(temp); // Assuming your library provides temperature
    Serial.println("---------------------------------\n");
}