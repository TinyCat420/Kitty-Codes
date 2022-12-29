// IV-18 Test
//
// MAX6921 datasheet:
// https://datasheets.maximintegrated.com/en/ds/MAX6921-MAX6931.pdf
// Send bits from D19 first to D0 last
//
// Timer calculator:
// http://www.8bit-era.cz/arduino-timer-interrupts-calculator.html
//

#include "SPI.h"
#include "Adafruit_NeoKey_1x4.h"
#include "seesaw_neopixel.h"

Adafruit_NeoKey_1x4 neokey; 

// Arduino connections to driver board
#define DISP_CLK SCLK   // White - SPI SCK 
#define DISP_DATA MOSI  // Yellow - SPI MOSI
#define DISP_LOAD 5   // Blue
#define DISP_BLANK 6 // Green

#define DISP_BRIGHTNESS 10 // 0-10

// The number of grid positions
#define DISP_WIDTH 10

// Grid positions              IV18 / MAX6921
#define GRID_8     0x800000 // OUT0 / D1 (right side of tube, last full segment)
#define GRID_7     0x400000 // OUT1 / D2
#define GRID_6     0x200000 // OUT2 / D3
#define GRID_5     0x100000 // OUT3 / D4
#define GRID_4     0x080000 // OUT4 / D5
#define GRID_3     0x040000 // OUT5 / D6
#define GRID_2     0x020000 // OUT6 / D7
#define GRID_1     0x010000 // OUT7 / D8 (left side of tube, first full segment)
#define GRID_0     0x008000 // OUT8 / D9 (dot / dash - very left of tube)

// Segments
#define SEG_A      0x004000 // OUT 9
#define SEG_B      0x002000 // OUT 10
#define SEG_C      0x001000 // OUT 11
#define SEG_D      0x000800 // OUT 12
#define SEG_E      0x000400 // OUT 13
#define SEG_F      0x000200 // OUT 14
#define SEG_G      0x000100 // OUT 15
#define SEG_MINUS  0x000100 // OUT 15
#define SEG_DEGREE 0x000080 // OUT 16
#define SEG_DOT    0x000080 // OUT 16

// Digits and letters
#define DIGIT_0   SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F
#define DIGIT_1   SEG_B | SEG_C
#define DIGIT_2   SEG_A | SEG_B | SEG_D | SEG_E | SEG_G
#define DIGIT_3   SEG_A | SEG_B | SEG_C | SEG_D | SEG_G
#define DIGIT_4   SEG_B | SEG_C | SEG_F | SEG_G
#define DIGIT_5   SEG_A | SEG_C | SEG_D | SEG_F | SEG_G
#define DIGIT_6   SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G
#define DIGIT_7   SEG_A | SEG_B | SEG_C
#define DIGIT_8   SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G
#define DIGIT_9   SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G

#define lowSq SEG_D | SEG_E | SEG_G | SEG_C
#define highSq SEG_A | SEG_B | SEG_F | SEG_G

#define CHAR_A   SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G
#define CHAR_B   SEG_C | SEG_D | SEG_E | SEG_F | SEG_G
#define CHAR_C   SEG_A | SEG_D | SEG_E | SEG_F
#define CHAR_D   SEG_B | SEG_C | SEG_D | SEG_E | SEG_G
#define CHAR_E   SEG_A | SEG_D | SEG_E | SEG_F | SEG_G
#define CHAR_F   SEG_A | SEG_E | SEG_F | SEG_G
#define CHAR_G   SEG_A | SEG_C | SEG_D | SEG_E | SEG_F
#define CHAR_H   SEG_B | SEG_C | SEG_E | SEG_F | SEG_G
#define CHAR_I   SEG_E | SEG_F
#define CHAR_J   SEG_B | SEG_C | SEG_D | SEG_E
#define CHAR_K   SEG_A | SEG_C | SEG_E | SEG_F | SEG_G
#define CHAR_L   SEG_D | SEG_E | SEG_F
#define CHAR_M   SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G
#define CHAR_N   SEG_A | SEG_B | SEG_C | SEG_E | SEG_F
#define CHAR_O   SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F
#define CHAR_P   SEG_A | SEG_B | SEG_E | SEG_F | SEG_G
#define CHAR_Q   SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G
#define CHAR_R   SEG_E | SEG_G
#define CHAR_S   SEG_A | SEG_C | SEG_D | SEG_F | SEG_G
#define CHAR_T   SEG_D | SEG_E | SEG_F | SEG_G
#define CHAR_U   SEG_B | SEG_C | SEG_D | SEG_E | SEG_F
#define CHAR_V   SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G
#define CHAR_W   SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G
#define CHAR_X   SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G
#define CHAR_Y   SEG_B | SEG_C | SEG_D | SEG_F | SEG_G
#define CHAR_Z   SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G

long grids[] = {GRID_0, GRID_1, GRID_2, GRID_3, GRID_4, GRID_5, GRID_6, GRID_7, GRID_8};
long letters[] = {CHAR_A, CHAR_B, CHAR_C, CHAR_D, CHAR_E, CHAR_F, CHAR_G, CHAR_H, CHAR_I, CHAR_J, CHAR_K, CHAR_L, CHAR_M, CHAR_N, CHAR_O, CHAR_P, CHAR_Q, CHAR_R, CHAR_S, CHAR_T, CHAR_U, CHAR_V, CHAR_W, CHAR_X, CHAR_Y, CHAR_Z};
long digits[] = {DIGIT_0, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4, DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8, DIGIT_9};

// Digits and text to display
static char display_contents[DISP_WIDTH];

// Index for the grid position
static volatile uint8_t display_idx;

// The characters to display
String message;

volatile int interruptCounter;
int totalInterruptCounter;
 
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile int x = 1;
volatile int y = 0;
 
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  long j;
/*
  if (display_idx == 0 && (display_contents[display_idx] == '-' || display_contents[display_idx] == '*')) {
    j = grids[display_idx] | char_to_code(display_contents[display_idx]);
  } else {
    j = grids[display_idx] | char_to_code(display_contents[display_idx]);
  }
*/
  if(display_idx == x){
    if(y == 0){
      j = grids[display_idx] | lowSq;
    }else if(y == 1){
      j = grids[display_idx] | highSq;
    }
    
  }

  display_write(j);

  display_idx++;
  if (display_idx >= DISP_WIDTH-1)
    display_idx = 0;
  portEXIT_CRITICAL_ISR(&timerMux);
 
}

// DEBUGGING
void print_binary(uint32_t number, byte Length) {
  static int Bits;
  if (number) { //The remaining bits have a value greater than zero continue
    Bits++; // Count the number of bits so we know how many leading zeros to print first
    print_binary(number >> 1, Length); // Remove a bit and do a recursive call this function.
    if (Bits) for (byte x = (Length - Bits); x; x--)Serial.write('0'); // Add the leading zeros
    Bits = 0; // clear no need for this any more
    Serial.write((number & 1) ? '1' : '0'); // print the bits in reverse order as we depart the recursive function
  }
}


// Convert ASCII to hex value
long char_to_code(char c) {
  if ((c <= 0x5A) && (c >= 0x41)) { // Uppercase letters
    return letters[c - 0x41];
  } else if ((c <= 0x7A) && (c >= 0x61)) { // Lowercase letters
    return letters[c - 0x61];
  } else if ((c <= 0x39) && (c >= 0x30)) { // Digits
    return digits[c - 0x30];
  } else if (c == '.') {
    return SEG_DOT;
  } else if (c == '*') {
    return SEG_DEGREE;
  } else if (c == '-') {
    return SEG_MINUS;
  } else if (c == ' ') {
    return 0x0;
  }

  return 0x0;
}


//
void display_init() {
  Serial.println("Display Init");

  pinMode(DISP_LOAD, OUTPUT);
  digitalWrite(DISP_LOAD, HIGH);

  pinMode(DISP_BLANK, OUTPUT);
  display_blank();
}


// Turns display off (Force MAX6921 outputs OUT0 to OUT19 low)
void display_blank() {
  digitalWrite(DISP_BLANK, HIGH);
}


// Turns display back on at the defined brightness
void display_unblank() {
  int brightness = map(DISP_BRIGHTNESS, 0, 10, 255, 0);
  analogWrite(DISP_BLANK, brightness);
}


// Assigns the message into the dsiplay contents array
void display_message(const char* message) {
  strncpy(display_contents, message, DISP_WIDTH);
}


// Sends data to the MAX6821 
void display_write(uint32_t data) {
  SPI.transfer(data & 0xff);
  SPI.transfer((data>> 8) & 0xff);
  SPI.transfer((data>> 16) & 0xff);
}




/*
 * Sets up the timer
 * @link http://www.8bit-era.cz/arduino-timer-interrupts-calculator.html
 */
void display_timer_init() {
  Serial.println("Display Timer Init");
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000, true);
  timerAlarmEnable(timer);
 
  digitalWrite(DISP_LOAD, HIGH);
  display_unblank();
  
}  



//define a callback for key presses
NeoKey1x4Callback blink(keyEvent evt) {
  uint8_t key = evt.bit.NUM;

  
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {

    if(key == 0){
      if(x > 1){
        x = x - 1;
      }else{
        x = 1;
      }
     
    }else if(key == 1){
      if(x < 8){
        x = x + 1;
      }else{
        x = 8;
      }

    }else if(key == 2){
      if(y > 0){
        y = y - 1;
      }else{
        y = 0;
      }

    }else if(key == 3){
      if(y < 1){
        y = y + 1;
      }else{
        y = 1;
      }
    }

    Serial.println(x);
    Serial.println(y);
    neokey.pixels.setPixelColor(key, Wheel(map(key, 0, neokey.pixels.numPixels(), 0, 255)));

  } else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {


    neokey.pixels.setPixelColor(key, 0);
  }

  // Turn on/off the neopixels!
  neokey.pixels.show();
  return 0;
}

void setup() {
  Serial.begin(115200);
  Serial.println("IV18 Test");

  SPI.begin();
  SPI.setBitOrder(LSBFIRST);
  
  display_init();
  display_timer_init();

  display_message(" HELLO");

  if (! neokey.begin(0x30)) {     // begin with I2C address, default is 0x30
    Serial.println("Could not start NeoKey, check wiring?");
    while(1) delay(10);
  }

   for(int i=0; i<NEOKEY_1X4_KEYS; i++){
    neokey.registerCallback(i, blink);
  }

}


void loop() {

  neokey.read();
  message = "";

  // send data only when you receive data:
  if (Serial.available()) {
    message = Serial.readString();
    Serial.println(message);
    char *p = const_cast<char*>(message.c_str());
    display_message(p);
  }
}

/******************************************/

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
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
