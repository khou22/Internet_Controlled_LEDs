#include <SPI.h>
#include <Ethernet.h>

#define MaxHeaderLength 32    //maximum length of http header required
String HttpHeader = String(MaxHeaderLength); 

String tasks[] = {"Kevin", "Hou"}; // Names of tasks - corresponds with LEDs
int leds[] = {0, 1}; // Led pins
boolean ledStates[] = {true, false}; // On or off

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Physical mac address
byte ip[] = { 192, 168, 2, 201 }; // Static IP of Arduino
byte gateway[] = { 192, 168, 2, 254 }; // Gateway address
byte subnet[] = { 255, 255, 255, 0 }; // Subnet mask
EthernetServer server(80); // Web server port

String tempURL = ""; // Temporary storage
String URL = ""; // Actual URL with data


void setup(){
  // Enable serial monitor
  Serial.begin(9600);

  // Initiate LEDs
  inititateLEDs();
  
  // Start Ethernet
  Ethernet.begin(mac, ip, gateway, subnet);
  if (Ethernet.begin(mac)) {
    Serial.println(F("Connected."));
    Serial.print(F("Point your Browser at: http://"));
    Serial.println(Ethernet.localIP());
  } else {
    Serial.println(F("Could not connect to network"));
  }
   // Initialize variable
  HttpHeader="";
}

void loop(){
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
         char c = client.read();
         //read MaxHeaderLength number of characters in the HTTP header
         //discard the rest until \n
         if (HttpHeader.length() < MaxHeaderLength)
         {
           //store characters to string
           HttpHeader = HttpHeader + c; 
         }
         //if HTTP request has ended
         if (c == '\n') { 
           // show the string on the monitor - parse URL here
           tempURL = HttpHeader;
//           Serial.println(tempURL); // Feedback
           if (tempURL.indexOf('?') != -1) {
             Serial.println("Actual URL:");
             URL = tempURL;
             Serial.println(URL);
             parseURL();
           }
           
          // start of web page
           client.println("HTTP/1.1 200 OK");
           client.println("Content-Type: text/html");
           client.println("<html><head></head><body>");
           client.println();
           client.print("<form method=get>");
           client.print("<input type='checkbox' name=r value='led0'> Item One<br>");
           client.print("<input type='checkbox' name=r value='led1' checked> Item Two<br>");
           client.print("<input type='checkbox' name=r value='led2'> Item Three<br>");
            client.print("<input type=submit value=submit></form>");
           client.print("</body></html>");
           //clearing string for next read
           HttpHeader="";
           // Stopping client
           client.stop();
         }
       }
     }
   }
}

void parseURL() {
  for (int i = 0; i < sizeof(leds); i++) {
    String query = "r=" + leds[i]; // Search query
    Serial.println("Looking for " + query); // Feedback
    int result = URL.indexOf(query);
    if (result != -1) { // If contains the LED number in the URL
      Serial.println("LED: " + String(i) + " on"); // Feedback
      ledStates[i] = true; // Turn on
    } else {
      ledStates[i] = false; // Turn off
    }
  }
  setLEDs(); // Set the LEDs "on" or "off"
}

void setLEDs() {
  for (int i = 0; i < sizeof(ledStates); i++) {
    if (ledStates[i]) { // If LED is turned on
      digitalWrite(leds[i], HIGH); // Turn on the LED
    } else {
      digitalWrite(leds[i], LOW); // Turn off the LED
    }
  }
}

void inititateLEDs() {
  for (int i = 0; i < sizeof(leds); i++) {
    pinMode(leds[i], OUTPUT); // Initiate LED
    digitalWrite(leds[i], LOW); // Turn all of them off first
  }
}

