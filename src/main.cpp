// LCD I2C 16x2
// LiquidCrystal_I2C Library
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x3F or 0x27 for a 16 chars and 2 line display

// BIPBox pin assignments
#define KEY1 39
#define KEY2 36
#define PIR 23
#define DHT 19
#define PHOTORESISTOR 34
#define POTENTIOMETER 35
#define LED1 16
#define LED2 17
#define LED3 18
#define BUZZER 33
#define SERVO 32

enum gameState
{
  st_idle,
  st_waiting,
  st_starting,
  st_reaction,
  st_score
};

enum gameState state = st_waiting;
unsigned long mils_start;
unsigned long mils_reaction;

// variables for keys
bool b1, b2, oldb1 = false, oldb2 = false, k1 = false, k2 = false;

// Method forward declarations
void keyLoop();
bool readK1();
void scoreLoop();
void reactionLoop();
void startingLoop();
void waitingLoop();
void idleLoop();

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("BIPBox Reaction Speed Test Game\nWhen the buzzer sounds and the LED lights up, press the left key.");
  // keys
  pinMode(KEY1, INPUT); // Pullup resistors on the PCB, no need for INPUT_PULLUP
  pinMode(KEY2, INPUT); // and pullup resistors don't exist on pins 36 and 39 anyway
  // buzzer
  pinMode(33, OUTPUT);
  tone(33, 800, 250); // beep
  // Print a message to the LCD.s
  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Reaction Game");
  lcd.setCursor(0, 1);
  lcd.print("Wait for sound");
}

void loop()
{
  // put your main code here, to run repeatedly:
  keyLoop();
  switch (state)
  {
  case st_idle:
    idleLoop();
    break;
  case st_waiting:
    waitingLoop();
    break;
  case st_starting:
    startingLoop();
    break;
  case st_reaction:
    reactionLoop();
    break;
  case st_score:
    scoreLoop();
    break;
  }
}

void keyLoop()
{
  // ---- KEYS ----
  b1 = !digitalRead(KEY1);
  b2 = !digitalRead(KEY2);
  if (b1 && (b1 != oldb1))
    k1 = true;
  else
    k1 = false;
  if (b2 && (b2 != oldb2))
    k2 = true;
  else
    k2 = false;
  oldb1 = b1;
  oldb2 = b2;
}

bool readK1()
{
  if (k1)
  {
    k1 = false;
    return true;
  }
  else
  {
    return false;
  }
}

void scoreLoop()
{
  if (readK1())
  {
    lcd.clear();
    delay(250);
    state = st_idle;
  }
}

void reactionLoop()
{
  if (readK1())
  {
    unsigned long mils_now = millis();
    unsigned long score = mils_now - mils_reaction;
    lcd.clear();
    lcd.print("Reaction time:");
    lcd.setCursor(2, 1);
    lcd.print(score);
    delay(750);
    lcd.setCursor(9, 1);
    lcd.print("Press A");
    state = st_score;
  }
}

void startingLoop()
{
  if (mils_start - millis() > 0)
  {
    if (readK1())
    {
      // pressed the key too early - fail!
      lcd.clear();
      lcd.print("Too early!");
      lcd.setCursor(0, 1);
      lcd.print("--FAIL--");
      tone(33, 300, 500);
      delay(500);
      state = st_score;
      return;
    }
    else
    {
      return;
    }
  }
  tone(33, 800, 250);
  mils_reaction = millis();
  state = st_reaction;
}

void waitingLoop()
{
  if (readK1())
  {
    lcd.clear();
    lcd.print("Press A on sound!");
    state = st_starting;
    mils_start = millis() + random(1000, 5000); // pick the time 1s to 5s from current
    // INTRODUCE SMALL DELAY TO ACCOUNT FOR KEY BOUNCE!
    delay(100);
  }
}

void idleLoop()
{
  lcd.clear();
  lcd.print("Push A to start");
  state = st_waiting;
}
