
#define DISP_WIDTH 3

int segPins[] = {5,6,9,10,11,12,13};
int gridPins[] = {18,17,16};

volatile int interruptCounter;
int totalInterruptCounter;

 
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile int dIdx = 0;
volatile int dIdx_prev = 0;
volatile int display_contents[DISP_WIDTH] = {1,2,3};
volatile int currNum1 = 0;
volatile int currNum2 = 0;
volatile int currNum3 = 0;

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
  0b01011111,
  0b01111111,
  0b00111010,
  0b01110101,
  0b00111110,
  0b00011110,
  0b01111010,
  0b01011101,
  0b00011000,
  0b01110001,
  0b01011110,
  0b00111000,
  0b01010010,
  0b01011011,
  0b01111011,
  0b00011111,
  0b01001111,
  0b00011010,
  0b01101110,
  0b00111100,
  0b01111001,
  0b01110000,
  0b01110010, //w
  0b01011101,
  0b01101101,
  0b00110111,
};

int char_to_code(char c) {
  if ((c <= 0x5A) && (c >= 0x41)) { // Uppercase letters
    return c - 0x37;
  } else if ((c <= 0x39) && (c >= 0x30)) { // Digits
    return c - 0x30;
  }
 
  return 0;
}

void writeDigit(int n){
  //uint8_t digit = digitCodeMap[n];
  for(int i=0; i < 7; i++){
    digitalWrite(segPins[i], !bitRead( digitCodeMap[n], i));
    //Serial.print( bitRead( digitCodeMap[n], i), BIN);
  }

}

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);

  digitalWrite(gridPins[dIdx_prev], LOW);
  writeDigit(display_contents[dIdx]);
  //Serial.print("Idx:");
  //Serial.println(dIdx);
  //Serial.print("Value:");
  //Serial.println(display_contents[dIdx]);
  digitalWrite(gridPins[dIdx], HIGH);
  dIdx_prev = dIdx;
  dIdx++;
  
  if (dIdx >= DISP_WIDTH)
    dIdx = 0;

  portEXIT_CRITICAL_ISR(&timerMux);
 
}

void display_timer_init() {
  Serial.println("Display Timer Init");
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000, true);
  timerAlarmEnable(timer);
 
  //digitalWrite(DISP_LOAD, HIGH);
 // display_unblank();
  
} 



void setup() {
   Serial.begin(115200);
  for(int i=0; i < 7; i++){
    pinMode(segPins[i], OUTPUT);
  }
  for(int i=0; i < 3; i++){
    pinMode(gridPins[i], OUTPUT);
  }
  display_timer_init();

}

char p;
volatile int prevNum = 0;
volatile int currNum = 0;
void loop() {
  // put your main code here, to run repeatedly:
  //prevNum = 0;
  if(currNum1 < 35){
    currNum1 = currNum1 + 1;

  }else{
    currNum1 = 0;
  }
  delay(100);
  display_contents[0] = currNum1;
  display_contents[1] = currNum1;
  display_contents[2] = currNum1;
  /*
  if(currNum2 < 35){
    currNum2 = currNum2 + 1;
  }else if(currNum2 == 35){
    if(currNum1 == 35){
      currNum1 = 0;
      currNum2 = 0;   
    }else{
      currNum1 = currNum1 + 1;
      currNum2 = 0;
    }
    
  }
  
  display_contents[0] = currNum1;
  display_contents[1] = currNum2;
  delay(10);
  */
  /*
  display_contents[0] = prevNum;
  display_contents[1] = currNum;
 
 if (Serial.available()) {
    p = Serial.read();
    Serial.println(char_to_code(p));
    prevNum = currNum;
    currNum = char_to_code(p);

  }
  delay(25);
  */
}
