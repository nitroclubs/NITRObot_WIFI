//
//          NITRObot - WiFi WEB server control v1
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

// Install WiFiEsp library from the IDE:
// https://github.com/bportaluri/WiFiEsp

#include "WiFiEsp.h"

#define LEFT_FWD 9    // PWMB
#define LEFT_BACK 5   // DIRB  ---  left
#define RIGHT_FWD 6   // PWMA
#define RIGHT_BACK 10 // DIRA  ---  right

//
char ssid[] = "YOUR_WIFI_NETWORK_SSID_GOES_HERE";            // your network SSID (name)
char pass[] = "YOUR_WIFI_NETWORK_PASSWORD_GOES_HERE";        // your network password
int status = WL_IDLE_STATUS;

int Speed = 255; // Motor speed
int ledStatus = LOW;

void moveForward();
void moveBackward();
void turnLeft();
void turnRight();
void stopMoving();

WiFiEspServer server(80);

// use a ring buffer to increase speed and reduce memory allocation
RingBuffer buf(8);

// FUNCTION PROTOTYPES
void sendHttpResponse(WiFiEspClient client);
void printWifiStatus();


void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);  // initialize digital pin LED_BUILTIN as an output.

 // motor pins setup
  pinMode(LEFT_FWD, OUTPUT);
  pinMode(LEFT_BACK, OUTPUT);
  pinMode(RIGHT_FWD, OUTPUT);
  pinMode(RIGHT_BACK, OUTPUT);
  // stop motors:
  analogWrite(LEFT_FWD, 0);
  analogWrite(LEFT_BACK, LOW);
  analogWrite(RIGHT_FWD, 0);
  analogWrite(RIGHT_BACK, LOW);

  Serial.begin(115200);   // initialize serial for debugging
  Serial2.begin(115200);    // initialize serial for ESP module
  WiFi.init(&Serial2);    // initialize ESP module

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  printWifiStatus();
  
  // start the web server on port 80
  server.begin();
}


void loop()
{
  WiFiEspClient client = server.available();  // listen for incoming clients

  if (client) {                               // if you get a client,
    Serial.println("New client");             // print a message out the serial port
    buf.init();                               // initialize the circular buffer
    while (client.connected()) {              // loop while the client's connected
      if (client.available()) {               // if there's bytes to read from the client,
        char c = client.read();               // read a byte, then
        buf.push(c);                          // push it to the ring buffer

        // printing the stream to the serial monitor will slow down
        // the receiving of data from the ESP filling the serial buffer
        //Serial.write(c);
        
        // you got two newline characters in a row
        // that's the end of the HTTP request, so send a response
        if (buf.endsWith("\r\n\r\n")) {
          sendHttpResponse(client);
          break;
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (buf.endsWith("GET /F")) {
          Serial.println("Drive Forward");
          moveForward();
        }
        else if (buf.endsWith("GET /B")) {
          Serial.println("Drive Backward");
          moveBackward();    // turn the LED off by making the voltage LOW
        }
        else if (buf.endsWith("GET /L")) {
          Serial.println("Turn Left");
          turnLeft();    // turn the LED off by making the voltage LOW
        }
        else if (buf.endsWith("GET /R")) {
          Serial.println("Turn Right");
          ledStatus = LOW;
          turnRight();    // turn the LED off by making the voltage LOW
        }
        else if (buf.endsWith("GET /S")) {
          Serial.println("STOP");
          stopMoving();    // turn the LED off by making the voltage LOW
        }
      }
    }
    
    // close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
}


void sendHttpResponse(WiFiEspClient client)
{
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  
  // the content of the HTTP response follows the header:
  client.print("NITRObot control WEB page - simple");
  client.println("<br>");
  client.println("<br>");
  
  client.println("Click to drive forward <a href=\"/F\">FORWARD</a> <br>");
  client.println("Click to drive backward <a href=\"/B\">BACKWARD</a><br>");
  client.println("Click  to turn left <a href=\"/L\">TURN LEFT</a><br>");
  client.println("Click  to turn Right <a href=\"/R\">TURN RIGHT</a><br>");
  client.println("Click  to stop the movment <a href=\"/S\">STOP</a><br>");
  
  // The HTTP response ends with another blank line:
  client.println();
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in the browser
  Serial.println();
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
  Serial.println();
}

void moveForward() // Move forward
{
  analogWrite(LEFT_FWD, Speed);
  analogWrite(LEFT_BACK, LOW);
  analogWrite(RIGHT_FWD, Speed);
  analogWrite(RIGHT_BACK, LOW);
}

void moveBackward() // Move backward
{
  analogWrite(LEFT_FWD, LOW);
  analogWrite(LEFT_BACK, Speed);
  analogWrite(RIGHT_FWD, LOW);
  analogWrite(RIGHT_BACK, Speed);
}

void turnLeft() // Turn Left
{
  analogWrite(LEFT_FWD, LOW);
  analogWrite(LEFT_BACK, Speed);
  analogWrite(RIGHT_FWD, Speed);
  analogWrite(RIGHT_BACK, LOW);
}

void turnRight() // Turn Right
{
  analogWrite(LEFT_FWD, Speed);
  analogWrite(LEFT_BACK, LOW);
  analogWrite(RIGHT_FWD, LOW);
  analogWrite(RIGHT_BACK, Speed);
}

void stopMoving() // Stop moving
{
  analogWrite(LEFT_FWD, 0);
  analogWrite(LEFT_BACK, LOW);
  analogWrite(RIGHT_FWD, 0);
  analogWrite(RIGHT_BACK, LOW);
}
