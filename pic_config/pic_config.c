#define NO_BIT_DEFINES
#include <pic14regs.h>
#include <stdint.h>

__code uint16_t __at (_CONFIG) __configword = _INTRC_OSC_NOCLKOUT & _WDTE_OFF;

#define CLOCKPIN    0b00010000; // GP4
#define DATAPIN     0b00000100; // GP3

#define NOTCLOCKPIN 0b11101111; // GP4
#define NOTDATAPIN  0b11111011; // GP3

int width=1000;
int widthhalf=500;
 
#define OUTPUT 0
#define INPUT  1

void ndelay(int t) {
  for(int n=0;n<t;n++);
}

void write(int p,int v) {
  if(v == 1) GPIO = GPIO | (1 << p);
  if(v == 0) GPIO = GPIO & (0xFF^(1 << p));
}

void send_data_bit(int data) {
  char n=0;
  GPIO = 0;                 // CLK 0
  for(n=0;n<255;n++);  // DELAY
  if(data == 1) GPIO = DATAPIN; // DATA 1 (otherwise do nothing)
  for(n=0;n<255;n++);  // DELAY
  GPIO = GPIO | CLOCKPIN; // CLK 1
  for(n=0;n<255;n++);  // DELAY
  for(n=0;n<255;n++);  // DELAY
  GPIO = GPIO & NOTCLOCKPIN; // CLK 0
}
 

void outhigh() {
  GPIO = DATAPIN;
}
 
void send_data(uint16_t data,int16_t len) {
 
  for(int16_t n=len-1;n>=0;n--) {
    if(data & ((uint16_t)1 << n)) {
      send_data_bit(1);
    } else {
      send_data_bit(0);
    }
  }
}
 
void write_op(int phy,int reg,uint16_t data) {
   
  send_data_bit(1);
 
  send_data_bit(0);
  send_data_bit(1);
  send_data_bit(0);
  send_data_bit(1);
         
  send_data(phy,5);
  send_data(reg,5);
   
  send_data_bit(1); // Turn around
  send_data_bit(0);
   
  send_data(data,16);
}
 
//Port mirroring config sits in phy 20, regs 3 and 4.
//The bit pattern is shown below. See the datasheet for further details.
//15.......................0  
//   enable, mode, res, mirrored port rx
//3: 1,   11,00000,00000001
//
//   monitoring port, res, mirrored port tx
//4: 001 ,00000,00000001  
 
uint16_t reg3 = 0xE001;
uint16_t reg4 = 0x2001;

void main() {

  TRISIO=0;
  // Configure port

  outhigh();
  ndelay(100000); // five seconds
   
  write_op(20,3,reg3);
  write_op(20,4,reg4);
  for(int n=0;n<5;n++) for(int m=0;m<60;m++) ndelay(20000); // 5mins
}
