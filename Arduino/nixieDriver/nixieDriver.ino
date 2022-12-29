
int outPins[] = {5,6,9,10,11,12,13,14};
#define BIT(n,i) (n>>i&1)
volatile int countL = 0;
volatile int countR = 0;

void writeDigit(int l, int r){
  //uint8_t digit = digitCodeMap[n];
  for(int i=0; i < 4; i++){
    digitalWrite(outPins[i], BIT(l,i));
    Serial.print( BIT(l,i), BIN);
  }
  Serial.println();
  for(int i=4; i < 8; i++){
    digitalWrite(outPins[i], BIT(r,i-4));
    Serial.print( BIT(r,i-4), BIN);
  }
  Serial.println();
  Serial.println();
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  for(int i=0; i < sizeof(outPins); i++){
    pinMode(outPins[i], OUTPUT);
  }
  writeDigit(countL, countR);
}

void loop() {
  writeDigit(countL,countR);
  //Serial.println(countL);
  //Serial.println(countR);
  Serial.println();
  if(countL == 9 && countR == 9){
    countR = 0;
    countL = 0;
  }
  if(countR == 9){
    countR = 0;
    countL++;
  }else{
    countR++;
  }
  
  
  delay(500);
}
  


