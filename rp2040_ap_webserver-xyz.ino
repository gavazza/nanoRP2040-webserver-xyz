#include <SPI.h>
#include <WiFiNINA.h>
#include <Arduino_LSM6DSOX.h>

// Network SSID and password
char ssid[] = "rp2040";
char pass[] = "12345678";

int status = WL_IDLE_STATUS;
WiFiServer server(80);

float accX = 0.0, accY = 0.0, accZ = 0.0;
float gyrX = 0.0, gyrY = 0.0, gyrZ = 0.0;
float temp;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  IMU.begin();

  delay(500);
  
  Serial.println("Access Point Web Server");

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();
}

void loop() {
  IMU.readAcceleration(accX, accY, accZ);
  IMU.readGyroscope(gyrX, gyrY, gyrZ);
  IMU.readTemperatureFloat(temp);
      
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      Serial.println("Device connected to AP");
    }
    else {
      Serial.println("Device disconnected from AP");
    }
  }

  // listen for incoming clients
  WiFiClient client = server.available();

  if (client) {
    // if you get a client,
    Serial.println("new client");
    String currentLine = "";
    
    while (client.connected()) {
      
      
      // This is required for the Arduino Nano RP2040 Connect - otherwise it will loop so fast that SPI will never be served.
      delayMicroseconds(10);
                    
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        
        if (c == '\n') {
          // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // Create the webserver page 
            webServerPage(client);
            
            // break out of the while loop:
            break;
          }
          else {
            // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {
          // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }

  delay(500);
}

/// @brief Print the SSID, the IP Address and webserver URL
void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

/// @brief Create the HTML + CSS + Javascript page for the webserver
void webServerPage(WiFiClient client) {
  client.print("<html>");
  client.print("  <head>");
  client.print("    <style>");
  client.print("      label,h1 {");
  client.print("        font-family:verdana,garamond,serif;");
  client.print("        color: black;");
  client.print("        font-size: 38px;");
  client.print("        text-align: center;");
  client.print("        font-weight:normal;");
  client.print("      }");
  client.print("       .left {");
  client.print("         display: inline-block;");
  client.print("         text-align: left;");
  client.print("       }");
  client.print("    </style>");
  client.print("  </head>");
  client.print("  <body>");
  client.print("    <p><h1>Arduino Nano RP2040<h1></p>");
  client.print("    <fieldset style=\"width: 600px; margin: 0px auto\">");
  client.print("      <legend>Accelerometer</legend>");
  client.print("      <div class=\"left\">");
  client.print("        <label>X:</label>");
  client.print("        <label>");
  client.print(accX);
  client.print("        </label>");
  client.print("      </div>");
  client.print("      <div class=\"left\">");
  client.print("        <label>Y:</label>");
  client.print("        <label>");
  client.print(accY);
  client.print("        </label>");
  client.print("      </div>");
  client.print("      <div class=\"left\">");
  client.print("        <label>Z:</label>");
  client.print("        <label>");
  client.print(accZ);
  client.print("        </label>");
  client.print("      </div>");
  client.print("    </fieldset>");
  client.print("    <br>");
  client.print("    <fieldset style=\"width: 600px; margin: 0px auto\">");
  client.print("      <legend>Gyroscope</legend>");
  client.print("      <div class=\"left\">");
  client.print("        <label>X:</label>");
  client.print("        <label>");
  client.print(gyrX);
  client.print("        </label>");
  client.print("      </div>");
  client.print("      <div class=\"left\">");
  client.print("        <label>Y:</label>");
  client.print("        <label>");
  client.print(gyrY);
  client.print("        </label>");
  client.print("      </div>");
  client.print("      <div class=\"left\">");
  client.print("        <label>Z:</label>");
  client.print("        <label>");
  client.print(gyrZ);
  client.print("        </label>");
  client.print("      </div>");
  client.print("    </fieldset>");
  client.print("    <br>");
  client.print("    <fieldset style=\"width: 600px; margin: 0px auto\">");
  client.print("      <legend>Temperature</legend>");
  client.print("      <div class=\"left\">");
  client.print("        <label>");
  client.print(temp);
  client.print("        </label>");
  client.print("        <label>C</label>");
  client.print("      </div>");
  client.print("    </fieldset>");
  client.print("    <script>");
  client.print("      setInterval(updateData, 1000);");
  client.print("      function updateData() {");
  client.print("        location.reload();");
  client.print("      }");
  client.print("    </script>");
  client.print("  </body>");
  client.print("</html>");

  // HTTP response ends with another blank line:
  client.println();
}
