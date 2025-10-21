#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into pin D2 on the WeMos D1 Mini
#define ONE_WIRE_BUS D5
// Setup a oneWire instance to communicate with any OneWiredevices
OneWire oneWire(ONE_WIRE_BUS);
// Pass the oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);
// Set the LCD address to 0x3F for a 16 chars and 2 linedisplay
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup(void)
{
// Start serial communication
Serial.begin(115200);
Serial.println
("Dallas Temperature Control Library Demo");
// Start up the library
sensors.begin();
// Initialize the LCD
lcd.begin();
// Turn on the backlight
lcd.backlight();
lcd.print(" WELCOME ");
lcd.setCursor(0, 1);
lcd.print(" TO ");
delay(3000);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("PSMURO");
lcd.setCursor(0, 1);
lcd.print("UNIV.GUNADARMA");
delay(5000);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("MCU HEALTHCARE");
delay(3000);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Sistem monitor");
lcd.setCursor(0, 1);
lcd.print("Suhu Badan");
delay(5000);
lcd.clear();
lcd.print("Temperature:");
}
void loop(void)
{
// Request temperature readings
sensors.requestTemperatures();
// Read temperature in Celsius
float tempC =
sensors.getTempCByIndex(0);
// Print temperature to serial monitor
Serial.print("Temperature is: ");
Serial.print(tempC);
Serial.println("°C");
// Print temperature to LCD
lcd.setCursor(0, 1); // Set cursor to second line
lcd.print(tempC);
lcd.print(" C");
delay(1000);
}
lcd.print("PSMURO");
lcd.setCursor(0, 1);
lcd.print("UNIV.GUNADARMA");
delay(5000);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("MCU HEALTHCARE");
lcd.setCursor(0, 1);
lcd.print("HEART RATE");
delay(5000);
lcd.clear();
// Attempt to initialize the MAX30105 sensor. Check for asuccessful connection and report.
if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
{
// Start communication using fast I2C speed
Serial.println("MAX30102 was not found. Please check
wiring/power. ");
while (1); // Infinite loop to halt further execution ifsensor is not found
}
Serial.println("Place your index finger on the sensor with
steady pressure.");
particleSensor.setup();
// Configure sensor with default settings for heart ratemonitoring
particleSensor.setPulseAmplitudeRed(0x0A);
// Set the red LED pulse amplitude (intensity) to a low valueas an indicator
particleSensor.setPulseAmplitudeGreen(0);
// Turn off the green LED as it's not used here
}
void loop()
{
long irValue = particleSensor.getIR();
// Read the infrared value from the sensor
if (checkForBeat(irValue) == true)
{
// Check if a heart beat is detected
long delta = millis() - lastBeat;
// Calculate the time between the current and last beat
lastBeat = millis(); // Update lastBeat to the current time
beatsPerMinute = 60 / (delta / 1000.0);
// Calculate BPM
// Ensure BPM is within a reasonable range before updatingthe rates array
if (beatsPerMinute < 255 && beatsPerMinute > 20)
{
rates[rateSpot++] = (byte)beatsPerMinute;
// Store this reading in the rates array
rateSpot %= RATE_SIZE;
// Wrap the rateSpot index to keep it within the boundsof the rates array
// Compute the average of stored heart rates to smooth outthe BPM
beatAvg = 0;
for (byte x = 0 ; x < RATE_SIZE ; x++)
beatAvg += rates[x];
beatAvg /= RATE_SIZE;
}
}
Serial.print("IR=");
Serial.print(irValue);
Serial.print(", BPM=");
Serial.print(beatsPerMinute);
Serial.print(", Avg BPM=");
Serial.print(beatAvg);
if (irValue < 50000)
{
Serial.print(" No finger?");
lcd.setCursor(0, 0);
lcd.print("No Finger ? ");
lcd.setCursor(0, 1);
lcd.print("Letakan Jari! ");
}
else
{
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("BPM: ");
lcd.print(beatsPerMinute);
lcd.setCursor(0, 1);
lcd.print("Avg BPM: ");
lcd.print(beatAvg);
}
Serial.println();
//delay(500); // Delay to update the displayand avoid flickering
}
