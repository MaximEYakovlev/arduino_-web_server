/*
 Web Server

 A simple web server that shows the value of the analog input pins
 using an Arduino WIZnet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 */

#include <SPI.h>
#include <Ethernet.h>

// Constants
const int ANALOG_PINS = 6;  // Number of analog pins

// Ethernet configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server on port 80
EthernetServer server(80);

// Function to initialize Ethernet and start the server
void setupEthernet() {
  // Initialize Ethernet with CS pin 10
  Ethernet.init(10);

  // Start serial communication for debugging
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect (only needed for some boards)
  }
  Serial.println("Starting Ethernet WebServer...");

  // Start Ethernet with static IP
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield not found. Stopping execution.");
    while (true) {
      delay(1); // Infinite loop if Ethernet is not present
    }
  }

  // Check if Ethernet cable is connected
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // Start the server
  server.begin();
  Serial.print("Server is running at IP: ");
  Serial.println(Ethernet.localIP());
}

// Function to send HTTP response to the client
void sendHttpResponse(EthernetClient &client) {
  // Send HTTP response headers
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");  // Close connection after response
  client.println("Refresh: 5");         // Refresh the page every 5 seconds
  client.println();

  // Send HTML content
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  for (int analogChannel = 0; analogChannel < ANALOG_PINS; analogChannel++) {
    int sensorReading = analogRead(analogChannel);
    client.print("Analog input ");
    client.print(analogChannel);
    client.print(" is ");
    client.print(sensorReading);
    client.println("<br />");
  }
  client.println("</html>");
}

// Function to handle client connections
void handleClient(EthernetClient &client) {
  Serial.println("New client connected");

  bool currentLineIsBlank = true;

  // Read the client's request
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      Serial.write(c);

      // If end of HTTP request (blank line), send response
      if (c == '\n' && currentLineIsBlank) {
        sendHttpResponse(client);
        break;
      }

      // Check if the current line is blank
      if (c == '\n') {
        // Start of a new line
        currentLineIsBlank = true;
      } else if (c != '\r') {
        // Current line has content
        currentLineIsBlank = false;
      }
    }
  }

  // Short delay to ensure data is sent
  delay(1);

  // Close the connection
  client.stop();
  Serial.println("Client disconnected");
}

void setup() {
  setupEthernet();
}

void loop() {
  // Check for incoming clients
  EthernetClient client = server.available();

  if (client) {
    handleClient(client);
  }
}
