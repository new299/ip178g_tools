#define NO_BIT_DEFINES
#include <pic14regs.h>
#include <stdint.h>

__code uint16_t __at (_CONFIG) __configword = _INTRC_OSC_NOCLKOUT & _WDTE_OFF;

int clock_pin = 4; // GP4
int data_pin  = 3; // GP3

int width=100;
 
#define OUTPUT 0
#define INPUT  1

void delay(int t) {
  for(int n=0;n<t*20;n++);
}

void write(int p,int v) {
  if(v == 1) GPIO = GPIO | (1 << p);
  if(v == 0) GPIO = GPIO & (0xFF^(1 << p));
}

void pinMode(int p,int dir) {
  if(dir == OUTPUT) TRISIO=0;
  if(dir == INPUT ) TRISIO=0xFF;
}

/*
int read(int p) {
  if(GPIO & (1 << p)) return 1; else return 0;
}

int recv_one_bit() {
  int data=0;
  write(clock_pin,0);
  delay(width);
  write(clock_pin,1);
  delay(width/2);
  data = read(data_pin);
  delay(width/2);
  write(clock_pin,0);
  return data;
}
*/
 
void send_data_bit(int data) {
  write(clock_pin,0);
  delay(width/2);
  write(data_pin,data);
  delay(width/2);
  write(clock_pin,1);
  delay(width);
  write(clock_pin,0);
}
 

void outhigh() {
  pinMode(data_pin,OUTPUT);
  write(data_pin,1);
  write(clock_pin,0);
}
 
void send_data(uint32_t data,int32_t len) {
 
  for(int32_t n=len-1;n>=0;n--) {
    if(data & ((uint32_t)1 << n)) {
      send_data_bit(1);
    } else {
      send_data_bit(0);
    }
  }
}

/*
uint32_t recv_data(int32_t len) {
 
  uint32_t datain = 0;
  for(int32_t n=(len-1);n>=0;n--) {
    int d = recv_one_bit();
    if(d != 0) {
      datain = datain | (uint32_t)((uint32_t)1 << n);
    }
  }
  return datain;
}
 
uint32_t read_op(int phy,int reg) {
  int inA;
  uint32_t inp;
   
  pinMode(data_pin,OUTPUT);
  send_data_bit(1);
   
  send_data_bit(0); // start bits
  send_data_bit(1);
  send_data_bit(1); // read op
  send_data_bit(0);
   
  send_data(phy,5);
  send_data(reg,5);
 
  pinMode(data_pin,INPUT);
   
  inA = recv_one_bit(); // turn around
  //int inB = recv_one_bit(); // this should be required as per spec, but a bit gets missed if I use it.
   
  inp = recv_data(16);
 
  outhigh();
   
  return inp;
}
*/
 
 
void write_op(int phy,int reg,uint32_t data) {
   
  pinMode(data_pin,OUTPUT);
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
 
uint32_t reg3 = 0xE001;
uint32_t reg4 = 0x2001;

void main() {

  // Configure port

  outhigh();
  delay(5000); // five seconds
   
  write_op(20,3,reg3);
  write_op(20,4,reg4);
  for(int n=0;n<5;n++) for(int m=0;m<60;m++) delay(1000); // 5mins
}
