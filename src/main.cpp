#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// --- Pin Definitions ---
#define KEY1_PIN 39
#define POTENTIOMETER_PIN 35

// --- Component Objects ---
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_MPU6050 mpu;

// --- Variables ---
int potValue = 0;
bool buttonPressed = false;
bool lastButtonState = false;

sensors_event_t accelEvent, gyroEvent, tempEvent; // MPU6050 provides temperature too

unsigned long lastMpuReadTime = 0;
const unsigned long mpuReadInterval = 500;

// --- Helper function to print to LCD ---
void printLcdLine(int row, String text)
{
    lcd.setCursor(0, row);
    lcd.print("                "); // Clear line
    lcd.setCursor(0, row);
    lcd.print(text);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("BIPBox MPU6050 Test Sketch");

    lcd.init();
    lcd.backlight();
    printLcdLine(0, "Initializing...");

    // Initialize MPU6050
    if (!mpu.begin())
    {
        // Default address 0x68
        Serial.println("Failed to find MPU6050 chip");
        printLcdLine(1, "MPU6050 Fail");
        while (1)
        {
            delay(10);
        }
    }
    Serial.println("MPU6050 Found!");
    // Optional: Set accelerometer and gyro ranges if needed
    // mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    // mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    // mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    pinMode(KEY1_PIN, INPUT);
    pinMode(POTENTIOMETER_PIN, INPUT);

    printLcdLine(0, "Pot: ---- Btn: -");
    printLcdLine(1, "Ax: ---- Ay:----");
    delay(1000);
}

void loop()
{
    int currentPotValue = analogRead(POTENTIOMETER_PIN);
    if (abs(currentPotValue - potValue) > 20)
    {
        potValue = currentPotValue;
        String potStr = "Pot:" + String(potValue);
        String line0 = potStr;
        for (int i = potStr.length(); i < 10; i++) line0 += " ";
        line0 += "Btn:" + String(buttonPressed ? "P" : "-");
        printLcdLine(0, line0);
    }

    bool currentButtonState = (digitalRead(KEY1_PIN) == LOW);
    if (currentButtonState != lastButtonState)
    {
        if (currentButtonState)
        {
            buttonPressed = true;
            Serial.println("Button PRESSED");
        }
        else
        {
            buttonPressed = false;
            Serial.println("Button RELEASED");
        }
        String potStr = "Pot:" + String(potValue);
        String line0 = potStr;
        for (int i = potStr.length(); i < 10; i++) line0 += " ";
        line0 += "Btn:" + String(buttonPressed ? "P" : "-");
        printLcdLine(0, line0);
        lastButtonState = currentButtonState;
    }

    if (millis() - lastMpuReadTime > mpuReadInterval)
    {
        lastMpuReadTime = millis();

        mpu.getEvent(&accelEvent, &gyroEvent, &tempEvent);

        Serial.print("Accel X: ");
        Serial.print(accelEvent.acceleration.x);
        Serial.print(" Y: ");
        Serial.print(accelEvent.acceleration.y);
        Serial.print(" Z: ");
        Serial.println(accelEvent.acceleration.z);
        Serial.print("Gyro  X: ");
        Serial.print(gyroEvent.gyro.x);
        Serial.print(" Y: ");
        Serial.print(gyroEvent.gyro.y);
        Serial.print(" Z: ");
        Serial.println(gyroEvent.gyro.z);

        // Display accel X and Y on LCD
        // Note: MPU6050 library returns floats, so format them if needed for display
        String accelStr = "Ax:" + String(accelEvent.acceleration.x, 1) + // 1 decimal place
            " Ay:" + String(accelEvent.acceleration.y, 1);
        printLcdLine(1, accelStr);
    }

    delay(50);
}
