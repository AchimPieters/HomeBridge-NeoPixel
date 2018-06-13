/* Project name: Apple Homebridge - NeoPixel
   Project URI:  https://www.studiopieters.nl/homebridge-neopixel-light/ ‎
   Description: Apple Homebridge - NeoPixel Light
   Version: 6.0.3
   License: GNU General Public License V2 or later  */


#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>



////////////////////////////////////////////////////////////////////
// Which pin On which pin is the ESP8266-12E connected to the NeoPixels
#define PIN 13

// How many NeoPixels are attached to the ESP8266-12E
#define NUMPIXELS      144

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

///////////////////////////////////////////////////////////////////

WiFiServer server(80); //Set server port

String readString;           //String to hold incoming request
String hexString = "000000"; //Define inititial color here (hex value)

int state;

int r;
int g;
int b;

float R;
float G;
float B;

int x;
int V;



///// WiFi SETTINGS - Replace with your values /////////////////
const char* ssid = "YOUR_ROUTER_SSID";
const char* password = "YOUR_ROUTER_PASSWORD";
IPAddress ip(192, 168, 1, 10);   // set a fixed IP for the NodeMCU
IPAddress gateway(192, 168, 1, 1); // Your router IP
IPAddress subnet(255, 255, 255, 0); // Subnet mask
////////////////////////////////////////////////////////////////////

void WiFiStart() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print("_");
  }
  Serial.println();
  Serial.println("Done");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

  server.begin();
}

void allOff() {
  state = 0;
  for (int i = 0; i < NUMPIXELS; i++)
    strip.setPixelColor(i, 0, 0, 0);
  strip.show();
}

//Write requested hex-color to the pins
void setHex() {
  state = 1;
  long number = (long) strtol( &hexString[0], NULL, 16);
  r = number >> 16;
  g = number >> 8 & 0xFF;
  b = number & 0xFF;

  for (int i = 0; i < NUMPIXELS; i++)
    strip.setPixelColor (i, r, g, b);
  strip.show();
}

//Compute current brightness value
void getV() {
  R = roundf(r / 2.55);
  G = roundf(g / 2.55);
  B = roundf(b / 2.55);
  x = _max(R, G);
  V = _max(x, B);
  strip.setBrightness(V);
  strip.show();
}

//For serial debugging only
void showValues() {
  Serial.print("Status on/off: ");
  Serial.println(state);
  Serial.print("RGB color: ");
  Serial.print(r);
  Serial.print(".");
  Serial.print(g);
  Serial.print(".");
  Serial.println(b);
  Serial.print("Hex color: ");
  Serial.println(hexString);
  getV();
  Serial.print("Brightness: ");
  Serial.println(V);
  Serial.println("");
}

void setup() {
  Serial.begin(115200);
  setHex(); //Set initial color after booting. Value defined above
  WiFi.mode(WIFI_STA);
  WiFiStart();
  showValues(); //Uncomment for serial output

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  state = 0;
}

void loop() {
  //Reconnect on lost WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    WiFiStart();
  }

  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  while (client.connected() && !client.available()) {
    delay(1);
  }

  //Respond on certain Homebridge HTTP requests
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (readString.length() < 100) {
          readString += c;
        }
        if (c == '\n') {
          Serial.print("Request: "); //Uncomment for serial output
          Serial.println(readString); //Uncomment for serial output

          //Send reponse
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();

          //On
          if (readString.indexOf("on") > 0) {
            setHex();
            showValues();
          }

          //Off
          if (readString.indexOf("off") > 0) {
            allOff();
            showValues();
          }

          //Set color
          if (readString.indexOf("set") > 0) {
            hexString = "";
            hexString = (readString.substring(9, 15));
            setHex();
            showValues();
          }

          //Status on/off
          if (readString.indexOf("status") > 0) {
            client.println(state);
          }

          //Status color (hex)
          if (readString.indexOf("color") > 0) {
            client.println(hexString);
          }

          //Status brightness (%)
          if (readString.indexOf("bright") > 0) {
            getV();
            client.println(V);
          }

          delay(1);
          client.stop();
          readString = "";
        }
      }
    }
  }
}
