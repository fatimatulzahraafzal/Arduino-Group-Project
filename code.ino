#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#include <DHT.h>

//initializations for thingspeak
#include "WiFiEsp.h"
#include "ThingSpeak.h"
#include "ArduinoJson.h"
WiFiEspClient client;
const char* myWriteAPIKey = "3B7GVE22ADGN41Z1";
//list of possible logging messages
int water_on = 1;
int led_on = 2;
int led_off = 3;
int fan_on = 4;
int fan_off = 5;
int water_off = 6;
//list of control field (field numbers)
int controlfield_pump = 1;
int controlfield_LED = 2;
int controlfield_fan = 3;
#define ESP_BAUDRATE 115200
//char ssid[] = "EE3070_P1615_1";  // your network SSID (name)
//char pass[] = "EE3070P1615";     // your network password
char ssid[] = "Kajtek";   // your network SSID (name)
char pass[] = "64320942";     // your network password
int status = WL_IDLE_STATUS;  // the Wifi radio's status

//pin initializations: digital pins used-> 2,3,4,5,6,7,8
int trigPin = 3; 
int echoPin = 4; 
const byte relayPin = 2;
const byte fan1 = 5;
const byte fan2 = 6;
#define DHTPIN 7     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
//DHT dht(DHTPIN, DHTTYPE);
DHT dht;
float hum;  

//variable initializations
int maxDistance=5; 
int minDistance=5; 
int UpdateDataCounter=60; //data is uploaded every 1 minute
int PumpOnTime=5; //water pump is on for 1 minute
int PumpOffTime=5;//water pump is off for 20 minutes
bool PumpOnFlag=true;
bool FanOnFlag= false;


void setup() {

  pinMode(relayPin, OUTPUT); // set our relay pinmode to output 
  Serial.begin(9600);
  //Serial.begin(115200);
  pinMode(trigPin,OUTPUT); 
  pinMode(echoPin,INPUT); 
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(8, INPUT);
  Serial1.begin(ESP_BAUDRATE);
  dht.setup(DHTPIN);
  // initialize ESP module
  WiFi.init(&Serial1);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected");
  }
  ThingSpeak.begin(client);

  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

}

int ultrasonic() {
  long duration, distance; 
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 
  duration = pulseIn(echoPin, HIGH); 
  distance = (duration/2) / 29.1; 
  return distance;
}

void oledDisplay(int distance, int temp, int hum){
  //code for OLED
  display.clearDisplay();
  display.setTextSize(1.5); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(18, 2);
  display.print("Water Level");
  display.setTextSize(1.5);  
  display.setCursor(18, 16);
  display.print(distance);
  display.setCursor(38, 16);
  display.print("cm");  

  display.setCursor(18, 30);
  display.print("Temperature");
  display.setTextSize(1.5);  
  display.setCursor(18, 44);
  display.print(temp);
  display.display();      // Show initial text
  //delay(100); 
  
}

void waterPump(int distance){

  if (distance>7){  //water level too low. interrupt case.

    Serial.println("interrupt case");
    if(PumpOnFlag==true) {
      digitalWrite(relayPin, LOW); // Pump OFF 
      PumpOnTime=5; //reset the pump on time
      PumpOnFlag=false;
      writeDatalog(water_off);
    }
  }

  else { //regular routine case.
    if(PumpOnFlag==false) { //the pump is OFF
      if(PumpOffTime==0) { //the pump has completed one complete cycle of OFF time i.e. 20 min
        digitalWrite(relayPin, HIGH); //switch the pump ON
        PumpOffTime=5;//reset the pump OFF counter to 1200
        PumpOnFlag=true; 
        writeDatalog(water_on);
      } 
      else{ //the pump has not completed the cycle for OFF
        PumpOffTime=PumpOffTime-1; //decrement the counter for the OFF time
      }
    }
    else{ //the pump is ON
      if(PumpOnTime==0) { //the pump has completed one pumping cycle i.e 1 minute
        digitalWrite(relayPin, LOW); //switch the pump OFF
        PumpOnTime=5;//reset the pump ON counter to 60
        PumpOnFlag=false;
        writeDatalog(water_off);
        // Serial.print("hdus"); 
      } 
      else{ //the pump has not completed the cycle for OFF
        PumpOnTime=PumpOnTime-1; //decrement the counter for the OFF time
        // Serial.print("hduied"); 
      }
    }

  }
  
  //if water level is very low, put up an alert and stop pumping
  //otherwise keep pumping on regular intervals uncondtionally
  
    
}

void lightControls1(int lightlevels){
  if (lightlevels<300){
    //switch on led1
    writeDatalog(led_on);
  }
}

void lightControls2(int lightlevels){
  if (lightlevels<300){
    //switch on led2
    writeDatalog(led_on);
  }
}

void fancontrol(float hum, int gas){

  digitalWrite(fan2, HIGH); 
  if (FanOnFlag==false){
    if (hum<123 || gas == 0){ //there is some problem with either hum or gas
      digitalWrite(fan1, HIGH);
      digitalWrite(fan2, HIGH);    
      writeDatalog(fan_on); 
      FanOnFlag=true;
    }
  }
  else{
    if (hum>123 && gas == 1){ //there is no problem with either hum AND gas anymore
      digitalWrite(fan1, LOW);
      digitalWrite(fan2, LOW);    
      writeDatalog(fan_off); 
      FanOnFlag=false;
    }
  }
}

float readhumidity(void) {

  //int readData = DHT.read11(DHTPIN);
  float humidity = dht.getHumidity();
  return humidity;

}

int readtemperature(void) {

  //int readData = DHT.read11(DHTPIN);
  int temp = dht.getTemperature();
  return temp;

}

int tvoc(void) {
  int digitalValue;
  digitalValue = digitalRead(8);
  return digitalValue;

}

void writeDataToThingSpeak(int temp, int humidity, int volatil, int lightlevels1, int lightlevels2) {

  ThingSpeak.setField(1, temp);  //setting which field would accept what value (field, value)
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(5, volatil);
  ThingSpeak.setField(3, lightlevels1);
  ThingSpeak.setField(4, lightlevels2);

  int stat = 0;  //HTML error code
  stat = ThingSpeak.writeFields(2027890, myWriteAPIKey);  //wrt line 34 setField command, this writes the data
  if (stat == 200) {
    Serial.println("Channel update sccessful.");
  } else {
    Serial.println("Problem in Channel. HTTP error code " + String(stat));
    return;
  }

  return;
}

void writeDatalog(int log) {  //logging 

  int stat = 0;

  stat = ThingSpeak.writeField(2069971, 1, log, "FWQSFJIZ8YFHSORS");  //write one field
  if (stat == 200) {
    Serial.println("Channel update sccessful.");
  } else {
    Serial.println("Problem in Channel. HTTP error code " + String(stat));
    return;
  }
}

void readLed_thingspeak(int field){
  int bit = readControl_thingspeak(2);
  if (bit = 1){
    //turn on LEDs
  }else if(bit = 0){
    //turn off LEDs
  }
  int stat = 0;
  stat = ThingSpeak.writeField(2075747, 2, 999, "ARY1CPEUH52X7HRK");
}

void readPump_thingspeak(int field){
  int bit = readControl_thingspeak(1);
  if (bit = 1){
    digitalWrite(relayPin, HIGH);
  }else if(bit = 0){
    digitalWrite(relayPin, LOW);
  }
  int stat = 0;
  stat = ThingSpeak.writeField(2075747, 1, 999, "ARY1CPEUH52X7HRK");
}

void readFan_thingspeak(int field){
  int bit = readControl_thingspeak(3);
  if (bit = 1){
    digitalWrite(fan1, HIGH);
  }else if(bit = 0){
    digitalWrite(fan1, LOW);
  }
  int stat = 0;
  stat = ThingSpeak.writeField(2075747, 3, 999, "ARY1CPEUH52X7HRK");
}

//called by readPump, readLED or readFan
int readControl_thingspeak (int field) { 
  int val = "";
  val = ThingSpeak.readIntField(2075747, field, "WUT0OHT757G54N02");
  if (val == val){
    return val;
  }else{
    return 999;
  }
}


//delay in a single loop:
//best case = 60 seconds because of read time
//worst case = data log (20) + read (60) = 80 seconds
void loop() {

  int distance=ultrasonic();
  waterPump(distance);

  int gas = tvoc();
  Serial.println(gas);
  float hum = readhumidity();
  digitalWrite(fan1, HIGH);
  digitalWrite(fan2, HIGH); 
  //fancontrol(hum,gas);

  int lightlevels1=analogRead(A0);
  int lightlevels2=analogRead(A1);
  lightControls1(lightlevels1);
  lightControls2(lightlevels2);

  int temp = readtemperature();
  oledDisplay(distance, temp, hum); 
  writeDataToThingSpeak(temp, hum, gas, lightlevels1, lightlevels2);
  delay(20000);

  readFan_thingspeak(3);
  delay(20000);
  readLed_thingspeak(2);
  delay(20000);
  readPump_thingspeak(1);
  delay(20000);

}

