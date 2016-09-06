#include <pic12f629.h>

static __code int __at(_CONFIG) 
	config = _FOSC_INTRCIO
	        & _WDT_OFF
	        & _PWRTE_ON
	        & _MCLRE_OFF;

#define CLOCKPIN    0b00010000; // GP4
#define DATAPIN     0b00000100; // GP3

#define NOTCLOCKPIN 0b11101111; // GP4
#define NOTDATAPIN  0b11111011; // GP3

#define CLK GP4
#define DAT GP3

int width=1000;
int widthhalf=500;
 
#define OUTPUT 0
#define INPUT  1


void wait(int time_ms) {
	static unsigned char t_adj = 50;
	while (time_ms--) {
		TMR0 = t_adj;
		T0IE = 1;
		OPTION_REG = 1;
		while (T0IE) {
			__asm
				nop
				nop
				nop
				nop
			__endasm;
		}
		OPTION_REG = 0;
	}
}

//
//void write(int p,int v) {
//  if(v == 1) GPIO = GPIO | (1 << p);
//  if(v == 0) GPIO = GPIO & (0xFF^(1 << p));
//}

void send_data_bit(int data) {
  char n=0;
	CLK=0;
  wait(1);
	if(data == 1) DAT = 1;
	if(data == 0) DAT = 0;
  wait(1);
	CLK=1;
  wait(2);
	CLK=0;
}
 

void outhigh() {
  DAT=1;
}
 
void send_data(unsigned int data,int len) {
 
  for(int n=len-1;n>=0;n--) {
    if(data & ((unsigned int)1 << n)) {
      send_data_bit(1);
    } else {
      send_data_bit(0);
    }
  }
}
 
void write_op(int phy,int reg,unsigned int data) {
   
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
 
//unsigned int reg3 = 0xE001;
//unsigned int reg4 = 0x2001;

void main() {

	// reset
	OPTION_REG = 0;
	CMCON      = 0x07;
	TRISIO     = 0;
	GPIO       = 0b0;
	GIE  = 1;

	GP1 = 0;

  TRISIO=0;
	GP1 = 0;

OSCCAL = 0x80;
  outhigh();
	for(;;) {
    wait(1); // five seconds
	GP1 = 0;
    wait(1); // five seconds
   
    write_op(20,3,0xE001);
    write_op(20,4,0x2001);
    //for(int n=0;n<5;n++) for(int m=0;m<60;m++) wait(1); // 5mins
	GP1 = 1;
	}
}
