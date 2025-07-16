#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <ADXL362.h>

#define SEALEVELPRESSURE_HPA (1013.25)

// Pin definitions
#define I2C_SDA 8
#define I2C_SCL 9
#define SPI_CS_ADXL 10
#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_SCK 13

// Sensor objects
Adafruit_BME280 bme;
ADXL362 xl;

// Sensor data variables
float bme_temperature, bme_humidity, bme_pressure, bme_altitude;
int16_t adxl_x, adxl_y, adxl_z, adxl_temperature;

void setup() {
  Serial.begin(115200);
  delay(100);

  // Initialize I2C for BME280
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!bme.begin(0x76, &Wire)) {
    Serial.println("BME280 not found!");
    while (1);
    
  } else {
    Serial.println("BME280 initialized");
  }

  // Initialize SPI for ADXL362
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS_ADXL);
  xl.begin(SPI_CS_ADXL);  // Setup SPI protocol, issue device soft reset
  xl.beginMeasure();       // Switch ADXL362 to measure mode
  Serial.println("ADXL362 initialized.");
}

void loop() {

  // Read ADXL362 data for serial output
  xl.readXYZTData(adxl_x, adxl_y, adxl_z, adxl_temperature);
  Serial.print("XVALUE="); Serial.print(adxl_x);
  Serial.print("\tYVALUE="); Serial.print(adxl_y);
  Serial.print("\tZVALUE="); Serial.print(adxl_z);
  Serial.print("\tTEMPERATURE="); Serial.println(adxl_temperature);
  
  printValues();
  delay(2000);  // Short delay for serial readability
}

void printValues() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}