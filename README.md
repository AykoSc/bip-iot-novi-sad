# BIP Erasmus+ IoT and 3D Printing Notes

These notes focus on information from the virtual lectures relevant for practical application during the **physical part** of the Blended Intensive Programme.

## Core Hardware for Physical Labs

You will primarily be working with the **BIPBox** (an ESP32-based device) and various peripherals during the physical labs.

*   **Main Microcontroller ("The Brains"/"The Machine"):**
    *   **ESP32 Dev Kit (BipBox):** The central component for your projects. This is a tiny box with a processor that forms the heart of your embedded system.
        *   **Peripherals (on-chip):** Includes ADC (Analog-to-Digital Converter), DAC (Digital-to-Analog Converter), Timers, **WiFi**, Bluetooth, **built-in Real-Time Clock (RTC)**.
        *   **WiFi Hardware:**
            *   ESP32 has built-in support for WiFi.
            *   The metal shielding on the ESP32 module typically covers the CPU and WiFi silicon.
            *   An antenna is integrated on the board (often a PCB trace antenna) or can be an external one via a U.FL connector on some ESP32 dev kits.
            *   The default integrated antenna generally provides decent range for use within a typical room or small apartment.
        *   **Pin Definitions:** Pin numbers in code (e.g., `PIN_WATERPRESSUREANALOG` in `taskWaterPressure.cpp`, or defined in a `pinsDef.h` file) will map to physical pins on the ESP32. These definitions are often grouped in a header file or at the beginning of `main.cpp`.
            *   Example from code: `// definicije pinova su ovde` (pin definitions are here) likely points to a `pinsDef.h` or similar.
            *   **BIPBox Specific Pinout (from `AllDeviceTest.ino` and physical device image):**
                *   `KEY1`: Pin 39
                *   `KEY2`: Pin 36
                *   `PIR_PIN`: Pin 23 (Passive Infrared Sensor)
                *   `DHT_PIN`: Pin 19 (DHT22 Temperature/Humidity Sensor)
                *   `PHOTORESISTOR_PIN`: Pin 34
                *   `POTENTIOMETER_PIN`: Pin 35
                *   `LED1_PIN`: Pin 16 (Red LED)
                *   `LED2_PIN`: Pin 17 (Yellow LED)
                *   `LED3_PIN`: Pin 18 (Green LED)
                *   `BUZZER_PIN`: Pin 33
                *   `SERVO_PIN`: Pin 32
                *   **LCD:** Uses I2C, connected via PCF8574 expander. Default I2C pins for ESP32 are usually GPIO 21 (SDA) and GPIO 22 (SCL).
                *   **BMI160:** Uses I2C.
    *   Arduino Uno: May be used for initial prototyping and learning basic concepts.
*   **External Real-Time Clock (RTC) Modules (e.g., DS3231):**
    *   Can be used in conjunction with the ESP32 if a more accurate or battery-backed RTC is needed.
    *   The **DS3231** is a common, low-cost, and accurate RTC module.
    *   **Battery Backup:** These modules typically include a coin cell battery (e.g., CR2032) to maintain time even when the main ESP32 is powered off. Battery life can range from months to several years.
    *   **Communication:** Usually via I2C interface.
    *   **Libraries:** Arduino libraries are available for easy communication (e.g., setting time, reading time).
    *   **Initial Setup:** When first powered up, an external RTC might have an incorrect or default time (e.g., Jan 1, 2000) and will need to be set (either manually, via NTP through the ESP32, or from a GPS module).
    *   *Relevance for Physical Labs:* Useful for projects requiring accurate timekeeping independent of ESP32 power status or internet connectivity for NTP. This will be relevant for the "Lab: Data recording, Processing and Analysis".
*   **Peripherals (Devices attached to the processor / on the BIPBox):**
    *   **Input Devices (Sensors):**
        *   **BMI160 Chip:** For orientation sensing (provides X, Y, Z angles). Crucial for tasks like the "Tilt & Align" game example. (Indicated by `#include "DFRobot_BMI160.h"`). Connected via I2C.
        *   **Potentiometer:** A three-terminal variable resistor. Can be used as a **voltage divider** for analog input (e.g., controlling LED brightness, game controls like in "Quack Attack" via `analogRead(POTENTIOMETER_PIN)`). Its resistance change affects voltage and current in the circuit. Connected to **Pin 35**.
        *   **Push Buttons (Keys):** For user interaction (e.g., "Push Button to Walk" in traffic light example, game controls). Two buttons on BIPBox: **Key1 (Pin 39), Key2 (Pin 36)**.
        *   **DHT22 Sensor (or DHT11/DHT21):** For measuring temperature and humidity. (Indicated by `#include "temp.hpp"`, `#include <DHTesp.h>`, and comments like `// za DHT22` (for DHT22) in `main.cpp`). Connected to **Pin 19**.
            *   Initialization: `dht.setup(DHT_PIN, DHTesp::DHT22);`
            *   Reading: `TempAndHumidity temp_hum = dht.getTempAndHumidity();` then access `temp_hum.temperature` and `temp_hum.humidity`.
        *   **Photoresistor:** Light-dependent resistor. Analog input. Connected to **Pin 34**.
        *   **PIR Motion Sensor:** Detects motion. Digital input. Connected to **Pin 23**.
        *   **Water Pressure Sensor (e.g., Danfoss MBS 1700):** Example used in `taskWaterPressure.cpp` for analog reading and conversion to physical units. *This sensor is likely not on the standard BIPBox but an example for analog input processing.*
        *   Other analog and digital sensors will be introduced. These feed data into the ESP32 for processing.
    *   **Output Devices (Actuators):**
        *   **LCD Display (16x2):** For showing measurements and game information. (Indicated by `#include <LiquidCrystal_I2C.h>`). Connected via I2C using a **PCF8574 I/O expander**.
            *   **Custom Characters:** The 16x2 LCD allows defining up to **8 user-defined characters (UDCs)**. Each is a 5x8 pixel matrix.
                *   `LCD.createChar(char_index, pixel_data_array)`: Defines a UDC (index 0-7). `pixel_data_array` is 8 bytes, each byte's lower 5 bits define a row.
                *   `LCD.write(char_index)` or `LCD.printByte(char_index)`: Displays the UDC.
                *   Essential for creating simple graphics like the "Analog Strip Gauge" or game elements (ducks, bushes).
                *   The character representing a "full block" of pixels is often character code `255` (or one of the UDCs if defined as such).
        *   **TM1637 7-Segment Display:** For numerical output. (Indicated by `#include "disp/disp.hpp"` and comments like `// za TM1637 7-segment display` (for TM1637 7-segment display) in `main.cpp`). *This might be an alternative display, not on the primary BIPBox.*
        *   **LEDs (Light-Emitting Diodes):** For visual feedback. Three on BIPBox:
            *   Red LED: **Pin 16**
            *   Yellow LED: **Pin 17**
            *   Green LED: **Pin 18**
            *   **Polarity:** Have an **anode (+) (longer lead)** and a **cathode (-) (shorter lead, flat side of casing)**. Current flows from anode to cathode.
            *   **Current Limiting:** **Crucial:** Always use a **series resistor** to limit current and prevent burnout, especially when connecting to MCU pins or batteries. *Wokwi simulator might not enforce this, but it's essential in real circuits.*
            *   **Voltage Drop (Forward Voltage - V<sub>F</sub>):** LEDs have a characteristic voltage drop when lit. This varies by color:
                *   Red: 1.8V - 2.1V
                *   Amber/Yellow: ~2.0V - 2.2V
                *   Green: ~2.0V - 3.1V
            *   **Operating Current:** Typically 5mA to 20mA. Exceeding this can damage the LED or MCU.
            *   **SMD LEDs (Surface Mounted Devices):** Small, compact LEDs often found directly on PCBs (like the ESP32 board). Usually machine-soldered.
        *   **Buzzer:** For audio feedback. Connected to **Pin 33**.
        *   **Servo Motor:** Can be controlled via PWM. Connection to **Pin 32**.
        *   The ESP32 acts upon processed data to control these (e.g., turning lights on/off).
*   **Batteries:**
    *   Be mindful of safety warnings (e.g., "Disfiguring fire, explosion risk").
    *   Proper handling during charging and use is essential.
*   **Data Storage (Local & Backend):**
    *   **SD Card:** Can be connected to ESP32 for local data storage. Useful if internet is unavailable; data can be logged locally and uploaded later. (Relevant for "Lab: Data recording, Processing and Analysis").
    *   **SQLite Database (Python Backend Example):**
        *   A simple file-based database (`database.sqlite`).
        *   Example table structure for sensor data: `CREATE TABLE IF NOT EXISTS data (ID INTEGER PRIMARY KEY AUTOINCREMENT, temperature REAL, humidity REAL);`
        *   IoT devices send JSON data to a web server, which then inserts it into this database.

## Key Software & Tools

Familiarity with these tools will be beneficial for the physical part. Simulation tools are particularly important for pre-testing.

*   **Programming Language:**
    *   **C++:** This is **essential** for programming the Arduino and ESP32.
        *   **Core Concepts to Master (from "Programming an Embedded Device" lecture & Code Examples):**
            *   Data types (`int`, `float`, `char`, `String`, `unsigned long` for time, custom structs/classes like `model`, `TempAndHumidity`, `struct tm` for time).
            *   Decision control structures (if, else, switch).
            *   Multiple conditions.
            *   **Functions:** Operations will be abstracted as functions.
                *   **Callback Functions:** Used in command processing (e.g., `CmdProc::Add` takes a function pointer like `cmd_SetSsid`) and for handling asynchronous events like MQTT message arrival (`client.setCallback(callback_function_name)`).
                *   **Activation Records (AR):** When a function is called, an AR is created on its task's stack. It holds parameters, return address, saved registers, and local variables for that specific call. This is how recursion works and why tasks need sufficient stack space.
            *   Loops (for, do while, **`while (true)` or `for(;;)` for main control loop / FreeRTOS tasks**).
            *   Strings (Arduino `String` class, `std::string`, C-style `char` arrays/pointers like `char buf[255];`).
            *   Arrays and Pointers.
            *   **Classes and Structs:** Hardware units can be treated as **Objects** with internal state.
                *   Example: A `model` object (likely defined in `model.hpp`) seems to hold global state like sensor values, configuration parameters (e.g., `model.analogValue`, `model.pressure`, `model.iotServerUrl1`, `model.wifiSsid`). Comment in `main.cpp`: `// globalni podaci su ovde` (global data is here).
                *   Custom classes like `CmdProc` (command processor), `SerialBuf` (serial buffer), `HTTPClient`, `WiFiClientSecure`, `PubSubClient` demonstrate modular design.
            *   **`#preprocessor` directives** (e.g., `#define KEY1 39` for mapping pins, `#include`, `#define HAVE_DHT` for conditional compilation, `#define NTP_SERVER "pool.ntp.org"`).
            *   **Libraries and `#includes`** (e.g., `#include <Arduino.h>`, `#include <WiFi.h>`, `#include <HTTPClient.h>`, `#include "ArduinoJson.h"`, `#include <SPIFFS.h>`, `#include <PubSubClient.h>`, `#include <DHTesp.h>`, `#include <ESP32Time.h>` for RTC, `#include <time.h>` for `tm` struct and `strftime`, `#include "DFRobot_BMI160.h"`, `#include <LiquidCrystal_I2C.h>`).
                *   Project structure with multiple `.hpp` (header) and `.cpp` (source) files is shown (e.g., `iotClient.cpp`, `taskWaterPressure.cpp`, `cmdLine.cpp`, `cliCommands.cpp`, `model.hpp`). Header files (`.hpp`) contain declarations, source files (`.cpp`) contain definitions/implementations.
            *   **Variables:** Represent memory locations for internal data, inputs, and outputs. They have names (identifiers) and visibility (scope).
                *   `static` variables within functions retain their value between calls (e.g., `lastActionTime` in `millis()` based timing).
            *   **Bitwise Operations:** Essential for tasks like defining custom LCD characters. **Suggested to review these for C/C++.** (e.g., `def |= bit; bit = bit >> 1;` in Analog Strip Gauge example).
        *   **Time Handling Functions (ESP32/Arduino):**
            *   `configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, [ntpServer2], [ntpServer3])`: Configures the system time using NTP.
                *   `gmtOffset_sec`: Offset from GMT/UTC in seconds (e.g., `3600` for CET+1).
                *   `daylightOffset_sec`: Offset for DST in seconds (e.g., `3600` if DST is active, `0` otherwise).
                *   `ntpServer`: String with NTP server address (e.g., `"pool.ntp.org"`).
            *   `getLocalTime(struct tm * timeinfo, uint32_t ms = 5000)`: Attempts to get the synchronized time and populates a `tm` struct. Returns `true` on success. The optional `ms` parameter is a timeout.
            *   `struct tm timeinfo;`: Declare a variable of type `struct tm` to hold time components (sec, min, hour, mday, mon (0-11), year (since 1900), wday, yday, isdst).
            *   `strftime(char* buf, size_t maxsize, const char* format, const struct tm* timeptr)`: Formats the time from a `tm` struct into a character string according to `format`.
                *   Example: `strftime(timeStr, sizeof(timeStr), "%A, %B %d %Y %H:%M:%S", &timeinfo);`
            *   `ESP32Time rtc;` or `ESP32Time rtc(offset_seconds);`: Object for interacting with the ESP32's internal RTC.
                *   `rtc.setTime(epoch_seconds);`: Sets the RTC using Unix epoch time (seconds since Jan 1, 1970).
                *   `rtc.setTimeStruct(time_struct);`: Sets RTC from a `tm` struct.
                *   `rtc.getEpoch()`: Gets current time as Unix epoch.
                *   `rtc.getHour(true)` (24hr format), `rtc.getMinute()`, `rtc.getSecond()`, `rtc.getDay()`, `rtc.getMonth()+1`, `rtc.getYear()`.
            *   `setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1); tzset();`: For POSIX timezone string setup. This allows `getLocalTime()` to correctly apply complex DST rules.
*   **IDE and Build System:**
    *   **Visual Studio Code with PlatformIO extension:** This is the demonstrated environment.
        *   **`platformio.ini` file:** Configures the project. Key settings for ESP32:
            ```ini
            [env:esp32dev] // Or a custom name like [env:esp32dev_cloud]
            platform = espressif32
            board = esp32dev
            framework = arduino
            monitor_speed = 115200
            lib_deps = 
                knolleary/PubSubClient @ ^2.8       // For MQTT
                bblanchon/ArduinoJson @ ^6.21.2    // For JSON handling
                // Example for DHT sensor:
                // beegee-tokyo/DHT sensor library for ESPx @ ^1.19 
                // Example for DS3231 RTC:
                // adafruit/RTClib @ ^2.1.1 (or similar)
                // Example for ESP32Time library (often built-in or easily added):
                // fbiego/ESP32Time @ ^2.0.0 (if not part of core)
                // Example for BMI160:
                // DFRobot/DFRobot_BMI160 @ ^1.0.2 (or similar, check AllDeviceTest.ino)
                // Example for LiquidCrystal_I2C:
                //marcoschwartz/LiquidCrystal_I2C @ ^1.1.4 (or similar, check AllDeviceTest.ino)
            ```
            *   `lib_deps` is used to specify required libraries (e.g., for MQTT, JSON, specific sensors, RTC). PlatformIO will download and manage them.
*   **Circuit Simulation Software:**
    *   **Falstad Circuit Simulator ([falstad.com/circuit/](https://www.falstad.com/circuit/)):**
        *   **Will be used** for visualizing and simulating basic electronic circuits.
        *   Allows placing components, connecting them, measuring voltage/current, and observing current flow (visualized by moving dots).
        *   Useful for understanding voltage distribution (e.g., in voltage dividers, across potentiometers) and current changes.
        *   Great for pre-testing circuit ideas before building physical prototypes.
        *   **Circuits can be saved/shared as a text string or a direct URL.** This is useful for collaboration and documentation.
    *   **Wokwi:** Online ESP32 simulator.
        *   Described as a "complete simulation of the ESP32 microcontroller and related devices."
        *   Simulates: ESP32, sensors, LEDs, displays.
        *   **WiFi simulation is also supported.**
        *   *Use this to test your C++ code and circuit ideas before building physical prototypes. Note: Wokwi might not simulate current limits for LEDs correctly, so always add a resistor in real life.*
        *   Example Wokwi projects from lectures:
            *   `WiFiCloudTime - Reading time from the cloud` (HTTPS GET)
            *   `WiFiCloudTemp - Sending temperature to the cloud` (HTTPS POST with API Key)
            *   `ESP32 NTP Example` (shows time on LCD from NTP)
            *   `ESP32 Setting time without NTP` (allows manual time set via Serial or NTP command)
            *   `PotentiometerSerial` (Wokwi example for Analog Strip Gauge)
            *   MPU6050 Accelerometer + Gyroscope example (visualizes 3D cube on an OLED display based on sensor orientation).
*   **Online Calculators:**
    *   **LED Calculator ([ledcalculator.net](https://ledcalculator.net)):** Useful for quickly determining the correct current-limiting resistor value for LEDs.
        *   Input: Power supply voltage (e.g., 5V from Arduino/ESP32, or battery voltage), LED voltage drop (V<sub>F</sub> from datasheet or table), desired LED current (e.g., 10mA, 15mA, 20mA), number of LEDs.
        *   Output: Suggested resistor value (ohms) and a simple wiring diagram.
*   **3D Design Software (for 3D Printing Lab):**
    *   The following will be used/introduced:
        *   **Autodesk Fusion 360:** **Primary tool for the course.**
            *   **Free for personal and academic use.** Academic use requires proof of status and provides full commercial capabilities.
            *   Be aware that files created in the academic version might have features not fully supported if opened in the purely personal (free) version, potentially limiting collaboration if versions differ.
            *   *It's recommended to obtain the academic version if eligible for full feature access during the lab.*
        *   **ShapR3D**
        *   **Blender**
        *   **SketchUp** (Mentioned as simpler, but less ideal for complex 3D printing designs compared to Fusion 360).
    *   *Gaining basic familiarity with **Autodesk Fusion 360** beforehand is highly advantageous for the "Lab: 3D Design and 3D printing (wearables, devices enclosures, etc.)".*
*   **PCB Design Software (Context/Awareness):**
    *   **EasyEDA:** Mentioned as CAD software used for designing PCBs like the one in the BipBox. Understanding the general workflow (schematic -> layout -> manufacturing files) can be helpful.
*   **Data Analysis Software (for Homework & Lab Prep):**
    *   **Microsoft Excel:** Used for the "Knowledge Clips" homework task involving statistical analysis (BMI, descriptive stats, boxplots, z-scores, outliers) on the `health.xlsx` dataset. This prepares for the "Lab: Data recording, Processing and Analysis".
    *   **Python with Pandas, Scikit-learn, Matplotlib (via Google Colab):** Will be used for data processing, analysis, visualization, and machine learning tasks in the physical labs.
        *   **Google Colab:** Recommended framework for Python exercises.
            *   Free, cloud-based, enables Python code execution in a web browser.
            *   Good for machine learning, data analysis, and collaboration.
            *   To add files (like datasets):
                *   Use the "Files" (Datoteke) tab on the left.
                *   Click the "Upload to session storage" button (icon: page with an upward arrow, tooltip "Prenesi" / "Upload").
                *   Can also synchronize with Google Drive or drag-and-drop files.
*   **API Development & Testing Tools:**
    *   **Visual Studio 2022:** Used to create sample WebAPI projects (e.g., ASP.NET Core WebAPI).
    *   **Python (with `requests` library):** Used to write simple HTTP clients to consume/test WebAPIs.
    *   **Postman:** [https://www.postman.com/product/rest-client/](https://www.postman.com/product/rest-client/) - A popular tool for API testing.
    *   **ARC (Advanced REST Client):** [https://install.advancedrestclient.com/](https://install.advancedrestclient.com/) - Another tool for API testing, demonstrated in lectures.
*   **Cloud Platforms & Services:**
    *   **Azure Functions:** Event-driven, serverless compute service. Used to host the BIP2024NS messaging API and the Time API.
        *   Allows running small units of functionality (programmed in .NET - C#, F#) without managing infrastructure.
        *   Scales automatically.
        *   Can be triggered by HTTP requests, database changes, queue messages.
        *   Supports authentication (e.g., API Keys, Azure AD, Google, GitHub, etc.).
        *   Example documentation for Azure Functions related to the course is in the Classroom Notes (OneNote/Teams).
    *   Microsoft Azure (general cloud platform), Microsoft IoT Hub, Arduino Cloud.
    *   **Blynk:**
        *   IoT platform for creating mobile/web dashboards to control hardware.
        *   Offers a free tier (often limited by number of devices or widgets).
        *   Provides libraries for ESP32 (`BlynkSimpleEsp32.h`).
        *   Requires `BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME`, and `BLYNK_AUTH_TOKEN` to be defined in the ESP32 code. These are obtained from the Blynk console when you set up a device template.
        *   Uses "Virtual Pins" (e.g., `V0`, `V1`) to exchange data between the app/dashboard and the ESP32.
            *   `BLYNK_WRITE(V_PIN_NUMBER)`: A function that is automatically called when the value of `V_PIN_NUMBER` is changed from the Blynk app/dashboard. (e.g., `BLYNK_WRITE(V0)` for a switch).
            *   `Blynk.virtualWrite(V_PIN_NUMBER, value)`: Sends a value from the ESP32 to the specified virtual pin on the Blynk app/dashboard.
        *   `Blynk.run()`: Needs to be called regularly in the `loop()` or a dedicated task to process Blynk communication.
        *   *Relevance for Physical Labs: "Lab: Uploading data to the cloud, Remote control". Could also be used for game control/display if ESPNow/MQTT is challenging.*
    *   **Cloud Deployment Options & Considerations:**
        *   **Local PC as Server:**
            *   Pros: Free/cheap if ISP allows port forwarding and you have a static IP.
            *   Cons: ISP dependency, PC becomes dedicated, **major security and availability concerns.**
        *   **IaaS (Infrastructure as a Service - e.g., Virtual Machine on Digital Ocean, Linode, Azure VM):**
            *   Pros: Relatively inexpensive for basic VMs (e.g., ~$4-5/month).
            *   Cons: **You are the administrator.** Responsible for all OS/software patches, updates, and security.
        *   **PaaS (Platform as a Service - e.g., Azure App Service, Heroku, Google App Engine):**
            *   Pros: No need to manage underlying infrastructure/OS. Easy to scale.
            *   Cons: Can become expensive if resource usage isn't monitored.
        *   **SaaS (Software as a Service - e.g., Google Workspace, Dropbox, Cisco WebEx):**
            *   Pros: No maintenance worries at all. Easy to scale.
            *   Cons: Can be expensive. You use a pre-built service, less customization.
*   **IoT Communication & Platforms (for Game & Cloud Labs):**
    *   **Primary Goal:** Send and receive data from/to the microcontroller over a network.
    *   **Connection Types for ESP32/Arduino:**
        *   **WiFi:** **Main focus for the course.** ESP32 will act as a **client** connecting to a WiFi Access Point (AP).
            *   Requires **SSID** (network name) and **password** of the AP.
                ```cpp
                // In your C++ code (e.g., main.cpp or a config file)
                const char* ssid = "YOUR_WIFI_SSID"; // e.g., "MYAP"
                const char* password = "YOUR_WIFI_PASSWORD"; // e.g., "Radi.Stvarno.Radi" (It works. It really works.)
                ```
            *   Initialize WiFi connection in `setup()`:
                ```cpp
                Serial.begin(115200); // For debug messages
                WiFi.begin(ssid, password);
                Serial.print("Connecting to WiFi..");
                while (WiFi.status() != WL_CONNECTED) {
                    delay(500); // Using delay here is acceptable as setup runs once
                    Serial.print(".");
                }
                Serial.println("\nConnected to the WiFi network");
                Serial.print("IP address: ");
                Serial.println(WiFi.localIP()); // Print the ESP32's IP address
                ```
            *   ESP32 can also act as a primitive AP itself.
        *   **Ethernet:** Possible with an additional module.
        *   **Cellular (SIM card module):** For mobile internet access.
    *   **Networking Protocols (Layered Approach):**
        *   **TCP/IP:**
            *   Fundamental protocol for communication between computers once a network link (like WiFi) is established.
            *   Higher-level protocols (HTTP, MQTT) are built on top of TCP/IP.
            *   **Key elements for connection:**
                *   **IP Address:** Unique numerical identifier for a device on a network.
                    *   **IPv4:** 32-bit (e.g., `147.91.177.196`). Four decimal numbers (0-255) separated by dots.
                    *   **IPv6:** 128-bit (e.g., `2001:0db8::8a2e:0370:7334`). Longer, hexadecimal format.
                *   **Port Number:** A 16-bit number (0-65535) that identifies a specific application or service on a device.
            *   **Client-Server Architecture:**
                *   A **server** program listens on a specific IP address and port for incoming connections.
                *   A **client** program initiates a connection to the server's IP address and port.
                *   The combination (Server IP, Server Port) is called a **socket**.
                *   A single server machine can run multiple server programs, each listening on a different port.
            *   TCP/IP itself allows for the exchange of raw **bytes** or arrays of bytes.
        *   **HTTP/HTTPS (HyperText Transfer Protocol / Secure):**
            *   Built on top of TCP/IP. Commonly used for WebAPIs and cloud communication.
            *   **Client-Server Model:** ESP32 acts as an HTTP client, connecting to an HTTP server (web server).
            *   **Connection:** Client needs server IP address and port (default **80 for HTTP**, **443 for HTTPS**). If a non-default port is used, it must be specified in the URL (e.g., `http://192.168.137.1:8080/`).
                *   **Server Address for Local Python Server Example:** `const char *server = "http://192.168.137.1:8080";` (Replace IP with your laptop's IP).
            *   **Using `HTTPClient` library on ESP32:**
                ```cpp
                #include <HTTPClient.h>
                // ... (WiFi connection code) ...

                HTTPClient http;
                String server_url = "http://YOUR_SERVER_IP:PORT"; // For HTTP
                // For HTTPS: String server_url = "https://YOUR_SERVER_DOMAIN_OR_IP";
                
                // --- Sending a GET request ---
                String get_endpoint = "/some_resource"; // e.g., "/api/Time" for cloud
                http.begin(server_url + get_endpoint); 

                int httpCode = http.GET();
                if (httpCode > 0) { 
                    if (httpCode == HTTP_CODE_OK) { // 200
                        String payload = http.getString();
                        Serial.println("GET Response: " + payload);
                    } else {
                        Serial.printf("[HTTP] GET... failed, error: %s (code: %d)\n", http.errorToString(httpCode).c_str(), httpCode);
                    }
                } else {
                    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
                }
                http.end();

                // --- Sending a POST request with JSON ---
                String post_endpoint = "/another_resource"; // e.g., "/api/Message" for cloud
                http.begin(server_url + post_endpoint); 
                http.addHeader("Content-Type", "application/json");
                // For cloud API with x-functions-key:
                // http.addHeader("x-functions-key", "YOUR_API_KEY_HERE");

                // Example JSON payload construction (can also use ArduinoJson)
                char json_buffer[200];
                float temp_val = 25.5; float hum_val = 60.2;
                // For simple JSON, snprintf is efficient:
                // const char *json_template = "{\"temperature\": %.1f, \"humidity\": %.1f}";
                // snprintf(json_buffer, sizeof(json_buffer), json_template, temp_val, hum_val);
                // For BIP Functions API example:
                const char *json_template_message = "{\"sender\":\"YourName\",\"recipient\":\"TargetName\",\"messageText\":\"temperature: %.1f\"}";
                snprintf(json_buffer, sizeof(json_buffer), json_template_message, temp_val);


                httpCode = http.POST(json_buffer); // Send the content of json_buffer
                if (httpCode > 0) {
                    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) { // 200 or 201
                        String payload = http.getString();
                        Serial.println("POST Response: " + payload);
                    } else {
                         Serial.printf("[HTTP] POST... failed, error: %s (code: %d)\n", http.errorToString(httpCode).c_str(), httpCode);
                    }
                } else {
                    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
                }
                http.end();
                ```
            *   **Communication Structure:**
                *   **HTTP Request (Client to Server):**
                    1.  **Mandatory First Line:** `METHOD PATH VERSION`
                        *   `METHOD`: e.g., `GET` (retrieve data), `POST` (send new data), `PUT` (update existing data), `DELETE` (remove data).
                        *   `PATH`: Resource on the server (e.g., `/resource`, or just `/` for default).
                        *   `VERSION`: e.g., `HTTP/1.1` (will be the focus).
                    2.  **Optional Headers (Attributes):** Lines of `name: value` pairs.
                        *   `User-Agent`: Client browser/OS info.
                        *   `Content-Type`: Type of data in the payload (e.g., `text/plain`, `application/json`). **Required for POST/PUT.**
                        *   `Content-Length`: Size of the payload in bytes. **Required for POST/PUT.**
                        *   `x-functions-key`: Custom header for authorization with Azure Functions API.
                    3.  **Mandatory Empty Line:** (CRLF - Carriage Return, Line Feed)
                    4.  **Optional Payload:** Actual data being sent (e.g., for `POST`, `PUT`). Not used by `GET` or `DELETE`.
                *   **HTTP Response (Server to Client):**
                    1.  **Mandatory First Line:** `VERSION STATUS_CODE STATUS_TEXT`
                        *   `VERSION`: e.g., `HTTP/1.1`.
                        *   `STATUS_CODE`: 3-digit code indicating result.
                            *   `1xx`: Informational (e.g., `100 Continue`, `101 Switching Protocols`).
                            *   `2xx`: Success (e.g., `200 OK`, `201 Created`, `202 Accepted`, `206 Partial Content`). **This is desired.**
                            *   `3xx`: Redirection (e.g., `301 Moved Permanently`). Client browser usually handles this.
                            *   `4xx`: Client Error (e.g., `400 Bad Request`, `401 Unauthorized`, `403 Forbidden`, `404 Not Found`).
                            *   `5xx`: Server Error (e.g., `500 Internal Server Error`).
                        *   `STATUS_TEXT`: Human-readable message (e.g., "OK", "Not Found").
                    2.  **Optional Headers (Attributes):** Similar to request headers.
                    3.  **Mandatory Empty Line:** (CRLF)
                    4.  **Optional Payload:** Data returned by the server (e.g., HTML page, JSON data).
            *   **HTTPS is crucial for security.** Requires:
                *   Specifying `https://` in the URL.
                *   For public, trusted servers, ESP32's `http.begin(url_string_starting_with_https)` often works directly.
                *   For servers with self-signed certificates or private CAs, you might need to provide a **Root CA certificate** to `http.begin(url, root_ca_certificate_string)`.
                *   The example code `client.setInsecure();` or `verify=False` (Python) bypasses certificate validation for easier testing but is **not secure for production**. The comment `// kako napraviti HTTPS poziv sa raznim nivoima sigurnosti, sa ili bez sertifikata servera` (how to make an HTTPS call with various security levels, with or without a server certificate) and a link to BearSSL examples points to more secure methods.
                *   **Certificates:** Digital documents that bind a public key to an identity.
                    *   Can be purchased (e.g., from GoDaddy, Verisign) or obtained for free (e.g., from **Let's Encrypt** - valid for 3 months, requires automated renewal).
            *   **Common Request Headers:**
                *   `http.addHeader("Content-Type", "application/json");` for sending JSON data.
                *   `http.addHeader("Authorization", model.iotClientAuthHeader);` for API keys or tokens.
                *   `http.addHeader("x-functions-key", api_key_string);` for Azure Functions.
                *   `http.addHeader("Connection", "close");` is a common practice.
        *   **Data Formats for Payloads:**
            *   **JSON (JavaScript Object Notation):**
                *   Simple, text-based, human-readable, and machine-parsable.
                *   **Most commonly used format for IoT data exchange with WebAPIs.**
                *   Structure: Objects (`{}`), arrays (`[]`), key-value pairs (`"key": "value"` or `"key": number`). Keys are strings in double quotes. String values are in double quotes.
                *   Example: `{"id":1, "temperature":23.5, "humidity":55.0, "device_position":"living room"}`
                *   **ArduinoJson Library (`#include "ArduinoJson.h"`):** Used to create and parse JSON objects on ESP32.
                    *   `StaticJsonDocument<size> doc;` or `DynamicJsonDocument doc(size);` to create a document. Choose size carefully (e.g., `JSON_DOC_SIZE` in example, often 200-1024 bytes).
                    *   `doc["key"] = value;` to add data (e.g., `doc["senderId"] = model.iotClientId;`).
                    *   `serializeJson(doc, charBuffer);` or `serializeJson(doc, StringObject);` to convert to a string for sending.
                    *   `deserializeJson(doc, jsonString);` to parse an incoming string (used in `cli_ConfigLoad`).
                    *   Use `snprintf` to format data into a JSON string template if ArduinoJson is too heavy or for simple cases:
                        ```cpp
                        char buffer[200]; // Ensure buffer is large enough
                        float temp_value = 25.4;
                        // Example for simple temperature JSON:
                        // const char *payload_format = "{\"temperature\": %.1f}"; 
                        // snprintf(buffer, sizeof(buffer), payload_format, temp_value);
                        // Example for BIP Functions API:
                        const char *payload_format = "{\"sender\":\"djordjeL\",\"recipient\":\"dejanaL\",\"messageText\":\"temperature: %.1f\"}";
                        snprintf(buffer, sizeof(buffer), payload_format, temp_value);
                        // Now 'buffer' contains the formatted JSON string
                        // http.POST(buffer);
                        ```
            *   Plain text (`text/plain`)
            *   XML (`text/xml`)
            *   Images, etc.
        *   **MQTT (Message Queue Telemetry Transport):**
            *   Lightweight publish/subscribe messaging protocol, ideal for IoT.
            *   **Architecture:**
                *   **Broker:** A central server that receives messages from publishers and forwards them to subscribers. (e.g., Mosquitto).
                *   **Client (Publisher):** Sends messages to specific "topics" on the broker.
                *   **Client (Subscriber):** Subscribes to "topics" on the broker to receive messages published to those topics.
                *   **Topic:** A hierarchical string (e.g., `home/livingroom/temperature`) that acts as a channel for messages.
            *   **Key Features for Physical Labs:**
                *   **Asynchronous:** Publisher sends a message and doesn't wait for subscribers. Subscribers receive messages when available.
                *   **Reliability (QoS - Quality of Service):**
                    *   **QoS 0 (At most once):** "Fire and forget." No guarantee of delivery. Messages are not persisted on the broker if the client is offline.
                    *   **QoS 1 (At least once):** Broker ensures message is delivered at least once. Messages are queued if client is offline. *May result in duplicate messages.*
                    *   **QoS 2 (Exactly once):** Strongest guarantee. Ensures message is delivered exactly once using a more complex handshake. *Higher overhead.*
                *   **Security:**
                    *   **Encryption:** Use TLS/SSL to encrypt communication between client and broker.
                    *   **Authentication:** Clients authenticate to the broker (e.g., username/password).
                    *   **Authorization:** Broker can control which clients can publish/subscribe to which topics.
            *   **Using PubSubClient Library on ESP32:**
                ```cpp
                #include <WiFi.h>
                #include <PubSubClient.h>

                // WiFi credentials
                const char* ssid = "YOUR_SSID";
                const char* password = "YOUR_PASSWORD";

                // MQTT Broker details
                const char* mqtt_broker = "BROKER_IP_OR_HOSTNAME"; // e.g., "192.168.1.100" or "test.mosquitto.org"
                const int mqtt_port = 1883; // Default MQTT port (1883 for unencrypted, 8883 for TLS)
                const char* mqtt_topic_pub = "esp32/data";
                const char* mqtt_topic_sub = "esp32/commands"; // Or same as pub topic
                const char* mqtt_username = "YOUR_MQTT_USERNAME"; // If broker requires auth
                const char* mqtt_password = "YOUR_MQTT_PASSWORD"; // If broker requires auth

                WiFiClient espClient; // For unencrypted MQTT
                // For encrypted MQTT (TLS):
                // WiFiClientSecure espClientSecure; 
                // espClientSecure.setCACert(root_ca_mqtt_broker); // If needed
                // PubSubClient client(espClientSecure);


                PubSubClient client(espClient);

                // Callback function for received messages
                void callback(char* topic, byte* payload, unsigned int length) {
                  Serial.print("Message arrived [");
                  Serial.print(topic);
                  Serial.print("] ");
                  String messageTemp;
                  for (int i = 0; i < length; i++) {
                    messageTemp += (char)payload[i];
                  }
                  Serial.println(messageTemp);
                  // Add logic to handle the received message
                }

                void reconnect_mqtt() {
                  while (!client.connected()) {
                    Serial.print("Attempting MQTT connection...");
                    String clientId = "ESP32Client-"; // Create a unique client ID
                    clientId += String(random(0xffff), HEX); // Or use MAC address for more uniqueness
                    
                    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
                      Serial.println("connected");
                      client.publish(mqtt_topic_pub, "ESP32 Connected");
                      client.subscribe(mqtt_topic_sub);
                    } else {
                      Serial.print("failed, rc=");
                      Serial.print(client.state());
                      Serial.println(" try again in 5 seconds");
                      delay(5000);
                    }
                  }
                }

                void setup() {
                  Serial.begin(115200);
                  // Connect to WiFi (code from above)
                  // ... WiFi.begin(ssid, password); ... wait for WL_CONNECTED ...

                  client.setServer(mqtt_broker, mqtt_port);
                  client.setCallback(callback); // Register the callback function
                }

                void loop() {
                  if (!client.connected()) {
                    reconnect_mqtt();
                  }
                  client.loop(); // IMPORTANT: Allows the client to process incoming messages and maintain connection

                  // Example: Periodically publish data
                  // static unsigned long lastMsg = 0;
                  // if (millis() - lastMsg > 5000) {
                  //   lastMsg = millis();
                  //   float temp = 25.0; // Read actual sensor
                  //   char msg[50];
                  //   snprintf(msg, 50, "{\"temperature\": %.2f}", temp);
                  //   client.publish(mqtt_topic_pub, msg);
                  // }
                }
                ```
            *   **Mosquitto Broker (Linux Setup for Testing):**
                *   Install:
                    *   `sudo apt-get update`
                    *   `sudo apt-get install mosquitto -y`
                    *   `sudo apt-get install mosquitto-clients -y` (for `mosquitto_sub` and `mosquitto_pub`)
                *   Configure (edit `/etc/mosquitto/mosquitto.conf`):
                    *   `allow_anonymous false` (disable anonymous access for security)
                    *   `password_file /etc/mosquitto/pwfile`
                    *   `listener 1883` (default port)
                *   Create password file:
                    *   `sudo mosquitto_passwd -c /etc/mosquitto/pwfile your_mqtt_username` (prompts for password; omit `-c` for adding more users)
                *   Restart Mosquitto: `sudo systemctl restart mosquitto`
                *   Test subscription: `mosquitto_sub -h localhost -u your_mqtt_username -P your_mqtt_password -t "some/topic"`
                *   Test publishing: `mosquitto_pub -h localhost -u your_mqtt_username -P your_mqtt_password -t "some/topic" -m "Hello MQTT"`
    *   **Device-to-Device Communication (for Game Assignment):**
        *   **ESPNow:** **Recommended method** for direct communication between ESP32 devices.
            *   Requires knowing the MAC address of the peer ESP32.
            *   A `send` function sends a binary buffer.
            *   The receiving device is notified. No intermediaries.
        *   **MQTT:** An alternative, using a Mosquitto broker (can be run on a student's laptop with a local AP provided in the lab).
    *   **BIP2024NS Messaging API (Hosted on Azure Functions):**
        *   **Base URL:** `https://bipfunctions2024.azurewebsites.net/api/`
        *   **Endpoints:**
            *   `GET .../api/Time`: Gets UTC time (no authorization).
                *   `GET .../api/Time?format=unix`: Gets UTC time as a Unix timestamp (long integer).
            *   `GET .../api/names`: Gets all sender/recipient names (requires authorization).
            *   `GET .../api/Message`: Gets messages (requires authorization).
                *   Query Params: `recipient=name`, `fromend=n`, `count=k`, `sender=name`.
            *   `POST .../api/Message`: Posts a new message (requires authorization).
                *   Body: JSON `{"sender": "your_name", "recipient": "target_name", "messageText": "your_message"}`.
        *   **Authorization:**
            *   Required for all functions except `/Time`.
            *   Add HTTP Header: `x-functions-key: W2I3RXCAg7HhfgUcKYSpWy13STX6fAWwd7HhPvCxqG8AzFu_4n3cA==` (This is the "Student2025" API key).
        *   **JSON Message Object Structure (when retrieving messages):**
            *   `Id` (int, Key)
            *   `Sender` (Text(25), Sender's name)
            *   `Recipient` (Text(25), Recipient's name)
            *   `MessageText` (Text(100), Message text)
            *   `Time` (DateTime, Message creation time)
            *   `RowVersion` (Byte array, Timestamp)
    *   **WeatherAPI (Free Public API for Practice):**
        *   Website: [https://www.weatherapi.com/](https://www.weatherapi.com/)
        *   Requires an API Key (get from their website).
        *   Example GET request: `https://api.weatherapi.com/v1/current.json?key=YOUR_API_KEY&q=Novi Sad&aqi=no`
        *   Returns current weather data in JSON format.
    *   **Error Handling:** Always check WiFi status (`WiFi.status() == WL_CONNECTED`) and MQTT connection status (`client.connected()`) before network operations. Check HTTP response codes.
*   **Home Automation (Example Platform):**
    *   **Home Assistant:** Showcased for integrating various sensors and devices, providing a dashboard for monitoring and control.
*   **Output Formatting:**
    *   `sprintf` and `snprintf` functions: Useful for formatting numbers and text before printing to Serial or a display (e.g., ensuring consistent width, adding leading zeros, formatting floats into JSON strings).
        *   `snprintf(buffer, buffer_size, format_string, variables...)`
        *   Need a `char` buffer (`char buf[size];`) to store the formatted output.
        *   **Be careful with buffer size!** `snprintf` is safer than `sprintf` as it prevents overflow.
        *   `Serial.print(buffer)` to output the formatted text.
    *   Format specifiers (e.g., `%d` for decimal integer, `%s` for string, `%.1f` or `%.2f` for float with 1 or 2 decimal places, `%02d` for decimal integer with leading zero to minimum width 2).

## Networking Configuration & Setup

These are fundamental for enabling network communication on your ESP32.

*   **Required Parameters for TCP/IP Communication:**
    *   **DNS (Domain Name Service) Server Address:** Translates human-readable domain names (e.g., `www.google.com`) into numerical IP addresses. Google's public DNS is `8.8.8.8`.
    *   **Gateway Address:** The IP address of the router/device that connects your local network to the wider internet. Usually, this is your WiFi AP.
    *   **Network Mask (Subnet Mask):** Defines the boundary of your local network (e.g., `255.255.255.0`). Helps determine if a target IP is local or external.
*   **DHCP (Dynamic Host Configuration Protocol):**
    *   **Crucial for ease of use:** When your ESP32 connects to a WiFi AP that has DHCP enabled (most do), the AP **automatically assigns** an IP address to your ESP32 and provides the necessary DNS server, Gateway address, and Subnet Mask.
    *   **This is the default and recommended setup for the labs,** meaning you generally won't need to manually configure these network parameters in your ESP32 code.
*   **Local Network Communication:**
    *   Devices connected to the **same WiFi AP** are on the same local network.
    *   They can communicate with each other directly (using their local IP addresses) **without needing a gateway**.
*   **Internet Communication:**
    *   To communicate with devices/servers **outside** your local network (on the internet), traffic must pass through the **Gateway**.

## Digital Electronics Fundamentals

These are key skills and knowledge areas that will be applied directly in the hands-on sessions.

*   **Ohm's Law:** Fundamental relationship: **`U = I * R`** (Voltage = Current × Resistance).
    *   Can be rearranged: `I = U / R` or `R = U / I`.
    *   Essential for all circuit calculations, especially for determining resistor values.
*   **Resistors:**
    *   **Function:** Limit current flow, adjust signal levels, divide voltage.
    *   **Power Rating:** Indicates how much power (heat) a resistor can dissipate. For typical digital electronics with MCUs, **1/8W resistors** are common. Larger physical size often means higher power rating (e.g., 1/4W, 1/2W, 1W, 10W, 20W ceramic resistors for higher power).
    *   **Color Codes:** Physical resistors are marked with colored bands to indicate their resistance value and tolerance. You'll need to use charts to decode these in the lab.
    *   **Series Connection:** Total resistance is the sum: `R_eq = R1 + R2 + ... + Rn`.
    *   **Parallel Connection:** Total resistance is `R_eq = 1 / (1/R1 + 1/R2 + ... + 1/Rn)`. For two resistors: `R_eq = (R1 * R2) / (R1 + R2)`.
*   **Current Limiting for LEDs:**
    *   **Calculation Example (using Ohm's Law):**
        *   Source Voltage (V<sub>S</sub>): e.g., 5V (from ESP32/Arduino).
        *   LED Forward Voltage (V<sub>F</sub>): e.g., 1.8V for a red LED (from datasheet/table).
        *   Desired LED Current (I<sub>LED</sub>): e.g., 10mA (0.01A).
        *   Voltage across resistor (V<sub>R</sub>) = V<sub>S</sub> - V<sub>F</sub> = 5V - 1.8V = **3.2V**.
        *   Required Resistance (R) = V<sub>R</sub> / I<sub>LED</sub> = 3.2V / 0.01A = **320 Ω**.
        *   Choose the closest standard resistor value available (e.g., 330 Ω is common and acceptable).
*   **Voltage Divider Circuits:**
    *   A circuit with two (or more) resistors in series used to obtain a lower voltage from a higher voltage source.
    *   The voltage is distributed across the resistors in proportion to their resistance values.
    *   Example: With a 5V source and two equal resistors (e.g., 1kΩ and 1kΩ), the voltage across each will be 2.5V. If one resistor is 2kΩ and the other is 1kΩ, the 5V will be split in a 2:1 ratio (approx. 3.33V and 1.67V).
    *   A **potentiometer** is a practical implementation of a variable voltage divider.
    *   **Use Case:** Getting a specific analog voltage level for an MCU input.
*   **Voltage Scaling & Logic Level Shifters:**
    *   While voltage dividers *can* scale down voltages (e.g., 5V to 3.3V for ESP32 inputs), they consume additional energy (current flows through resistors).
    *   **A more appropriate way for scaling digital logic levels** (e.g., between a 5V sensor and a 3.3V ESP32) is using dedicated **Logic Level Shifters**.
    *   These are ICs or modules designed for this purpose and are more efficient for digital signals.
*   **Pull-up and Pull-down Resistors:**
    *   Used for digital inputs (like buttons) to ensure a defined state (HIGH or LOW) when the button is not pressed, preventing a "floating" input which can lead to random readings.
    *   **`pinMode(pin, INPUT_PULLUP)`:** Activates an internal pull-up resistor in Arduino/ESP32. The pin is connected to Vcc (e.g., 3.3V or 5V) through this resistor.
        *   When a button connected between the pin and Ground is **not pressed (open)**, the pin reads **HIGH**.
        *   When the button is **pressed (closed)**, it connects the pin to Ground, and the pin reads **LOW**.
    *   **Pull-down Resistor Circuit:** An external resistor connects the input pin to Ground. The button would then connect the pin to Vcc when pressed.
        *   When a button connected between the pin and Vcc is **not pressed (open)**, the pin reads **LOW**.
        *   When the button is **pressed (closed)**, it connects the pin to Vcc, and the pin reads **HIGH**.
    *   **Common Practice:** `INPUT_PULLUP` is frequently used as it simplifies wiring (button connects to GND).
    *   Typical external pull-up/pull-down resistor values range from **30kΩ to 50kΩ**.
    *   **Important ESP32 Note:** Pins **34-39** are input-only and **do NOT have internal pull-up or pull-down resistors**. If using these pins for buttons, you **must** use external pull-up or pull-down resistors. (BIPBox Key1=39, Key2=36 will need external pull-ups/downs if not already on the PCB).
*   **Transistors & Relays (Brief Awareness):**
    *   **Transistors:** Used as electronic switches or amplifiers. Essential for managing circuits where the current or voltage needed by a device (e.g., a motor) surpasses the limits of an MCU pin.
    *   **Relays (Electromechanical & Solid-State):** Electrically operated switches. Used to control high-power circuits (e.g., AC lighting, heaters, powerful motors) using a low-power signal from an MCU. They provide isolation between the control circuit and the load circuit.
*   **Sensor Data Acquisition & Processing (Example: `taskWaterPressure.cpp`):**
    *   Reading data from analog sensors (e.g., Potentiometer via `analogRead(PIN_WATERPRESSUREANALOG)`).
    *   **Smoothing Data (Moving Average):**
        *   Use a circular buffer (e.g., `int readings[16];`) to store recent sensor readings.
        *   Periodically calculate the sum/average of buffer contents: `ind = (ind + 1) % 16; ... sum=0; for(...) sum+=readings[i]; model.analogValue = sum/16;`.
        *   This helps reduce noise and get a more stable reading.
    *   **Calibration & Conversion to Physical Units:**
        *   **Voltage Conversion:** Convert raw ADC value (0-4095 for ESP32 12-bit ADC) to voltage: `volt = model.analogValue * (3.3 / 4095.0)`.
        *   **Voltage Correction (Empirical):** ESP32 ADC can be non-linear. A small offset might be needed (e.g., `volt += 0.16;` in the example). This value is often found by comparing ADC readings to actual multimeter readings.
        *   **Sensor-Specific Formula:** Apply formulas from datasheets or derived through calibration to convert voltage/current to the physical unit (e.g., pressure in Bar from sensor current: `pressure = 375 * (current_amps - 0.004)` for a 4-20mA, 0-6 bar sensor).
        *   Document these formulas and their derivations in comments (e.g., `// P - pritisak u Bar` (Pressure in Bar), `// R - otpor u Om` (Resistance in Ohm), `// A - analog value za napon, 0V -> 0; 3.3V -> 4095.` (Analog value for voltage, 0V -> 0; 3.3V -> 4095.), `// Ili kraće, P = 0.001679 * A - 1.5` (Or shorter, P = 0.001679 * A - 1.5)).
    *   **Outlier Detection:** A C++ code example illustrated calculating mean, standard deviation, and z-score to identify outliers in sensor readings (e.g., from a potentiometer).
*   **Displaying Data on LCD:**
    *   Includes using the `LiquidCrystal_I2C` library.
    *   Initializing the LCD object (`LiquidCrystal_I2C LCD(...)`, providing I2C address, columns, and rows).
    *   Initializing the LCD (`LCD.init()`).
    *   Turning on the backlight (`LCD.backlight()`).
    *   Setting the cursor position (`LCD.setCursor(col, row)`).
    *   Printing text or formatted values (`LCD.print(text)`).
    *   **Managing Screen Content:** Printing shorter strings or using spaces to clear previous text at a position is important for dynamic updates.
*   **Embedded Programming Paradigms (C/C++ for Arduino/ESP32):**
    *   **Imperative Programming:** Programs are a set of instructions executed sequentially. Some instructions can be conditional.
    *   **Arduino Program Structure:**
        *   `void setup() { ... }`: Runs once at the beginning. Used for initialization (e.g., setting pin modes, `Serial.begin()`, initializing machine state, creating FreeRTOS tasks).
        *   `void loop() { ... }`: Runs repeatedly forever. This is the main control loop mimicking the flow of time and implementing the system's ongoing behavior. When using FreeRTOS, this loop might be minimal or even just contain a delay, with tasks handling operations.
    *   **Finite State Machines (FSMs):** A crucial concept for designing reactive embedded systems (like the traffic light controller example).
        *   **State:** The current configuration/condition of the system (e.g., "Green Light ON"). Represented by a variable (e.g., `currentState`).
        *   **Events:** Triggers that cause transitions between states (e.g., button press, timer expiry).
        *   **Transitions:** Rules defining how the system moves from one state to another based on events and current state. Implemented using a **State Transition Function** (e.g., `StateType NextStateFunction(currentState, inputs)`).
        *   **Actions/Outputs:** Operations performed when entering a state or during a transition. Implemented using an **Outputs Function** (e.g., `OutputType OutputsFunction(currentState)`).
        *   **Timers:** Used for delays and timed state transitions. Arduino's `delay(milliseconds)` function is a simple way to implement pauses.
    *   **Multitasking (Essential for Concurrent Operations):**
        *   Doing multiple things "simultaneously" (e.g., blinking LEDs *and* reading input).
        *   **"Manual" Multitasking (using `millis()`):**
            *   The **`delay()` function is blocking** – it halts the entire CPU. **Avoid `delay()`** when multiple independent actions need to occur.
            *   **Use the `millis()` function** for non-blocking timing.
            *   `millis()` returns the number of milliseconds since the board started.
            *   To perform an action periodically (e.g., every 5 seconds for DHT sensor):
                ```cpp
                static unsigned long lastActionTime = 0; // Or initialize with millis() in setup
                unsigned long currentMillis = millis();
                if (currentMillis - lastActionTime >= 5000) { // 5000 ms interval
                    // Perform action (e.g., read DHT sensor)
                    lastActionTime = currentMillis; // Update the last action time
                }
                ```
            *   Use **`unsigned long`** for `millis()` variables to avoid overflow issues (occurs after ~50 days).
        *   **Multitasking with FreeRTOS on ESP32 (ESP-IDF):**
            *   The ESP32 comes with FreeRTOS, a real-time operating system, allowing for more robust multitasking. This will be crucial for more complex projects and games.
            *   **Core Concepts:**
                *   **Tasks:** Independent C functions that run (seemingly) in parallel. Each task is typically an infinite loop (e.g., `for(;;)` or `while(1)`). Tasks have their own Program Counter (PC), registers, and a **private stack**. Tasks must make calls to the scheduler (e.g., `vTaskDelay`) to allow context switching.
                    *   Example: `TaskWaterPressure` for sensor reading, `TaskIoTClient` for cloud communication, `TaskCLI` for command line interface. Forward declarations in `main.cpp`: `// forward deklaracije taskova koji su u zasebnim fajlovima` (forward declarations of tasks that are in separate files).
                *   **Scheduler:** Manages the execution of tasks based on a priority scheme, giving the appearance of simultaneous execution through **context switching**.
                *   **Task Stack:**
                    *   Each task has its **own private stack**.
                    *   This stack is used to store:
                        *   Local variables of the task's function and any functions it calls.
                        *   Function call parameters, return addresses, and saved processor registers (context switching information). This forms an "Activation Record" (AR) for each function call within the task.
                    *   It's **critical** to allocate a sufficient `stack_size` when creating a task. If the stack overflows (runs out of space due to deep function calls or large local variables), your program will likely crash.
                    *   The `1000` in the `xTaskCreate` example refers to stack size (often in bytes in Arduino wrappers, but can be words in native FreeRTOS - **check documentation for your specific environment, e.g., ESP-IDF**). This may need adjustment based on task complexity. A common starting point for simple tasks is 2048 or 4096 bytes.
            *   **Key FreeRTOS Functions (for ESP32/Arduino):**
                *   **Task Creation:** Typically done in `setup()`.
                    *   `xTaskCreate()` or `xTaskCreatePinnedToCore()`:
                        ```cpp
                        // General syntax for xTaskCreate (parameters might vary slightly by wrapper)
                        xTaskCreate(
                            TaskFunction_t pvTaskCode,     // Pointer to the task function (e.g., void myTask(void *pvParameters))
                            const char * const pcName,     // A descriptive name for the task (for debugging)
                            const configSTACK_DEPTH_TYPE usStackDepth, // Stack size (e.g., 1000, 2048, 4096 bytes or words)
                            void * const pvParameters,     // Pointer to parameters to pass to the task (can be NULL)
                            UBaseType_t uxPriority,        // Priority of the task (e.g., 0 is lowest, configMAX_PRIORITIES-1 is highest)
                            TaskHandle_t * const pvCreatedTask // Optional: Pointer to a handle to the created task (for control like suspend/resume) - can be NULL
                        );
                        ```
                        *   Example from lecture: `xTaskCreate(toggleLED, "Toggle LED", 1000, NULL, 1, NULL);`
                        *   For ESP32 with multiple cores, `xTaskCreatePinnedToCore()` is often used to assign a task to a specific core (e.g., `APP_CPU_NUM` (core 1) for application tasks, `PRO_CPU_NUM` (core 0) for protocol stacks like WiFi/Bluetooth).
                *   **Task Delay (Non-Blocking Yield):**
                    *   `vTaskDelay(xTicksToDelay)`: Suspends the calling task for a specified number of RTOS ticks, allowing other tasks to run. **This is the preferred way to delay within a task and yield control.**
                    *   To convert milliseconds to ticks: `pdMS_TO_TICKS(xTimeInMs)` or `(milliseconds / portTICK_PERIOD_MS)`.
                    *   Example from `taskWaterPressure.cpp`: `vTaskDelay(700 / portTICK_PERIOD_MS); // Čekamo 700ms pre narednog merenja` (Wait 700ms before next measurement).
                *   **Task Control (using Task Handles):**
                    *   `vTaskSuspend(TaskHandle_t xTaskToSuspend)`: Suspends a specific task. If `NULL` is passed, it suspends the *calling* task.
                    *   `vTaskResume(TaskHandle_t xTaskToResume)`: Resumes a suspended task. Can only be called from *outside* the task being resumed.
            *   **RTOS Task States (Practical for Physical Labs):**
                *   **Running:** The task is currently executing on the CPU.
                *   **Ready:** The task is able to execute but is waiting for the scheduler to give it CPU time.
                *   **Blocked:** The task is waiting for an event (e.g., `vTaskDelay` expiry, data in a queue, semaphore availability, mutex unlocked).
                *   **Suspended:** The task has been explicitly paused using `vTaskSuspend()` and will only become **Ready** again after a call to `vTaskResume()`.
                *   *Understanding these states helps in debugging multitasking applications.*
            *   **Simple Task Example (Blinking LED with FreeRTOS):**
                ```cpp
                // Define LED pin, e.g., const int ledPin = LED_BUILTIN; // Or your specific LED pin

                // Task function
                void blinkTask(void *parameter) {
                  for (;;) { // Infinite loop for the task
                    digitalWrite(ledPin, HIGH);
                    vTaskDelay(500 / portTICK_PERIOD_MS); // Wait 500ms, allows other tasks to run
                    digitalWrite(ledPin, LOW);
                    vTaskDelay(500 / portTICK_PERIOD_MS); // Wait 500ms, allows other tasks to run
                  }
                }

                // In setup()
                void setup() {
                  pinMode(ledPin, OUTPUT);
                  Serial.begin(115200); // Initialize Serial for debugging

                  xTaskCreate(
                    blinkTask,        // Task function to execute
                    "Blink Task",     // Name of the task (for debugging)
                    1000,             // Stack size (bytes or words, check ESP-IDF docs) - adjust as needed!
                    NULL,             // Parameter to pass to task (NULL if not needed)
                    1,                // Priority (0 is lowest)
                    NULL              // Task handle (NULL if not needed for external control)
                  );
                  // You might create other tasks here as well
                }

                // loop() can be empty or do minimal work if FreeRTOS tasks handle everything
                void loop() {
                  // vTaskDelay(1000 / portTICK_PERIOD_MS); // Example: keep loop() from spinning too fast if it has work
                }
                ```
            *   **FreeRTOS Resources (Check Teams for these):**
                *   PDF: "**Free RTOS Resources.pdf**" (by Prof. Djordje Herceg) - contains links to:
                    *   Video: "Multitasking on ESP32 with Arduino and FreeRTOS | Simply Explained"
                    *   Video: "Manage FreeRTOS tasks - Suspend, Delay, Resume, Delete | Simply Explained"
                    *   GitHub examples for Arduino FreeRTOS:
                        *   `AnalogRead/Blink` (parallel tasks)
                        *   `StructQueue` (inter-task communication using **Queues**)
                        *   `Mutex` (synchronization primitive)
                *   Other PDFs: `FreeRTOS_Doc1.pdf`, `FreeRTOS_Doc2.pdf`, `Mastering-the-FreeRTOS-Real-Time-Kernel-v1.1.0.pdf` (for deeper dives into Queues, Semaphores, Mutexes, Task Control Blocks (TCB), etc.).
    *   **Button Debouncing:**
        *   Physical buttons produce multiple transitions due to mechanical bounce.
        *   Software debouncing is needed to register a single press/release event reliably.
        *   A common method involves checking the button state only after a short period (e.g., 50-100 ms) has passed since the last registered change (`if (millis() - lastDebounceTime > debounceDelay)`).
    *   **Key Arduino Functions:**
        *   `pinMode(pinNumber, mode)`: Configures a pin. `OUTPUT` for LEDs, `INPUT` or **`INPUT_PULLUP`** for buttons.
        *   `digitalRead(pinNumber)`: Reads the value from a specified digital pin (HIGH or LOW).
        *   `digitalWrite(pinNumber, value)`: Writes a HIGH or a LOW value to a digital pin.
        *   `analogRead(pinNumber)`: Reads the value from the specified analog pin.
        *   `delay(ms)`: Pauses the program for the amount of time (in milliseconds) specified as parameter. **Blocking.**
    *   **Program/Machine Interaction:** The program (the "brains") controls the machine by:
        1.  Reading inputs (from sensors, internal variables).
        2.  Writing to outputs (actuators, internal variables).
        3.  Reading/writing intermediate data to its internal memory (variables) to maintain state and make decisions.
*   **Command Line Interface (CLI) for Configuration (Example: `cmdLine.cpp`, `cliCommands.cpp`):**
    *   **Purpose:** Allows changing device parameters (WiFi credentials, server URLs, API keys, operational settings) via the Serial Monitor without recompiling and re-uploading firmware. **Extremely useful for the physical labs.**
    *   **Implementation:**
        *   A dedicated FreeRTOS task (`TaskCLI`) can handle reading from Serial and processing commands.
        *   Use a custom serial buffer class (e.g., `SerialBuf` from `SerialBuf.h` and `SerialBuf.cpp`) to handle line-by-line input or input with timeouts.
            *   `sbuf.loop()`: Called repeatedly to check for incoming serial data.
            *   `sbuf.available()`: Checks if a complete command line is ready.
        *   A command processor class (e.g., `CmdProc` from `CmdProc.h` and `CmdProc.cpp`) to parse commands and their arguments.
            *   `cmdProc.Add(command_string, callback_function, min_tokens, max_tokens)`: Registers a command and its handler (e.g., `cmdProc.Add("setssid", cmd_SetSsid, 2, 2);`).
            *   `cmdProc.Parse(input_buffer)`: Parses the input string, identifies the command, and calls the associated callback.
            *   Comment in `cmdline.cpp`: `// komandni procesor` (command processor).
        *   Callback functions for each command (e.g., `cmd_SetSsid`, `cmd_SetPassword`, `cmd_SetIotServerUrl1` in `cliCommands.cpp`) implement the logic to update the configuration (often stored in a global `model` object).
*   **Persistent Configuration Storage (Example: `cliCommands.cpp`):**
    *   **Purpose:** To save settings so the device remembers them after a power cycle or reboot.
    *   **Method:** Use the **SPIFFS (SPI Flash File System)** available on ESP32.
        *   `#include <SPIFFS.h>`
        *   **Initialization:** `SPIFFS.begin(true);` (the `true` formats SPIFFS if it's not already formatted).
        *   **Saving Configuration (`cli_ConfigSave()`):**
            1.  Create a JSON document (e.g., using ArduinoJson: `StaticJsonDocument<JSON_DOC_SIZE> doc;`).
            2.  Populate the JSON document with values from your `model` or configuration variables (e.g., `doc["wifi_ssid"] = model.wifiSsid; doc["iotserverurl1"] = model.iotServerUrl1;`).
            3.  Open a file for writing: `File file = SPIFFS.open("/config.txt", FILE_WRITE);`.
            4.  Serialize the JSON document to the file: `serializeJson(doc, file);`.
            5.  Close the file: `file.close();`.
        *   **Loading Configuration (`cli_ConfigLoad()`):**
            1.  Open the file for reading: `File file = SPIFFS.open("/config.txt", FILE_READ);`.
            2.  Check if the file exists and has content (`if (file && file.size())`).
            3.  Deserialize the JSON from the file into a JSON document: `DeserializationError error = deserializeJson(doc, file);`.
            4.  If deserialization is successful, extract values and update your `model` or configuration variables (e.g., `if (doc.containsKey("wifi_ssid")) strncpy(model.wifiSsid, doc["wifi_ssid"], MODEL_WIFI_SSID_LEN);`).
            5.  Close the file: `file.close();`.
        *   Implement CLI commands like `config save`, `config load`, `config showfile`, `config clear` to manage this.

## Data Analysis & Machine Learning (Python)

This section covers tools and techniques relevant for the "Lab: Data recording, Processing and Analysis" and the practical machine learning task. The dataset for the practical task is `water_pressure_dataset.csv` with columns of interest: `SenderID`, `Payload`, `Created`.

### Python Files for Practical Exercises
*   **`Data_analysis.py`:** Provided Python script for initial data analysis (Goal 1).
*   **`lab_water_pressure.py`:** Provided Python script for the machine learning task (Goal 2 and Additional Goal). Contains placeholders (`??????????` or `YOUR CODE HERE`) that need to be filled based on hints.

### Pandas Library
*   **Purpose:** Python library for working with datasets in tabular form. Useful for data cleaning, analysis, manipulation, and visualization.
*   **Import:** `import pandas as pd`
*   **Core Data Structure: DataFrame**
    *   A 2-dimensional labeled data structure, like a table or spreadsheet.
    *   Rows typically represent samples/observations, columns represent features.
    *   Can hold data of different types (numeric, string, boolean, etc.) in different columns.
*   **Key Operations for Physical Labs:**
    *   **Reading Data:**
        *   `df = pd.read_csv('water_pressure_dataset.csv')`
    *   **Viewing Data (Inspecting the DataFrame):**
        *   `df.head()`: Shows the first 5 rows.
        *   `df.info()`: Provides a concise summary (data types, non-null values).
        *   `df.describe()`: Generates descriptive statistics for numerical columns.
    *   **Selecting Data (Indexing):**
        *   Select columns: `df[['SenderID', 'Payload', 'Created']]`.
        *   Conditional selection (filtering): `df[df['SenderID'] == 1]` or `df[df['SenderID'] == 5]`.
    *   **Data Cleaning:**
        *   Handling Missing Values (NaN).
        *   Handling Duplicates.
        *   Resetting index: `df = df.reset_index(drop=True)`.
    *   **Time Series Specific Operations:**
        *   Convert 'Created' column to datetime objects: `pd.to_datetime(df['Created'])`.
        *   Set 'Created' column as index for time series analysis: `df.set_index('Created', inplace=True)`.
        *   **Smoothing Data (Moving Average):**
            *   `df['Payload_MA'] = df['Payload'].rolling(window=...).mean(min_periods=...)` (Hint: `rolling()` method followed by `mean()`. Specify `window` and `min_periods`).
        *   **Creating Lag Features (for time series forecasting):**
            *   `df[f'Payload_Lag_{i}'] = df['Payload_MA'].shift(i)` (Hint: `shift()` method).
    *   **Visualization (uses Matplotlib backend):**
        *   `df['Payload'].plot()`
        *   Plotting ACF: `from statsmodels.graphics.tsaplots import plot_acf; plot_acf(df['Payload_MA'], lags=...)`
        *   Use `matplotlib.pyplot.show()` (often imported as `plt.show()`) to display plots.

### Machine Learning with Scikit-learn (Python)
*   **Purpose:** Python library for various machine learning tasks.
*   **Focus for Physical Labs: Decision Tree Regressor** and **Dense Neural Network (DNN)**.
*   **General Workflow:**
    1.  **Load and Prepare Data** (using Pandas, including feature engineering like time encoding and lag features).
    2.  **Split Data** into training and testing sets (`train_test_split`).
    3.  **Choose and Configure a Model.**
    4.  **Train the Model** (`model.fit(X_train, y_train)`).
    5.  **Make Predictions** (`y_pred = model.predict(X_test)`).
    6.  **Evaluate the Model** (MSE, RMSE).
    7.  **Tune Hyperparameters.**
*   **Feature Engineering for Time Series:**
    *   **Cyclical Time Features:** Encode minutes, hours, day of the week, month using sine and cosine transformations to capture cyclical patterns.
        *   Example for minutes:
            ```python
            df['minute_sin'] = np.sin(2 * np.pi * df['minute'] / 60)
            df['minute_cos'] = np.cos(2 * np.pi * df['minute'] / 60)
            ```
            (Similar logic for hour (period 24), day_of_week (period 7), month (period 12)).
    *   **Lagged Features:** Use past values of the target variable (or smoothed target variable) as input features.
        *   `df['Payload_Lag_1'] = df['Payload_MA'].shift(1)` (previous value)
        *   `df['Payload_Lag_2'] = df['Payload_MA'].shift(2)` (value before previous)
        *   The exercise uses 20 lagged features (`Payload_Lag_1` to `Payload_Lag_20`).
*   **Decision Tree Regressor (`sklearn.tree.DecisionTreeRegressor`):**
    *   **Input Features (`feature_cols`):** `minute_sin`, `minute_cos`, `hour_sin`, `hour_cos`, `day_sin`, `day_cos`, `month_sin`, `month_cos`, and `Payload_Lag_1` through `Payload_Lag_20`.
    *   **Target Variable (`target_col`):** `Payload_MA`.
    *   **Hyperparameters to Tune:**
        *   `max_depth`
        *   `min_samples_leaf`
        *   `criterion='squared_error'` (default for regression)
        *   `random_state` (for reproducibility)
        *   `max_leaf_nodes`
        *   `lookback` (this refers to the number of lagged features, e.g., 20 in the example).
*   **Dense Neural Network (DNN):**
    *   Details of the DNN architecture will likely be provided in the `lab_water_pressure.py` schema.
    *   Will be compared against the Decision Tree Regressor.
*   **Evaluation Metrics:**
    *   **MSE (Mean Squared Error):** `from sklearn.metrics import mean_squared_error; mse = mean_squared_error(y_test, y_pred)`
    *   **RMSE (Root Mean Squared Error):** `rmse = np.sqrt(mse)` or `rmse = mean_squared_error(y_test, y_pred, squared=False)`

## 3D Design & Printing (for Physical Lab)

This section covers information specifically for the "Lab: 3D Design and 3D printing (wearables, devices enclosures, etc.)".

*   **Goals of 3D Printing Relevant to the Lab:**
    *   **Rapid Prototyping:** You will quickly design, manufacture, and test prototypes (e.g., enclosures for your IoT devices). This allows for fast iteration and refinement of your designs.
    *   **On-Demand Manufacturing:** You'll produce items as needed, directly in the lab. This highlights the flexibility of 3D printing for custom parts.
    *   **Complex Geometries:** 3D printing allows you to create shapes and features that would be difficult or impossible with traditional manufacturing methods, which is useful for custom enclosures and wearables.
    *   **Accessibility:** The lab provides access to 3D printers, enabling you to create physical objects without needing expensive industrial machinery.
*   **Additive Manufacturing Methods:**
    *   **Fused Deposition Modeling (FDM):**
        *   This is the **most common and likely method you will use in the lab.**
        *   Process: A plastic filament is melted and extruded through a nozzle, depositing material layer by layer to build the object.
    *   *Other methods like Vat Photopolymerization (resin-based), Sintering (powder-based, for metals/ceramics), and Material Jetting are more industrial and less likely for this lab but good to be aware of.*
*   **3D Printing Workflow (Practical Steps for the Lab):**
    1.  **Designing the Model (e.g., in Autodesk Fusion 360):**
        *   Use **CAD (Computer-Aided Design) software** like **Autodesk Fusion 360**.
        *   **Example: Designing a Key Tag in Fusion 360**
            1.  **Create a 2D Sketch:** Start a new sketch and select a plane (e.g., XY plane) to ensure proper orientation.
            2.  **Draw the Main Shape:**
                *   Use the Rectangle tool to create the base of the tag (e.g., 72mm x 32mm).
                *   Use the Circle tool to make a hole for the keychain ring (e.g., 10mm diameter) and a larger concentric circle (e.g., 20mm diameter) for the rounded end.
                *   Use **Construction Lines** (helper lines, not part of the final profile) to position elements, e.g., a line from the midpoint of the rectangle's side to the center of the circles.
                *   Add **Tangent Lines** to connect the short side of the rectangle to the larger circle for a smooth transition.
                *   **Apply Constraints:** Use constraints like *Coincident, Horizontal, Vertical, Parallel, Perpendicular, Midpoint, Concentric, Tangent, and dimensional constraints (Length, Radius/Diameter)* to precisely define the geometry. This makes the sketch robust to changes.
            3.  **Define a Raised Edge (Optional):** Draw an inner rectangle inside the tag body to create a space for a paper label. This edge will be extruded separately.
            4.  **Finish Sketch.**
            5.  **Extrude to 3D:**
                *   Select the main profile (the base of the tag) and extrude it to the desired depth (e.g., 2mm). This creates the base body.
                *   If a raised edge was defined, select its profile and extrude it further (e.g., to 3mm from the original sketch plane, resulting in a 1mm raised border if the base was 2mm). Ensure the operation is set to "Join" if it's part of the same body.
    2.  **Preparing the File (Slicing - e.g., in Ultimaker Cura):**
        *   Export the 3D model from Fusion 360 (e.g., "Make" command, often exports as `.STL`).
        *   Import the `.STL` file into **slicing software** (e.g., Ultimaker Cura).
        *   Configure print settings:
            *   Select printer model.
            *   Select material (e.g., PLA).
            *   Set **Layer Height** (e.g., 0.2mm).
            *   Set **Infill Density** (e.g., 20%) and **Infill Pattern** (e.g., Cubic, Gyroid, Lines). Infill is the internal support structure that saves material and print time.
            *   Set printing temperatures (nozzle and bed), print speed.
            *   Enable/configure **Supports** if the model has significant overhangs.
            *   Enable/configure **Adhesion** (e.g., Brim, Raft) if needed for better bed sticking.
        *   Click "Slice." The slicer generates **G-code** (a text file with printer instructions).
    3.  **Printing the Object:**
        *   Transfer the G-code file to the 3D printer (e.g., via SD card, USB, WiFi).
        *   The printer executes the G-code, building the object layer by layer.
    4.  **Post-Processing:**
        *   Once printing is complete, carefully remove the object from the print bed.
        *   **Support Removal:** If supports were used, remove them (break, cut, dissolve depending on type).
        *   **"3S" - Sandpaper, Scalpel, and Superglue:**
            *   **Sandpaper:** For smoothing surfaces.
            *   **Scalpel/Craft Knife:** For trimming small imperfections, stringing, or helping remove supports. **Use with caution.**
            *   **Superglue:** For joining multi-part prints or repairing small breaks.
        *   Other finishing: Painting, polishing.
    5.  **Quality Control (QC) & Testing Adjustments:**
        *   Check dimensions, fit, and strength.
        *   Iterate on design or slicer settings if needed.
*   **Key 3D Printing Materials (FDM - likely for the lab):**
    *   **PLA (Polylactic Acid):**
        *   **Go-to material for beginners and general prototyping.**
        *   Easy to print, lower temperatures, less warping. Biodegradable (industrial).
    *   **ABS (Acrylonitrile Butadiene Styrene):**
        *   Stronger, more durable, higher temperature resistance. Requires heated bed & enclosure. Emits fumes.
    *   **PETG (Polyethylene Terephthalate Glycol):**
        *   Good balance of PLA's ease and ABS's strength/temp resistance. Food-safe (material itself).
    *   **TPU (Thermoplastic Polyurethane) / TPC (Thermoplastic Copolyester):**
        *   **Flexible filaments.** For rubber-like parts. Can be trickier to print.
*   **Common Issues in FDM Printing (Be Prepared to Troubleshoot):**
    *   **Material Shrinkage:** Can cause warping or layer separation (especially ABS).
        *   *Solutions: Heated bed, enclosure, brim/raft, proper bed leveling, adhesives.*
    *   **Tolerances:** Parts rarely perfect. Holes often print smaller.
        *   *Solutions: Design with tolerances. Print test pieces.*
    *   **Elephant's Foot:** First few layers bulge outwards.
        *   *Causes: Bed too hot, nozzle too close, first layer over-extrusion.*
        *   *Solutions: Adjust bed temp, Z-offset, first layer extrusion.*
    *   **Corner Bulging:** Corners not sharp, rounded or bulging.
        *   *Causes: Print head/filament roundness, printer slowing at corners while extruding.*
        *   *Solutions: Adjust print speed, acceleration, jerk settings in slicer.*
    *   **Stringing (Oozing):** Thin plastic strands between parts.
        *   *Causes: Material oozing during non-extruding moves (common with PETG).*
        *   *Solutions: Tune retraction settings, adjust print temperature, increase travel speed.*
    *   **Heat Tower:** A special test print used to find the optimal printing temperature for a specific filament to minimize issues like stringing or poor layer adhesion. It's printed with varying temperatures at different heights.
    *   *Refer to online guides (Simplify3D, Basic3DP, 3Dsourced, PrintingIt3D, CleverCreations) for detailed troubleshooting.*
*   **Design Considerations for 3D Printing (Practical for Lab):**
    *   **Enclosures (e.g., for the BIPBox):**
        *   **Component First Design:** Model electronics first, then design enclosure around them.
        *   **Mounting Holes/Bosses:** For securing PCBs with screws.
        *   **Clearances & Tolerances:** Essential for parts to fit.
        *   **Accessibility:** For buttons, ports, batteries.
        *   **Section Analysis (in Fusion 360):** Use this tool to create a cross-section view of your assembly to check internal fits and clearances.
    *   **Overhangs and Supports:** Minimize supports by design (e.g., chamfers, optimal orientation).
    *   **Wall Thickness:** Typically 2-3 perimeters (0.8mm - 1.2mm for a 0.4mm nozzle).
    *   **Stress Analysis (Fusion 360):**
        *   Identifies potential weak spots in load-bearing parts.
        *   Helps optimize material usage.
        *   This feature is available in the **full academic or commercial versions of Fusion 360.**
*   **Provided 3D Design Examples & Files (on Teams/OneNote):**
    *   Videos demonstrating design of a box and a remote holder in Fusion 360.
    *   Fusion 360 files: `10mm pad.f3d`, `BipBox V6 v21.f3z`, `Box.f3d`, `Vox Remote Holder V10.f3d`.
    *   The **BipBox** is the enclosure designed for the course's electronic device.

## Assignments (Focus on Physical Part Execution)

The physical part will involve group work on three main assignments. Development for some aspects, especially the game and data analysis/ML, **must start before the physical part.**

### 1. MCU Programming Problem
*   This will be one of the three assignments per group.
*   Specific details about the problem will be provided, but it will involve MCU programming, likely using C++ on the ESP32/Arduino, potentially implementing concepts like FSMs and multitasking (manual or FreeRTOS).

### 2. Design and Implement a Turn-Based or Interactive Game
*   **Core Task:** Each group will devise and implement a two-player game.
    *   The game should be turn-based or interactive.
    *   It **must** utilize the communication capabilities of the ESP32.
    *   Each player will use a separate **BIPBox**.
*   **Development Timeline:** Can be worked on *during* the in-person part, but **data exchange protocol design and initial C++ coding (including FSM logic for game states and potentially FreeRTOS task structure) can start beforehand based on virtual lecture content.**
*   **Data Exchange Between Devices (BIPBoxes):**
    *   **Recommended Method:** **ESPNow** (most direct for ESP32-to-ESP32).
        *   Requires knowing the hardware MAC address of the peer ESP32.
        *   A `send` function can send a binary buffer.
        *   The receiving device is notified when data arrives.
    *   **Alternative Methods:** MQTT with a Mosquitto broker (can be run on a student's laptop; an access point will be provided in the lab).
    *   **Design Considerations for Data Exchange:**
        *   **Message Types:** What information needs to be sent? (e.g., Player 1 action/position, Player 2 action/position, score, game start/end).
        *   **Payload Structure:** How will data in each message be organized? (e.g., none, fixed length, variable length).
        *   **Serialization Format:** How will the payload be encoded/decoded? (e.g., custom string, binary, JSON).
    *   **Implementation:**
        *   Create **C++ routines** to construct, interpret, and validate messages.
*   **Example Game Concepts (for inspiration):**
    *   **Tilt & Align:**
        *   **Hardware:** Uses **BMI160** (accelerometer + gyroscope) for orientation sensing on each BIPBox. LCD for display.
        *   **Gameplay:** Player 1 sets an orientation, sends it to Player 2. Player 2 tries to match it. Score based on closeness. Roles reverse.
        *   **Feedback:** LCD bars for X,Y,Z angles; audio beeps.
    *   **Quack Attack:**
        *   **Hardware:** Uses a **potentiometer** for input on each BIPBox. LCD for display.
        *   **Gameplay:** Player 1 places a "duck" (1-2 LCD characters) on screen, possibly behind "bushes" (1-2 LCD characters). Player 2 aims with potentiometer to "hit" the duck. If hidden, bush must be hit first. Score based on speed/attempts. Roles reverse.
        *   **No direct visual feedback of aiming position** for the aiming player until an action is taken.
*   **Deliverables for the Game:**
    *   Functional game on two BIPBoxes.
    *   Written material (concepts, data exchange, C++ implementation).
    *   10-15 min presentation + 10-15 min Q&A during physical part.

### 3. Design a 3D Printed Model
*   This assignment will be done **during the in-person part**.
*   Will likely involve using software like **Autodesk Fusion 360**.
*   Focus on creating enclosures, wearables, or other components related to your IoT projects/game.
*   Apply concepts of designing for 3D printing (e.g., component first, tolerances, mounting holes, minimizing supports).

### General Group Assignment Notes
*   Participants will be divided into groups. Groups and assignments will be e-mailed by end of the week (of April 29th).
*   Each group receives all three assignments.
*   Preparatory work (game logic, data exchange protocols) can/should begin during the virtual part. Support will be available during the physical part.

### Homework & Practical Exercises (Preparation for Physical Lab - **Deadlines are important!**)

*   **Statistical Data Processing (Excel Homework):**
    *   **Task:** Analyze `health.xlsx` using **Excel** (BMI, Descriptive Stats, boxplots, Z-scores, outliers).
    *   **Purpose:** Prepares for **"Lab: Data recording, Processing and Analysis"**.
    *   **Deadline:** Complete **before** arriving in Novi Sad.
*   **Python Practical Exercises (Goals 1 & 2 - Deadline: May 2, 2025 - *Virtual Part*):**
    *   **Goal 1: Data Analysis (`Data_analysis.py`, `water_pressure_dataset.csv` for `SenderId = 1`)**
        *   Basic stats of `Payload`, visual time series analysis, ACF for seasonality, sampling rate.
    *   **Goal 2: ML - Prediction (`lab_water_pressure.py` for `SenderId = 1`)**
        *   Fill in missing code (Pandas `.rolling().mean()`, `.shift()`).
        *   Evaluate and fine-tune Decision Tree Regressor (MSE, RMSE).
            *   **Features:** Cyclical time (sin/cos of min, hr, day, month), 20 lagged `Payload_MA`.
            *   **Target:** `Payload_MA`.
*   **Python Practical Exercises (Additional Goals - Deadline: May 19, 2025 - *Start of Physical Part*):**
    *   Repeat Goals 1 & 2 for `SensorId = 5`.
    *   Develop and compare a DNN model with the Decision Tree.
*   **Consultation:** Online sessions available before physical part.

## Physical Part Logistics
*   **Schedule:**
    *   9:00-13:30: Classroom activities (lectures, exercises, presentations).
    *   13:30-14:30: Lunch break.
    *   14:30-15:30: Q&A Session.
    *   Team presentations on Thursday and Friday.
*   **Travel to Novi Sad from Belgrade (BEG) Airport:**
    *   ~80km drive.
    *   **Train:** Cheapest (~€10 or less), arrives near town center.
    *   **Bus.**
    *   **Taxi:** €30-€60. **Book in advance** to avoid overcharging.
    *   **Airport to Belgrade Train Station:** Bus number 600 (to be confirmed).
*   **Food:**
    *   Lunch provided by UNS during the physical part (as per main program description).
    *   Welcome dinner provided.
    *   Coffee will be available.
    *   For other meals, cheap and edible food options (e.g., sandwiches for €2-€3) are available near the university.
*   **Leisure:** A beach is near the university.
