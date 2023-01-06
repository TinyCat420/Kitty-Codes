
//NOTES:

//pins  15, 14, 18, 19, 00, 01, 21, 20, 23, 22, 16, 17, 13, 11, 12, 10
//gpio6 19, 18, 17, 16, 03, 02, 27, 26, 25, 24, 23, 22
//gpio7                                                03, 02, 01, 00
//gpio06 2-3, 16-19, 22-27 and gpio07 0-3


//alternate pins 41, 40, 39, 38
//gpio6          21, 20, 29, 28
//gpio6 2-3, 16-29


//wiring
//alternate
/*
data gpio pins
 00   16   19
 01   17   18
 02   18   14
 03   19   15
 04   20   40
 05   21   41
 06   22   17
 07   23   16
 08   24   22
 09   25   23
 10   26   20
 11   27   21
 12   28   38
 13   29   39
 14   02   01
 15   03   00
*/


//CODE:

//gpio6 + gpio7
void set_16bit(uint8_t mode) {
  const int pins[16] = { 15, 14, 18, 19, 0, 1, 21, 20, 23, 22, 16, 17, 13, 11, 12, 10};
  for (int i = 0; i < 16; i++)
    pinMode(pins[i], mode);
}

//all gpio6
void set_16bit_alternate(uint8_t mode) {
  const int pins[16] = { 15, 14, 18, 19, 0, 1, 39, 38, 21, 20, 23, 22, 16, 17, 41, 40};
  for (int i = 0; i < 16; i++)
    pinMode(pins[i], mode);
}



//gpio6 + gpio7
//0x03f0 0000 0011 1111 0000 in binary
//0x0c00 0000 1100 0000 0000 in binary
inline void write_16bit(uint16_t data) {
  //               4+18=22 9+18=27         10-8=2 11-8=3             12+4=16 15+4=19
  //          data 4-9 to gpio6 22-27  data 10-11 to gpio6 2-3  data 12-15 to gpio6 16-19
  GPIO6_DR = ((data & 0x03f0) << 18) | ((data & 0x0c00) >> 8) | ((data & 0xf000) << 4);
  //       data 0-3 to gpio7 0-3
  GPIO7_DR = (data & 0x0f);
}

//all gpio6
//new gpio 20,21 28,29
//0011 1111 1111 1111 in hex 0x3fff
//1100 0000 0000 0000 in hex 0xc000
inline void write_16bit_alternate(uint16_t data) {
  //            0+16=16 13+16=29              14-12=2 15-12=3
  //         data 0-13 gpio 16-29          data 14-15 gpio 2-3
  GPIO6_DR = ((data & 0x3fff) << 16) | ((data & 0xc000) >> 12);
}



void setup() {
  Serial.begin(115200);
  set_16bit_alternate(OUTPUT);


}

uint8_t dac1 = 1;
uint8_t dac2 = 1;
uint16_t dac;
void loop() {

  /*
 //cycles each set of 8 pins
 for(int i=0; i<16; i++){ 
  dac1 = dac1<<i;
  dac2 = dac2<<i;
  dac = (dac2<<8) + dac1;
  write_16bit_alternate(dac);
  delay(500);
 }
 */

 //ramps
  for(int i=0; i<256; i++){ 
   dac1 = i;
   dac = (dac2<<8) + dac1;
   write_16bit_alternate(dac);
   delay(10);
 }
 for(int i=0; i<256; i++){ 
   dac2 = i;
   dac = (dac2<<8) + dac1;
   write_16bit_alternate(dac);
   delay(10);
 }
 
}





//reference
//gpio6 pins 22 - 25 and 16 - 19
inline void write_8bit(uint8_t data) {
  //          shift bottom 4 bits 16   shift top 4 bits 18
  GPIO6_DR  = ((data & 0x0f) << 16) | ((data & 0xf0) << 18);
  //            lines up data to 16-19 and 22-25 on gpio6 port
}
