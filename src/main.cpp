// LCD I2C 16x2
// LiquidCrystal_I2C Library
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Specify device
#define TOPIC_OUT "game/result1" // on the other device switch 1 and 2
#define TOPIC_IN "game/result2"

// NETWORKING SETUP
String client_id = "esp-"; // SET FOR EACH DEVICE INDIVIDUALLY
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>

IPAddress server(192, 168, 150, 250); //Set this to the address of your PC running a MOSQUITTO server!
String clientId;

const char* ssid = "DzienDobry";        // Enter your Wi-Fi name
const char* password = "";  // Enter Wi-Fi password

#define SINGLEPLAYER false
#define MULTIPLAYER true

bool gameMode = SINGLEPLAYER;
bool anotherPlayerJoined = false;
bool gameStartSignalReceived = false;
unsigned long opponentReactionTime = 0;
bool opponentReactionTimeRecieved = false;
unsigned long score = 0;

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
  st_score,
  st_connecting,
  st_multiplayer_waiting
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
void connectingLoop();
void multiplayerWaitingLoop();


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

WiFiClient ethClient;
PubSubClient client(ethClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  // Create a temporary buffer to hold the payload and null-terminate it
  char messagePayload[length + 1];
  memcpy(messagePayload, payload, length);
  messagePayload[length] = '\0'; // Null-terminate the string

  Serial.println(messagePayload); // Print the received string to verify

  // Handle different MQTT messages
  if (strcmp(topic, "game/join") == 0) {
    anotherPlayerJoined = true;
    lcd.clear();
    lcd.print("Player Joined!");
    lcd.setCursor(0,1);
    lcd.print("Ready to start?");
    if (state == st_connecting) {
      state = st_multiplayer_waiting;
    }
  } else if (strcmp(topic, "game/start") == 0) {
    char startTimeStr[length + 1];
    memcpy(startTimeStr, payload, length);
    startTimeStr[length] = '\0';
    unsigned long synchronizedStartTime = strtoul(startTimeStr, NULL, 10);
    
    // Calculate delay until synchronized start time
    long timeToWait = synchronizedStartTime - millis();
    if (timeToWait < 0) timeToWait = 0; // If somehow late, start immediately
    if (timeToWait > 1) timeToWait = 1;

    lcd.clear();
    lcd.print("Game Starts In:");
    lcd.setCursor(0,1);
    lcd.print(timeToWait / 1000); // Display seconds countdown

    // Delay until the synchronized start time
    unsigned long currentMils = millis();
    while (millis() < synchronizedStartTime) {
      // Optional: Update countdown on LCD
      if (millis() - currentMils > 1000) {
        currentMils = millis();
        lcd.setCursor(0,1);
        lcd.print((synchronizedStartTime - millis()) / 1000);
        lcd.print(" sec ");
      }
      client.loop(); // Keep MQTT connected
    }
    
    lcd.clear();
    lcd.print("Game Starting!");
    state = st_waiting;
    gameStartSignalReceived = true; // Ensure this is set for all devices
  } else if (strcmp(topic, TOPIC_IN) == 0) {
    // Handle reaction time
    opponentReactionTime = strtoul(messagePayload, NULL, 10);
    Serial.print("Converted opponent reaction time: ");
    Serial.println(opponentReactionTime);
    opponentReactionTimeRecieved = true;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("...Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("game/join", clientId.c_str());
      // ... and resubscribe
      client.subscribe("game/join");
      client.subscribe("game/start");
      client.subscribe(TOPIC_IN);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 second");
      // Wait 1 second before retrying
      delay(1000);
    }
  }
}

// NETWORKING SETUP OVER



void connectingLoop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (anotherPlayerJoined) {
    client.publish("game/join", clientId.c_str());
    lcd.clear();
    lcd.print("Player Found!");
    lcd.setCursor(0, 1);
    lcd.print("Press A to Start!");
    state = st_multiplayer_waiting;
  }
}

void multiplayerWaitingLoop() {
  client.loop();

  if (anotherPlayerJoined && consumeK1PressEvent()) {
    // This device initiates the game
    unsigned long proposedStartTime = millis() + 2000; // Propose to start in 2 seconds
    char startTimeStr[10];
    sprintf(startTimeStr, "%lu", proposedStartTime);
    client.publish("game/start", startTimeStr);
    lcd.clear();
    lcd.print("Game Initiated!");
    lcd.setCursor(0,1);
    lcd.print("Waiting for sync...");
    // No direct state change here. The callback will handle the state change based on the synchronized start time.
  }
  
  // The transition to st_waiting is now handled by the callback when 'game/start' is received
  // and the synchronized wait is complete.
  // This loop primarily waits for 'game/start' message.
  if (gameStartSignalReceived) {
    // This part should be removed, as the callback now directly transitions to st_waiting
    // after its synchronized delay. The `gameStartSignalReceived` is primarily for the callback
    // to know to proceed.
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  /*
   */

  Serial.println("BIPBox Reaction Speed Test Game\nWhen the buzzer sounds and the LED lights up, press the left key.");

  // keys
  pinMode(KEY1, INPUT); // Pullup resistors on the PCB, no need for INPUT_PULLUP
  pinMode(KEY2, INPUT); // and pullup resistors don't exist on pins 36 and 39 anyway

  // buzzer
  ledcAttach(BUZZER, NOTE_A, 7);
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

  lcd.clear();
  lcd.print("Connecting!");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("...Connecting to WiFi...");
  }
  Serial.print("Connected to the WiFi AP. My address is: ");
  Serial.println(WiFi.localIP());
  clientId = "esp32-client-" + String(WiFi.macAddress());
  Serial.println("The client connects to the MQTT broker\n");

  client.setServer(server, 1883);
  client.setCallback(callback);

  // Allow the hardware to sort itself out
  delay(200);

  lcd.clear();
  lcd.print("WiFi Connected!");
  delay(500);

  lcd.clear();
  lcd.print("F1 Game Start");
  lcd.setCursor(0, 1);
  lcd.print("A - SP, B - MP");
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

  case st_connecting:
    connectingLoop();
    break;

  case st_multiplayer_waiting:
    multiplayerWaitingLoop();
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

bool consumeK2PressEvent()
{
  if (k2_pressed_event)
  {
    k2_pressed_event = false; // Consume
    return true;
  }
  return false;
}

void scoreLoop() {
  if (gameMode == MULTIPLAYER) {
    // Continuously process MQTT messages while waiting for opponent's score
    client.loop(); // Crucial for receiving the message
    if (!opponentReactionTimeRecieved) {

      return; // Stay in scoreLoop until opponent's score arrives
    }

    // Both scores are available, display results
    lcd.clear();
    if (mils_reaction_stimulus_shown == 0) { // This indicates a false start by the current player
      lcd.print("You False Started!");
      lcd.setCursor(0, 1);
      if (opponentReactionTime == 99999) { // Opponent also false started
        lcd.print("Opponent also false!");
      } else {
        lcd.print("Opponent: ");
        lcd.print(opponentReactionTime);
        lcd.print("ms");
      }
    } else if (opponentReactionTime == 99999) { // Opponent false started, current player didn't
        lcd.print("YOU WON!");
        ledcWriteNote(BUZZER, NOTE_A, 6); delay(500);
        ledcWriteNote(BUZZER, NOTE_A, 6); delay(250);
        ledcWriteNote(BUZZER, NOTE_A, 6); delay(250);
        ledcWriteNote(BUZZER, NOTE_B, 6); delay(500);
        ledcWriteNote(BUZZER, NOTE_Cs, 7); delay(500);
        ledcWriteNote(BUZZER, NOTE_D, 7); delay(1000);
        lcd.setCursor(0,1);
        lcd.print("Opponent false started!");
        
    }
    else {
      // Both players finished, compare scores
      if (score < opponentReactionTime) {
        lcd.print("YOU WON!");
        ledcWriteNote(BUZZER, NOTE_A, 6); delay(500);
        ledcWriteNote(BUZZER, NOTE_A, 6); delay(250);
        ledcWriteNote(BUZZER, NOTE_A, 6); delay(250);
        ledcWriteNote(BUZZER, NOTE_B, 6); delay(500);
        ledcWriteNote(BUZZER, NOTE_Cs, 7); delay(500);
        ledcWriteNote(BUZZER, NOTE_D, 7); delay(1000);
        lcd.setCursor(0, 1);
        lcd.print(score);
        lcd.print(" vs ");
        lcd.print(opponentReactionTime);
      } else if (score > opponentReactionTime) {
        lcd.print("YOU LOST!");
        lcd.setCursor(0, 1);
        lcd.print(score);
        lcd.print(" vs ");
        lcd.print(opponentReactionTime);
      } else {
        lcd.print("IT'S A TIE!");
        lcd.setCursor(0, 1);
        lcd.print(score);
        lcd.print(" ms");
      }
    }
    
    // Reset flags and prepare for next game
    opponentReactionTimeRecieved = false;
    mils_reaction_stimulus_shown = 0; // Reset this for accurate false start detection next round
    score = 0; // Reset current player's score

    delay(3000); // Give players time to read the result
    lcd.clear();
    lcd.print("Play Again?");
    lcd.setCursor(0,1);
    lcd.print("A-SP, B-MP");
    state = st_idle; // Go back to idle state to allow new game selection

  } else { // Single Player
    // For single player, the score is already displayed in reactionLoop()
    // Just prompt for a new game
    lcd.clear();
    lcd.print("Play Again?");
    lcd.setCursor(0, 1);
    lcd.print("A-SP, B-MP");
    delay(2000); // Small delay to let user see "Play Again?"
    state = st_idle;
  }
  digitalWrite(LED1, LOW);
  digitalWrite(LED3, LOW);
}

void reactionLoop()
{
  if (consumeK1PressEvent())
  {
    const unsigned long mils_now = millis();
    score = mils_now - mils_reaction_stimulus_shown;
    digitalWrite(LED3, LOW); // Turn off reaction LED

    lcd.clear();
    lcd.print("Your reaction:");
    lcd.setCursor(0, 1);
    lcd.print(score);
    lcd.print(" ms");

    if (gameMode == MULTIPLAYER) {
      char reactionTimeStr[10];
      sprintf(reactionTimeStr, "%09lu", score);
      client.publish(TOPIC_OUT, reactionTimeStr); // We send our score to the opponent
      mils_reaction_stimulus_shown = mils_now;
      state = st_score;
      lcd.clear();
      lcd.print("Waiting for");
      lcd.setCursor(0, 1);
      lcd.print("opponent...");
    } else { // Single player
      delay(1500);
      lcd.setCursor(0, 1);
      lcd.clear();
      lcd.print("Press A for new Race!");
      client.loop();
      state = st_score;
    }
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

      if (gameMode == MULTIPLAYER) {
          client.publish(TOPIC_OUT, "99999"); // False start!
      }
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

void waitingLoop() {
  if (gameMode == SINGLEPLAYER) {
    if (consumeK1PressEvent()) {
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
  } else {
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
  if (consumeK1PressEvent()) {
    gameMode = SINGLEPLAYER;
    state = st_waiting; // Classic mode
  } else if (consumeK2PressEvent()) {
    gameMode = MULTIPLAYER;
    state = st_connecting; //Time for networking
      lcd.clear();
    lcd.print("Connecting MQTT...");
    lcd.setCursor(0,1);
    lcd.print("Waiting for player");
  }
}