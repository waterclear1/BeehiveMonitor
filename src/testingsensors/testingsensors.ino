#include <WiFi.h>
#include <WebServer.h>
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

// WiFi credentials
const char* ssid = "aaa";  // Enter SSID here
const char* password = "123";  // Enter Password here

WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(100);

  // Initialize I2C for BME280
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!bme.begin(0x76, &Wire)) {
    Serial.println("BME280 not found!");
    while (1);
  }

  // Initialize SPI for ADXL362
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS_ADXL);
  xl.begin(SPI_CS_ADXL);  // Setup SPI protocol, issue device soft reset
  xl.beginMeasure();      // Switch ADXL362 to measure mode
  Serial.println("ADXL362 initialized.");

  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: "); Serial.println(WiFi.localIP());

  // Setup web server routes
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  // Read ADXL362 data for serial output
  xl.readXYZTData(adxl_x, adxl_y, adxl_z, adxl_temperature);
  Serial.print("XVALUE="); Serial.print(adxl_x);
  Serial.print("\tYVALUE="); Serial.print(adxl_y);
  Serial.print("\tZVALUE="); Serial.print(adxl_z);
  Serial.print("\tTEMPERATURE="); Serial.println(adxl_temperature);
  delay(100);  // Short delay for serial readability
}

void handle_OnConnect() {
  // Read BME280 data
  bme_temperature = bme.readTemperature();
  bme_humidity = bme.readHumidity();
  bme_pressure = bme.readPressure() / 100.0F;
  bme_altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  // Read ADXL362 data for web display
  xl.readXYZTData(adxl_x, adxl_y, adxl_z, adxl_temperature);

  // Send HTML response
  server.send(200, "text/html", SendHTML(bme_temperature, bme_humidity, bme_pressure, bme_altitude, adxl_x, adxl_y, adxl_z, adxl_temperature));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float temp, float hum, float press, float alt, int16_t x, int16_t y, int16_t z, int16_t temp_adxl) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Beehive Monitor</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>Beehive Monitor</h1>\n";
  ptr += "<p>Temperature (BME280): "; ptr += temp; ptr += "&deg;C</p>";
  ptr += "<p>Humidity (BME280): "; ptr += hum; ptr += "%</p>";
  ptr += "<p>Pressure (BME280): "; ptr += press; ptr += "hPa</p>";
  ptr += "<p>Altitude (BME280): "; ptr += alt; ptr += "m</p>";
  ptr += "<p>X-Axis (ADXL362): "; ptr += x; ptr += "</p>";
  ptr += "<p>Y-Axis (ADXL362): "; ptr += y; ptr += "</p>";
  ptr += "<p>Z-Axis (ADXL362): "; ptr += z; ptr += "</p>";
  ptr += "<p>Temperature (ADXL362): "; ptr += temp_adxl; ptr += "</p>";
  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}