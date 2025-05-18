# BIP Erasmus+ IoT and 3D Printing: Notes for Physical Part

These notes focus on information from the virtual lectures **directly relevant and practically usable** for the **physical part** of the Blended Intensive Programme in Novi Sad.

## Core Hardware: The BIPBox (ESP32-based)

You will primarily work with the **BIPBox** and its integrated peripherals.

*   **Main Microcontroller: ESP32 Dev Kit (on BIPBox)**
    *   **Key Features:** Processor, ADC, DAC, Timers, **WiFi**, Bluetooth, **built-in Real-Time Clock (RTC)**.
    *   **WiFi:** Built-in with on-board antenna.
    *   **BIPBox Pinout (Essential for Labs):**
        *   `KEY1` (Button 1): Pin 39 (Input-only, **requires external pull-up/down if not on PCB**)
        *   `KEY2` (Button 2): Pin 36 (Input-only, **requires external pull-up/down if not on PCB**)
        *   `PIR_PIN` (Motion Sensor): Pin 23
        *   `DHT_PIN` (Temp/Humidity Sensor): Pin 19 (for DHT22)
        *   `PHOTORESISTOR_PIN`: Pin 34 (Input-only)
        *   `POTENTIOMETER_PIN`: Pin 35 (Input-only)
        *   `LED1_PIN` (Red LED): Pin 16
        *   `LED2_PIN` (Yellow LED): Pin 17
        *   `LED3_PIN` (Green LED): Pin 18
        *   `BUZZER_PIN`: Pin 33
        *   `SERVO_PIN`: Pin 32
        *   **LCD (16x2):** I2C interface (via PCF8574). Default ESP32 I2C: GPIO 21 (SDA), GPIO 22 (SCL).
        *   **BMI160 (Orientation Sensor):** I2C interface.

*   **External Real-Time Clock (RTC) Module (e.g., DS3231)**
    *   *May be used for "Lab: Data recording, Processing and Analysis" if enhanced timekeeping is needed.*
    *   Accurate, battery-backed (CR2032). Communicates via I2C.
    *   Requires initial time setting. Libraries available (e.g., RTClib).

*   **BIPBox Peripherals - Input Devices (Sensors):**
    *   **BMI160 Chip:** Orientation (X, Y, Z angles). Crucial for "Tilt & Align" game. Library: `DFRobot_BMI160.h`.
    *   **Potentiometer (Pin 35):** Analog input (`analogRead()`). For game controls (e.g., "Quack Attack").
    *   **Push Buttons (KEY1 Pin 39, KEY2 Pin 36):** Digital input. For user interaction. Remember: pins 34-39 are input-only and lack internal pull-ups/downs.
    *   **DHT22 Sensor (Pin 19):** Temperature & humidity. Library: `DHTesp.h`.
    *   **Photoresistor (Pin 34):** Light sensing. Analog input.
    *   **PIR Motion Sensor (Pin 23):** Motion detection. Digital input.

*   **BIPBox Peripherals - Output Devices (Actuators):**
    *   **LCD Display (16x2):** Shows measurements, game info. Library: `LiquidCrystal_I2C.h`.
        *   **Custom Characters:** Define up to 8 (5x8 pixels each) using `LCD.createChar()` and `LCD.write()`. Essential for simple graphics.
    *   **LEDs (Pins 16, 17, 18):** Visual feedback.
        *   **Polarity:** Anode (+) long lead, Cathode (-) short lead.
        *   **Current Limiting Resistor:** **ALWAYS USE ONE IN REAL CIRCUITS.** (e.g., 220-330Ω for 3.3V/5V).
    *   **Buzzer (Pin 33):** Audio feedback.
    *   **Servo Motor (Pin 32):** Controlled via PWM.

*   **Batteries:** Handle with care (charging, use).

## Key Software & Tools for Physical Labs

*   **Programming Language: C++ (Essential for ESP32/Arduino)**
    *   **Core Concepts:** Data types, control structures (if/else, loops), functions (incl. callbacks), strings, arrays, pointers, classes/structs, preprocessor directives (`#define`, `#include`).
    *   **Key Libraries (check `platformio.ini` for versions):**
        *   `<Arduino.h>`, `<WiFi.h>`, `<HTTPClient.h>`, `"ArduinoJson.h"`, `<SPIFFS.h>`
        *   `<PubSubClient.h>` (MQTT)
        *   `<DHTesp.h>` (DHT22 sensor)
        *   `<ESP32Time.h>` or `<time.h>` with `getLocalTime()` (ESP32 internal RTC & NTP)
        *   `"DFRobot_BMI160.h"` (BMI160 sensor)
        *   `<LiquidCrystal_I2C.h>` (LCD display)
        *   FreeRTOS headers (e.g., `freertos/FreeRTOS.h`, `freertos/task.h`)
    *   **Time Handling (ESP32):**
        *   `configTime(gmtOffset_sec, daylightOffset_sec, ntpServer)`: Sync with NTP.
        *   `getLocalTime(struct tm * timeinfo, ...)`: Get local time into `struct tm`.
        *   `strftime()`: Format `struct tm` to string.
        *   `ESP32Time rtc;`: Object for ESP32 internal RTC.
        *   `setenv("TZ", "CET-1CEST...", 1); tzset();`: For complex timezone/DST rules.

*   **IDE and Build System:**
    *   **PlatformIO:** Manages project structure, libraries, build process.
        *   **`platformio.ini` (Key Configuration):**
            ```ini
            [env:esp32dev]
            platform = espressif32
            board = esp32dev       ; Or your specific ESP32 board variant if different
            framework = arduino
            monitor_speed = 115200
            lib_deps =
                knolleary/PubSubClient         ; For MQTT
                bblanchon/ArduinoJson          ; For JSON
                DFRobot/DFRobot_BMI160       ; For BMI160 sensor
                marcoschwartz/LiquidCrystal_I2C ; For I2C LCD
                beegee-tokyo/DHT sensor library for ESPx ; For DHT sensor
                fbiego/ESP32Time               ; For ESP32 internal RTC
                ; Add other necessary libraries
            ```
    *   **JetBrains IDEs for ESP32/PlatformIO Development:**
        *   **CLion (Recommended for C/C++):**
            *   Use the official "[PlatformIO for CLion](https://plugins.jetbrains.com/plugin/13922-platformio-for-clion)" plugin.
            *   Install from CLion marketplace. Provides UI integration for PlatformIO actions.
        *   **IntelliJ IDEA (with C/C++ plugin):**
            *   **Option 1 (PlatformIO CLI + CMake):**
                1.  Generate CMake project: `pio project init --ide clion`.
                2.  Open `CMakeLists.txt` in IntelliJ.
                3.  Use IntelliJ for editing, integrated terminal for `platformio run`, `platformio run --target upload`, etc.
            *   **Option 2 (Basic CLI):** Use IntelliJ as editor, run all PlatformIO commands in terminal.
        *   **`platformio.ini`** remains central. Ensure PlatformIO Core is installed.

*   **Circuit Simulation Software:**
    *   **Wokwi ([wokwi.com](https://wokwi.com)):** **Primary ESP32 simulator.** Simulates ESP32, sensors, LEDs, displays, **WiFi**. Test C++ code and circuit ideas.
    *   **Falstad Circuit Simulator ([falstad.com/circuit/](https://falstad.com/circuit/)):** For visualizing basic electronic circuits, voltage/current.

*   **3D Design & Printing Software:**
    *   **Autodesk Fusion 360:** **Primary tool for 3D design lab.** Get free personal/academic version.
    *   **Slicing Software (e.g., Ultimaker Cura):** Converts `.STL` models to G-code for the printer.

*   **Data Analysis Software (for "Lab: Data recording, Processing and Analysis"):**
    *   **Python:** Using **Google Colab** is recommended.
        *   Libraries: **Pandas** (data manipulation), **Scikit-learn** (machine learning), **Matplotlib** (plotting).
        *   Focus on `water_pressure_dataset.csv` and scripts `Data_analysis.py`, `lab_water_pressure.py`.

*   **API Testing Tools:**
    *   **Postman** or **Advanced REST Client (ARC):** For testing HTTP APIs like the BIP2024NS Messaging API.

*   **IoT Communication & Cloud Services:**
    *   **BIP2024NS Messaging API (Azure Functions):**
        *   **Base URL:** `https://bipfunctions2024.azurewebsites.net/api/`
        *   **Key Endpoints:**
            *   `GET .../api/Time` (or `.../api/Time?format=unix`)
            *   `POST .../api/Message` (Body: `{"sender": "name", "recipient": "name", "messageText": "text"}`)
            *   `GET .../api/Message?recipient=name...`
        *   **Authorization Header:** `x-functions-key: W2I3RXCAg7HhfgUcKYSpWy13STX6fAWwd7HhPvCxqG8AzFu_4n3cA==`
    *   **Blynk:** IoT platform for mobile/web dashboards.
        *   Requires `BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME`, `BLYNK_AUTH_TOKEN`.
        *   Uses Virtual Pins (e.g., `V0`, `V1`). `BLYNK_WRITE(VPIN)`, `Blynk.virtualWrite(VPIN, value)`.
        *   `Blynk.run()` in loop/task. Relevant for "Lab: Uploading data to the cloud, Remote control".

## Networking for ESP32

*   **WiFi Connection:**
    *   ESP32 acts as a client to a WiFi Access Point (AP).
    *   Requires SSID and password.
    *   **DHCP is standard:** AP automatically assigns IP, DNS, Gateway.
    ```cpp
    #include <WiFi.h>
    const char* ssid = "YOUR_WIFI_SSID";
    const char* password = "YOUR_WIFI_PASSWORD";
    // In setup():
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
    ```
*   **HTTP/HTTPS Communication (for BIP API, other services):**
    *   Uses `HTTPClient` library.
    *   **GET Request:**
        ```cpp
        HTTPClient http;
        http.begin("https://bipfunctions2024.azurewebsites.net/api/Time");
        int httpCode = http.GET();
        if (httpCode == HTTP_CODE_OK) String payload = http.getString();
        http.end();
        ```
    *   **POST Request (with JSON):**
        ```cpp
        #include "ArduinoJson.h" // For creating JSON
        HTTPClient http;
        http.begin("https://bipfunctions2024.azurewebsites.net/api/Message");
        http.addHeader("Content-Type", "application/json");
        http.addHeader("x-functions-key", "W2I3RXCAg7HhfgUcKYSpWy13STX6fAWwd7HhPvCxqG8AzFu_4n3cA==");
        
        StaticJsonDocument<200> doc; // Adjust size as needed
        doc["sender"] = "MyESP32";
        doc["recipient"] = "Server";
        doc["messageText"] = "Hello from ESP32!";
        String jsonOutput;
        serializeJson(doc, jsonOutput);
        
        int httpCode = http.POST(jsonOutput);
        if (httpCode == HTTP_CODE_OK) String response = http.getString();
        http.end();
        ```
    *   **JSON:** Lightweight data format. Use `ArduinoJson` library or `snprintf` for simple JSON.
*   **MQTT (for Game Alternative / Cloud Lab):**
    *   Publish/subscribe protocol. Requires an MQTT Broker (e.g., Mosquitto, public brokers like `test.mosquitto.org`).
    *   Uses `PubSubClient` library.
    ```cpp
    #include <PubSubClient.h>
    WiFiClient espClient; // Or WiFiClientSecure for TLS
    PubSubClient client(espClient);
    const char* mqtt_broker = "test.mosquitto.org"; // Or your broker IP
    const int mqtt_port = 1883; // 8883 for TLS

    void callback(char* topic, byte* payload, unsigned int length) { /* Handle message */ }
    void reconnect_mqtt() { /* Reconnection logic */ }
    // In setup():
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    // In loop():
    if (!client.connected()) reconnect_mqtt();
    client.loop(); // Essential for processing messages
    // client.publish("topic/test", "hello world");
    // client.subscribe("topic/commands");
    ```
*   **ESPNow (Recommended for BIPBox-to-BIPBox Game Communication):**
    *   Direct, low-latency communication between ESP32s without a router after initial pairing.
    *   Requires MAC address of peer device.
    *   Simpler than MQTT for direct peer-to-peer. Search for ESPNow examples.

## Digital Electronics Fundamentals for Labs

*   **Ohm's Law:** `U = I * R` (Voltage = Current × Resistance). Essential for resistor calculations.
*   **Resistors:** Limit current. For LEDs, typically 1/8W or 1/4W. Use color codes or multimeter to identify values.
*   **LED Current Limiting:** **Crucial.** Calculate resistor: `R = (V_Source - V_LED_Forward) / I_Desired`.
    *   E.g., for 3.3V source, Red LED (Vf ~1.8V), 10mA (0.01A) current: `R = (3.3 - 1.8) / 0.01 = 150Ω`. Use nearest standard value (e.g., 150Ω or 180Ω).
*   **Voltage Divider:** Two resistors in series to get a lower voltage. Potentiometers are variable voltage dividers.
*   **Pull-up/Pull-down Resistors (for Buttons):**
    *   Ensure defined state (HIGH/LOW) for inputs.
    *   `pinMode(pin, INPUT_PULLUP)`: Activates internal pull-up. Button connects pin to GND. Press = LOW.
    *   **ESP32 pins 34-39 (incl. KEY1, KEY2) LACK internal pull-ups/downs.** Use external resistors (e.g., 10kΩ to 3.3V for pull-up, button to GND) if not already on BIPBox PCB.

## Embedded Programming Concepts (C++ for ESP32)

*   **Arduino Program Structure:**
    *   `void setup()`: Runs once for initialization (pin modes, Serial, WiFi, FreeRTOS tasks).
    *   `void loop()`: Runs repeatedly. With FreeRTOS, this might be minimal.
*   **Finite State Machines (FSMs):** Excellent for managing game logic or sequential operations.
    *   Define `states`, `events`, `transitions`, and `actions`.
*   **Multitasking with FreeRTOS (Essential for complex projects/games):**
    *   ESP32 has dual cores and FreeRTOS support.
    *   **Tasks:** Independent C functions running (seemingly) in parallel. Each has its own stack.
        ```cpp
        // Task function signature
        void myTask(void *pvParameters) {
          for (;;) { // Infinite loop
            // Task logic
            vTaskDelay(pdMS_TO_TICKS(1000)); // Non-blocking delay for 1 sec
          }
        }
        // In setup():
        xTaskCreate(
          myTask,         // Task function
          "MyTaskName",   // Name (for debugging)
          2048,           // Stack size (bytes) - adjust as needed!
          NULL,           // Parameters (optional)
          1,              // Priority (0 is lowest)
          NULL            // Task handle (optional)
          // For specific core: xTaskCreatePinnedToCore(..., APP_CPU_NUM);
        );
        ```
    *   **`vTaskDelay(ticks)` or `vTaskDelay(pdMS_TO_TICKS(milliseconds))`:** Non-blocking delay, yields CPU. **Use this instead of `delay()` in tasks.**
    *   **Stack Size:** Critical. Too small leads to crashes. Start with 2048-4096 bytes for simple tasks.
    *   **Task Communication/Synchronization:** (Advanced, if needed) Queues, Semaphores, Mutexes.
*   **"Manual" Multitasking (using `millis()` - for simpler non-blocking timing):**
    ```cpp
    unsigned long previousMillis = 0;
    const long interval = 1000; // 1 second
    // In loop() or a task:
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      // Perform action
    }
    ```
*   **Button Debouncing:** Handle mechanical bounce for reliable button presses.
    *   Software: Check state only after a short delay (e.g., 50ms) since last change.
*   **Command Line Interface (CLI) via Serial Monitor:**
    *   Allows changing parameters (WiFi credentials, server URLs) without re-uploading.
    *   Read commands from `Serial`, parse, and act.
*   **Persistent Configuration (SPIFFS Flash File System):**
    *   Save settings (e.g., WiFi credentials) across reboots.
    *   `#include <SPIFFS.h>`. `SPIFFS.begin(true);`
    *   Store data in files (e.g., `config.json`). Use `ArduinoJson` to serialize/deserialize.
    ```cpp
    // Example: Saving (simplified)
    File configFile = SPIFFS.open("/config.json", FILE_WRITE);
    serializeJson(doc, configFile); // doc is your JsonDocument
    configFile.close();

    // Example: Loading (simplified)
    File configFile = SPIFFS.open("/config.json", FILE_READ);
    deserializeJson(doc, configFile);
    configFile.close();
    // String ssid = doc["wifi_ssid"].as<String>();
    ```

## Data Analysis & Machine Learning (Python for Lab)

*   **Focus:** `water_pressure_dataset.csv`, scripts `Data_analysis.py`, `lab_water_pressure.py`.
*   **Tools:** Python, Pandas, Scikit-learn, Matplotlib (in Google Colab).
*   **Key Pandas Operations for the Lab:**
    *   `pd.read_csv()`
    *   Data type conversion: `pd.to_datetime()`
    *   Time series indexing: `df.set_index('Created')`
    *   Smoothing: `df['Payload'].rolling(window=...).mean()`
    *   Lag features: `df['Payload_MA'].shift(i)`
    *   Cyclical time features (sin/cos for minute, hour, day, month).
*   **Scikit-learn for `lab_water_pressure.py`:**
    *   `train_test_split()`
    *   `DecisionTreeRegressor()`: Tune `max_depth`, `min_samples_leaf`.
    *   DNN model (details in script).
    *   Evaluation: `mean_squared_error()` (MSE), `np.sqrt(MSE)` (RMSE).

## 3D Design & Printing (Fusion 360 for Lab)

*   **Goal:** Design and print enclosures or parts for your IoT project/game.
*   **Software:** **Autodesk Fusion 360** (CAD) -> **Slicer** (e.g., Cura) -> G-code.
*   **Process:**
    1.  **Design in Fusion 360:** Create 2D sketches, extrude to 3D, use constraints.
        *   Consider: Mounting holes, port access, component fit, tolerances.
        *   Use **Section Analysis** to check internal fits.
    2.  **Export as `.STL`**.
    3.  **Slice in Cura:** Set layer height (e.g., 0.2mm), infill (e.g., 20%), supports (if needed), adhesion (brim/raft).
    4.  **Print (FDM printers, likely PLA material):** Transfer G-code, monitor print.
    5.  **Post-Process:** Remove from bed, remove supports, clean up (sandpaper, knife - carefully!).
*   **Material: PLA (Polylactic Acid)** - Easy to print, good for prototypes.
*   **Common FDM Issues & Solutions:**
    *   **Warping/Bed Adhesion:** Heated bed, brim/raft, clean bed, proper Z-offset.
    *   **Stringing:** Tune retraction, temperature.
    *   **Overhangs:** Design to minimize or use supports (generated by slicer).
*   **Design for Printability:** Minimize overhangs (use 45° rule or chamfers), ensure sufficient wall thickness.

## Assignments for Physical Part

*   **Groups will work on three assignments.** Development can start before arrival.
### 1. MCU Programming Problem
    *   Details to be provided. Involves C++ on ESP32/BIPBox.
### 2. Design and Implement a Turn-Based or Interactive Game
    *   **Two-player game using two BIPBoxes.**
    *   **Communication:** **ESPNow (recommended)** or MQTT.
        *   Define message types, payload structure, serialization.
    *   **Hardware:** Utilize BIPBox sensors/actuators (BMI160, Potentiometer, LCD, Buttons).
    *   **Examples:** "Tilt & Align", "Quack Attack".
    *   **Deliverables:** Functional game, written material, presentation.
### 3. Design a 3D Printed Model
    *   Done **during the in-person part** using Fusion 360.
    *   Enclosures, game parts, wearables.

*   **Python Practical Exercises (Deadlines from emails):**
    *   Goals 1 & 2 (`Data_analysis.py`, `lab_water_pressure.py` for SenderID 1) - **Due May 2, 2025 (Virtual Part)**.
    *   Additional Goals (Repeat for SenderID 5, DNN model) - **Due May 19, 2025 (Start of Physical Part)**.

## Physical Part Logistics (from emails/PDF)

*   **Bring:** Laptop with Arduino IDE (and PlatformIO setup for your chosen JetBrains IDE or VSCode), Fusion 360 (personal/academic). Mobility documents for signing.
*   **Schedule:**
    *   **Monday, May 19, 2025, 8:50 AM:** Meet at Science and Technology Park, Fruškogorska 1, Novi Sad (main hall).
    *   **Monday, May 19, 2025, 7:00 PM:** Welcome dinner at Restaurant Cubo.
    *   Daily: 9:00-13:30 (Lectures/Exercises), 13:30-14:30 (Lunch - provided), 14:30-15:30 (Q&A).
    *   Group presentations: Thursday/Friday.
*   **Contact:** `IoT.bip@uns.ac.rs`
*   **Insurance:** Standard international travel insurance covering dates.
