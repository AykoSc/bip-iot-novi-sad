// LCD I2C 16x2
// LiquidCrystal_I2C Library
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x3F for physical Box or 0x27 for virtual Wokwi


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

// Custom character definitions
byte empty_TL[] = {0x00, 0x0E, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00};
byte empty_TR[] = {0x00, 0x0E, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00};
byte empty_BL[] = {0x00, 0x10, 0x10, 0x10, 0x10, 0x0E, 0x00, 0x00};
byte empty_BR[] = {0x00, 0x01, 0x01, 0x01, 0x01, 0x0E, 0x00, 0x00};

byte filled_TL[] = {0x00, 0x0E, 0x1E, 0x1E, 0x1E, 0x1E, 0x00, 0x00};
byte filled_TR[] = {0x00, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00};
byte filled_BL[] = {0x00, 0x1E, 0x1E, 0x1E, 0x1E, 0x0E, 0x00, 0x00};
byte filled_BR[] = {0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0E, 0x00, 0x00};

enum gameState
{
  st_idle,
  st_waiting,
  st_starting,
  st_reaction,
  st_score
};

gameState state = st_idle;
unsigned long mils_start_reaction_signal;
unsigned long mils_reaction_stimulus_shown;

// variables for keys
bool b1, b2, oldb1 = false, oldb2 = false, k1_pressed_event = false, k2_pressed_event = false;

// Method forward declarations
void keyLoop();
bool consumeK1PressEvent();
void scoreLoop();
void reactionLoop();
void startingLoop();
void waitingLoop();
void idleLoop();

void printEmptyCircle(const int startCol)
{
  lcd.setCursor(startCol, 0);
  lcd.write(0);
  lcd.write(1);

  lcd.setCursor(startCol, 1);
  lcd.write(2);
  lcd.write(3);
}

void printFilledCircle(const int startCol)
{
  lcd.setCursor(startCol, 0);
  lcd.write(4);
  lcd.write(5);

  lcd.setCursor(startCol, 1);
  lcd.write(6);
  lcd.write(7);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("BIPBox Reaction Speed Test Game\nWhen the buzzer sounds and the LED lights up, press the left key.");

  // keys
  pinMode(KEY1, INPUT); // Pullup resistors on the PCB, no need for INPUT_PULLUP
  pinMode(KEY2, INPUT); // and pullup resistors don't exist on pins 36 and 39 anyway

  // buzzer
  pinMode(BUZZER, OUTPUT);
  tone(BUZZER, 800, 250); // beep

  // Print a message to the LCD
  lcd.init(); // initialize the lcd
  lcd.backlight();

  // Create custom characters
  lcd.createChar(0, empty_TL);
  lcd.createChar(1, empty_TR);
  lcd.createChar(2, empty_BL);
  lcd.createChar(3, empty_BR);
  lcd.createChar(4, filled_TL);
  lcd.createChar(5, filled_TR);
  lcd.createChar(6, filled_BL);
  lcd.createChar(7, filled_BR);

  pinMode(LED1, OUTPUT);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED3, LOW);
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

  // Check for a press event for KEY1
  if (b1 && !oldb1)
  {
    k1_pressed_event = true;
  }
  // Check for a press event for KEY2
  if (b2 && !oldb2)
  {
    k2_pressed_event = true;
  }
  oldb1 = b1;
  oldb2 = b2;
}

// Call this to check if K1 was pressed SINCE THE LAST CALL TO THIS FUNCTION.
// It consumes the event.
bool consumeK1PressEvent()
{
  if (k1_pressed_event)
  {
    k1_pressed_event = false; // Consume
    return true;
  }
  return false;
}

void scoreLoop()
{
  if (consumeK1PressEvent())
  {
    digitalWrite(LED1, LOW);
    digitalWrite(LED3, LOW);
    state = st_idle;
  }
}

void reactionLoop()
{
  if (consumeK1PressEvent())
  {
    const unsigned long mils_now = millis();
    const unsigned long score = mils_now - mils_reaction_stimulus_shown;
    digitalWrite(LED3, LOW); // Turn off reaction LED

    lcd.clear();
    lcd.print("Your reaction:");
    lcd.setCursor(0, 1);
    lcd.print(score);
    lcd.print(" ms");

    // Display score for a bit, then prompt
    delay(1500);
    lcd.setCursor(0, 1);
    lcd.clear();
    lcd.print("Press A for new Race!");
    state = st_score;
  }
}

void startingLoop()
{
  static bool ledBlinkState = false;
  static unsigned long lastBlinkMils = 0;

  if (millis() < mils_start_reaction_signal)
  {
    if (consumeK1PressEvent())
    {
      // Pressed the key too early - fail!
      digitalWrite(LED1, LOW); // turn off the blinking LED
      lcd.clear();
      lcd.print("False Start!");
      lcd.setCursor(0, 1);
      lcd.print("-- PENALTY --");
      tone(BUZZER, 300, 500);
      delay(1000); // Increased delay to make "Too early" more visible
      state = st_score;
      return;
    }

    // Blink LED1 while waiting
    if (millis() - lastBlinkMils > 200)
    {
      ledBlinkState = !ledBlinkState;
      digitalWrite(LED1, ledBlinkState);
      lastBlinkMils = millis();
    }
    return;
  }

  // --- Time for reaction signal! ---
  digitalWrite(LED1, LOW);
  digitalWrite(LED3, HIGH);
  tone(BUZZER, 800, 1000);
  mils_reaction_stimulus_shown = millis();
  lcd.clear();
  // lcd.print("!!! PRESS A !!!"); // User should react NOW
  // printing 5 empty circles
  printEmptyCircle(1);
  printEmptyCircle(3 + 1);
  printEmptyCircle(5 + 2);
  printEmptyCircle(7 + 3);
  printEmptyCircle(9 + 4);

  state = st_reaction;
}

void waitingLoop()
{
  if (consumeK1PressEvent())
  {
    lcd.clear();
    lcd.print("Drivers, Ready!");
    lcd.setCursor(0, 1);
    lcd.print("Wait for the GO!");

    delay(1000);

    lcd.clear();

    // Do F1 traffic light simulation (F1 is doing this every about 1000ms).
    // Also make a sound each time.
    printEmptyCircle(1);
    printEmptyCircle(3 + 1);
    printEmptyCircle(5 + 2);
    printEmptyCircle(7 + 3);
    printEmptyCircle(9 + 4);
    delay(1000);
    tone(BUZZER, 300, 250);
    printFilledCircle(1);
    delay(1000);
    tone(BUZZER, 300, 250);
    printFilledCircle(3 + 1);

    delay(1000);
    tone(BUZZER, 300, 250);
    printFilledCircle(5 + 2);

    delay(1000);
    tone(BUZZER, 300, 250);
    printFilledCircle(7 + 3);

    delay(1000);
    tone(BUZZER, 300, 250);
    printFilledCircle(9 + 4);

    delay(10);

    // Random delay in F1 start clock is usually between 0.5 and 5 seconds
    mils_start_reaction_signal = millis() + random(500, 5000);

    digitalWrite(LED1, LOW);
    state = st_starting;
  }
}

void idleLoop()
{
  lcd.clear();
  lcd.print("F1 Start Clock Game");
  lcd.setCursor(0, 1);
  lcd.print("Push A to start!");

  state = st_waiting;
}
