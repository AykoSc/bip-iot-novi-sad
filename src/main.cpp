#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobot_BMI160.h>

// --- Pin Definitions (matching your BIPBox and diagram) ---
#define KEY1_PIN 39         // Connected to ESP32's VN (often GPIO39)
#define POTENTIOMETER_PIN 35

// --- Component Objects ---
// LCD I2C 16x2 - Address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// BMI160 - Address 0x69
DFRobot_BMI160 bmi160;
const int8_t BMI160_I2C_ADDRESS = 0x69;

// --- Variables to store sensor data and states ---
int potValue = 0;
bool buttonPressed = false;
bool lastButtonState = false;

int16_t accelData[3] = {0}; // ax, ay, az
int16_t gyroData[3] = {0}; // gx, gy, gz

unsigned long lastBmiReadTime = 0;
const unsigned long bmiReadInterval = 500; // Read BMI160 every 500ms

// --- Helper function to print to LCD and clear previous content on that line ---
void printLcdLine(int row, String text)
{
    lcd.setCursor(0, row);
    lcd.print("                "); // Clear the line (16 spaces)
    lcd.setCursor(0, row);
    lcd.print(text);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("BIPBox Combined Test Sketch");

    // Initialize LCD
    lcd.init();
    lcd.backlight();
    printLcdLine(0, "Initializing...");

    // Initialize BMI160
    if (bmi160.softReset() != BMI160_OK)
    {
        Serial.println("BMI160 reset failed!");
        printLcdLine(1, "BMI160 RST Fail");
        while (1);
    }
    if (bmi160.I2cInit(BMI160_I2C_ADDRESS) != BMI160_OK)
    {
        Serial.println("BMI160 I2C init failed!");
        printLcdLine(1, "BMI160 I2C Fail");
        while (1);
    }
    Serial.println("BMI160 Initialized");

    // Initialize Pin Modes
    pinMode(KEY1_PIN, INPUT); // Using external pull-up, so just INPUT
    pinMode(POTENTIOMETER_PIN, INPUT);

    printLcdLine(0, "Pot: ---- Btn: -");
    printLcdLine(1, "Ax: ---- Ay:----"); // Placeholder for Accel/Gyro
    delay(1000); // Show init messages
}

void loop()
{
    // --- Read Potentiometer ---
    int currentPotValue = analogRead(POTENTIOMETER_PIN);
    if (abs(currentPotValue - potValue) > 20)
    {
        // Update if changed significantly
        potValue = currentPotValue;
        String potStr = "Pot:" + String(potValue);
        // To combine with button status, we'll update the full line
        String line0 = potStr;
        for (int i = potStr.length(); i < 10; i++) line0 += " "; // Padding
        line0 += "Btn:" + String(buttonPressed ? "P" : "-");
        printLcdLine(0, line0);
    }

    // --- Read Button (KEY1) ---
    // Assumes external pull-up resistor, so LOW when pressed
    bool currentButtonState = (digitalRead(KEY1_PIN) == LOW);
    if (currentButtonState != lastButtonState)
    {
        if (currentButtonState)
        {
            // Button just pressed
            buttonPressed = true;
            Serial.println("Button PRESSED");
        }
        else
        {
            // Button just released
            buttonPressed = false;
            Serial.println("Button RELEASED");
        }
        // Update LCD line 0 with new button state
        String potStr = "Pot:" + String(potValue);
        String line0 = potStr;
        for (int i = potStr.length(); i < 10; i++) line0 += " "; // Padding
        line0 += "Btn:" + String(buttonPressed ? "P" : "-");
        printLcdLine(0, line0);
        lastButtonState = currentButtonState;
    }


    // --- Read BMI160 periodically ---
    if (millis() - lastBmiReadTime > bmiReadInterval)
    {
        lastBmiReadTime = millis();
        int16_t sensorData[6] = {0}; // gx, gy, gz, ax, ay, az

        if (bmi160.getAccelGyroData(sensorData) == BMI160_OK)
        {
            gyroData[0] = sensorData[0]; // gx
            gyroData[1] = sensorData[1]; // gy
            gyroData[2] = sensorData[2]; // gz
            accelData[0] = sensorData[3]; // ax
            accelData[1] = sensorData[4]; // ay
            accelData[2] = sensorData[5]; // az

            Serial.print("Accel X: ");
            Serial.print(accelData[0]);
            Serial.print(" Y: ");
            Serial.print(accelData[1]);
            Serial.print(" Z: ");
            Serial.println(accelData[2]);
            Serial.print("Gyro  X: ");
            Serial.print(gyroData[0]);
            Serial.print(" Y: ");
            Serial.print(gyroData[1]);
            Serial.print(" Z: ");
            Serial.println(gyroData[2]);

            // Display first two accel values on LCD line 1
            // You can choose which values are most interesting to display
            String accelStr = "Ax:" + String(accelData[0]) + " Ay:" + String(accelData[1]);
            printLcdLine(1, accelStr);
        }
        else
        {
            Serial.println("Failed to get BMI160 data");
            printLcdLine(1, "BMI Read Fail");
        }
    }

    delay(50); // Small delay to keep things responsive but not overwhelm
}
