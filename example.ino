/*
 Based on the Adafruit i2c/SPI LCD backpack 
 which uses MCP23008 I2C expander
 ( http://www.ladyada.net/products/i2cspilcdbackpack/index.html )

 I2C is better than SPI for NodeMCU. But, it can be confusing getting
 the pins connected. The pins I have below are the only ones that work.

  The circuit:
 * 5V to Arduino 5V pin
 * GND to Arduino GND pin
 * CLK to Analog 0 digital
 * DAT to Analog 1 digital
*/

// include the library code:
#include <Wire.h>
#include <LiquidTWI.h>
// This lets you used the NodeMCU with Arduino IDE.
#include <ESP8266WiFi.h>

// Connect via i2c, default address #0 (A0-A2 not jumpered)
LiquidTWI lcd(0);

// put your local wifi info here
const char* ssid     = "YOUR-WIFI-SSID";
const char* password = "YOUR-WIFI-PASSWORD";

// Change this host if you are using this for something other than MTA busTime
const char* host = "bustime.mta.info";

void setup() {
  Serial.begin(115200);
  delay(10);
  
  // Even though we connect to pins 0, 1 this is what they are called: Weird, right?
  Wire.pins(4,5);
  
  // set up the LCD's number of rows and columns: 
  lcd.begin(20, 3);
  
  // Print title to the LCD.
  lcd.print("Data Grabber For:");
  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 1);
  lcd.print(host);

 // connecting to a WiFi network
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    //lcd.setCursor(0, 3);
    delay(300);
    Serial.print(". ");
  }

  //Show wifi connection worked and display our ip!
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected"); 
  lcd.setCursor(0, 1); 
  lcd.print("IP address: ");
  lcd.setCursor(0, 2);
  lcd.print(WiFi.localIP());
}

int value = 0;

void loop() {

  ++value;

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, 80)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("connection failed");
    delay(2000);
    return;
  }
  

  delay(2000);
  //
  // You will need your own API key from the MTA. You will also need the ID number of the stop you want to watch.
  // You can get the key and stop number here: http://bustime.mta.info/wiki/Developers/Index
  // The <BUS-ID> is just BX1 or whatever your bus is for short. without this parameter. Will display all busses.
  // DirectionRef=1 means we are getting downtown buses. Change to DirectionRef=0 to get uptown busses.
  // 
  client.println("GET /api/siri/stop-monitoring.xml?key=<YOUR-API-KEY>&OperatorRef=MTA&MonitoringRef=<BUS-STOP-ID>&DirectionRef=1&MaximumStopVisits=2&LineRef=MTA%20NYCT_<BUS-ID> HTTP/1.1");
  client.println("Host: bustime.mta.info");
  client.println("Connection: close\r\n");
  //Pause long enough for the connection to happen. Make this longer if you have issues.
  delay(800);

  while(client.available()){
  String line = client.readStringUntil('\n');
  delay(60);

   //pull and print the "expected arrival times" from XML
   //This is just a text search, not fancy.
   if ( client.findUntil("ExpectedArrivalTime>","") ) {
    client.findUntil("T","");
    String nextBus = client.readStringUntil('.');
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Next 2 Bx1 buses:");
    lcd.setCursor(0, 1);
    lcd.print(nextBus);

    client.findUntil("/ExpectedArrivalTime>","");
    client.findUntil("ExpectedArrivalTime>","");
    client.findUntil("T","");
    String nextBus2 = client.readStringUntil('.');
    lcd.setCursor(0, 2);
    lcd.print(nextBus2);
     delay(3000); }

    
  }

}
