

#include "Adafruit_NeoKey_1x4.h"
#include "seesaw_neopixel.h"
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;

Adafruit_NeoKey_1x4 neokey; 

//int outPins2[] = {5,6,9,10,11,12,13};
//int outPins1[] = {18,17,16,15,14,8,36};
int outPins1[] = {0,1,2,3,4,5,6};
int outPins2[] = {7,8,9,10,11,12,13};
/*
static const uint8_t digitCodeMap[] = {
  // GFEDCBA  Segments      7-segment map:
  0b00111111, // 0   "0"          AAAA
  0b00000110, // 1   "1"         F    B
  0b01011011, // 2   "2"         F    B
  0b01001111, // 3   "3"          GGGG
  0b01100110, // 4   "4"         E    C
  0b01101101, // 5   "5"         E    C
  0b01111101, // 6   "6"          DDDD
  0b00000111, // 7   "7"
  0b01111111, // 8   "8"
  0b01101111, // 9   "9"
};
*/
static const uint8_t digitCodeMap[] = {
  // GFEDCBA  Segments 
  // 7654321     7-segment map:
  0b01111011, // 0   "0"          2222
  0b01000001, // 1   "1"         4    1
  0b00110111, // 2   "2"         4    1
  0b01100111, // 3   "3"          3333
  0b01001101, // 4   "4"         5    7
  0b01101110, // 5   "5"         5    7
  0b01111110, // 6   "6"          6666
  0b01000011, // 7   "7"
  0b01111111, // 8   "8"
  0b01101111, // 9   "9"
};

volatile int currNum1 = 0;
volatile int currNum2 = 0;

bool THmode = true;

void writeDigits(int n, int k){
  //uint8_t digit = digitCodeMap[n];
  for(int i=0; i < 7; i++){
    digitalWrite(outPins1[i], !bitRead( digitCodeMap[n], i));
    //Serial.print( bitRead( digitCodeMap[n], i), BIN);
  }

  for(int i=0; i < 7; i++){
    digitalWrite(outPins2[i], !bitRead( digitCodeMap[k], i));
    //Serial.print( bitRead( digitCodeMap[k], i), BIN);
  }
}

NeoKey1x4Callback blink(keyEvent evt) {
  uint8_t key = evt.bit.NUM;

  
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    if(key == 0){
      THmode = !THmode;
    }
    /*
    if(key == 0){
      if(currNum > 0){
        currNum = currNum - 1;
      }else if(currNum == 0){
        currNum = 9;
      }
     
    }else if(key == 1){
      if(currNum < 9){
        currNum = currNum + 1;
      }else if(currNum == 9){
        currNum = 0;
      }

    }
    */
    //writeDigit(currNum);
    neokey.pixels.setPixelColor(key, Wheel(map(key, 0, neokey.pixels.numPixels(), 0, 255)));
    
  } else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {


    neokey.pixels.setPixelColor(key, 0);
  }

  // Turn on/off the neopixels!
  neokey.pixels.show();
  return 0;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  for(int i=0; i < 7; i++){
    pinMode(outPins1[i], OUTPUT);
  }
  for(int i=0; i < 7; i++){
    pinMode(outPins2[i], OUTPUT);
  }

  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }

  //if (! neokey.begin(0x30)) {     // begin with I2C address, default is 0x30
  //  Serial.println("Could not start NeoKey, check wiring?");
  //  while(1) delay(10);
  //}
  
  neokey.begin(0x30);
  for(int i=0; i<NEOKEY_1X4_KEYS; i++){
    neokey.registerCallback(i, blink);
  }
  writeDigits(currNum1,currNum2);
}
float update = 0;
int tempf = 0;
int humidityf = 0;



void loop() {
  neokey.read();
  /*
  if(currNum2 < 9){
    currNum2 = currNum2 + 1;
  }else if(currNum2 == 9){
    currNum1 = currNum1 + 1;
    currNum2 = 0;
  }
  if(currNum1 > 9){
    currNum1 == 0;   
  }
  writeDigits(currNum1, currNum2);
  delay(1000);
  */
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
  tempf = temp.temperature * 1.8 + 32;
  humidityf = humidity.relative_humidity;
  
  if(THmode){
    writeDigits(tempf / 10, tempf % 10);
  }else{
    writeDigits(humidityf / 10, humidityf % 10);
  }
 delay(25);
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return seesaw_NeoPixel::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return seesaw_NeoPixel::Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return seesaw_NeoPixel::Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  return 0;
}