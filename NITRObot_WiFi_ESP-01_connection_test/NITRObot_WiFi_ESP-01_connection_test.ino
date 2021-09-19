// 
//            NITRObot ESP-01 connection test - v1
// 
// Modified from EXAMPLES>COMMUNICATION>SerialPassthrough sketch
//
//   This scetch allows you to communicate with ESP-01/ESP-01S (ESP8266) using AT commands via the Serial monitor
//   The module shoul be connected to Serial2 port - pins 16 & 17 of the Arduino Mega2560, 
//
// ESP-01 (ESP-01S) board uses the ESP8266 chip.
// ESP-01 (ESP-01S) is a 3.3V device and the I/O pins are not 5V tolerant!
// For simplicity, use ESP-01 adapter board to power the ESP-01 from the Arduino board and to be able communicate with the 5V I/O of the Arduino Mega.
// Alternatively, it is possible to connect the ESP-01 board to Arduino Mega without an adapter board,
// but you need to use a logic level shifter or a voltage divider on the Rx pin of the ESP-01 
// (two resistors - 1K to Mega Tx and 2K to Ground, the middle goes to the ESP-01 Rx pin).
//
// Arduino Mega2560 has a limit of 50mA on the 3.3v pin, but ESP-01 (ESP8266) can pull up to 300mA.
// You should not power the ESP-01 from the Arduino Mega 3.3V pin, as the 3.3V Arduino Mega can not supply enough current!
// You can power the ESP-01 from the Arduino Mega 5V pin, by using a 3.3V voltage regulator or use external 3.3V power supply.
//
// Serial port 1 is used for the Bluwtooth module on NITRObot!
// Connect ESP-01 adapter board to Arduino Mega Serial port 2: 
// Arduino Mega --------------------- ESP-01 (ESP-01S) adapter board
//          5V  --------------------- VCC
//          GND --------------------- GND
// Pin 16 - TX  --------------------- RX
// Pin 17 - RX  --------------------- TX

// AT COMMANDS to use for the test:
// After each command press Enter or click the Send button:
// 
// AT              ESP-01 should respond with OK
// If you do not get the OK reply - check your connections!
// AT+GMR          ESP-01 should respond with version, SDK and compile time:        
//                        AT version:1.1.0.0(May 11 2016 18:09:56)
//                        SDK version:1.5.4(baaeaebb)
//                        compile time:May 20 2016 15:08:19)
//                        OK
// AT+CWMODE?      ESP-01 should respond with the current mode:
//                        +CWMODE:2
//                        OK
// By default the ESP-01(ESP8266) should be in mode 2 Access Point (AP) 
// This means that you can connect to the WiFi Access point the ESP-01 created with your PC
//
// You can see a full list of the available AT commands at:
// https://www.espressif.com/sites/default/files/documentation/4a-esp8266_at_instruction_set_en.pdf 

void setup() {
  Serial.begin(115200);   // Initialize the serial port for the Serial monitor (USB)
  Serial2.begin(115200);  // Initialize the serial port for communication with the WiFi modeule ESP-01
}

void loop() {
  if (Serial.available()) {         // If anything comes in Serial (USB),
    Serial2.write(Serial.read());   // read it and send it out Serial2 (pins 16 & 17)
  }

  if (Serial2.available()) {        // If anything comes in Serial2 (pins 16 & 17)
    Serial.write(Serial2.read());   // read it and send it out Serial (USB)
  }
}
