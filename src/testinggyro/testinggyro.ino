// Standalone sketch for ADXL362, configured for high-sensitivity beehive monitoring.
// This version is specifically for the provided "annem/ADXL362" library files.

#include <SPI.h>
#include "ADXL362.h" // Using your provided library files

// Define the Chip Select pin for your ADXL362
#define SPI_CS_ADXL 10
#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_SCK 13
// Create an instance of the sensor library
ADXL362 xl;

void setup() {
    // 1. Start Serial Communication
    Serial.begin(115200);
    while (!Serial);
    Serial.println("High-Sensitivity Beehive Monitor Startup...");

    // 2. Initialize the ADXL362 using its begin() function
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS_ADXL);
    xl.begin(SPI_CS_ADXL);  // Setup SPI protocol, issue device soft reset
    xl.beginMeasure();      // Switch ADXL362 to measure mode
   Serial.println("ADXL362 initialized.");
    // --- MANUALLY CONFIGURE SENSOR REGISTERS ---
    // We do this because this library version lacks high-level functions like setRange().

    // Configure FILTER_CTL Register (Address 0x2C)
    // Set Range to +/- 2g, Bandwidth to ODR/4, and ODR to 100Hz
    // Value = 0b00010011 = 0x13
    Serial.println("Setting Range, ODR, and Filter...");
    xl.SPIwriteOneRegister(0x2C, 0x13);
    delay(10); // Small delay after writing

    // Configure POWER_CTL Register (Address 0x2D)
    // Set to Low Noise Mode and Measurement Mode
    // Value = 0b00010010 = 0x12
    Serial.println("Setting Low Noise and Measurement Mode...");
    xl.SPIwriteOneRegister(0x2D, 0x12);
    delay(10); // Small delay after writing


    // NOTE: We do NOT call xl.beginMeasure() anymore, because our write
    // to the POWER_CTL register already put the device in measurement mode.

    Serial.println("\nConfiguration complete. Starting measurements...");
}

void loop() {
    // Reading data every second for this example
    delay(1000); 

    int16_t x, y, z, temp;
    
    // Read all three axes and temperature data
  // Read ADXL362 data.
  xl.readXYZTData(x, y, z, temp);

  // To use the Serial Plotter, print each value separated by a tab ("\t").
  // Use println() only on the very last value to signal a new data set for the plot.
  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.println(z); // This plots X, Y, and Z as three separate lines.

  // The printValues() function is commented out because its text-based
  // output would interfere with the Serial Plotter's graph.
  // printValues();

  // A shorter delay provides a more responsive plot.
  delay(200);
}