int clock_pin = 1;
int data_pin  = 0;
 
int width=100;
 
void setup() {
  pinMode(clock_pin, OUTPUT); // CLOCK
  pinMode( data_pin, OUTPUT); // DATA
}
 
int recv_one_bit() {
  int data=0;
  digitalWrite(clock_pin,0);
  delayMicroseconds(width);
  digitalWrite(clock_pin,1);
  delayMicroseconds(width/2);
  data = digitalRead(data_pin);
  delayMicroseconds(width/2);
  digitalWrite(clock_pin,0);
  return data;
}
 
void send_data_bit(int data) {
  //Serial.print("1s");
  digitalWrite(clock_pin,0);
  delayMicroseconds(width/2);
  digitalWrite(data_pin,data);
  delayMicroseconds(width/2);
  digitalWrite(clock_pin,1);
  delayMicroseconds(width);
  digitalWrite(clock_pin,0);
}
 
void outhigh() {
  pinMode(data_pin,OUTPUT);
  digitalWrite(data_pin,1);
  digitalWrite(clock_pin,0);
}
 
void send_data(uint32_t data,int len) {
 
  for(int32_t n=len-1;n>=0;n--) {
  //for(int32_t n=0;n<len;n) {
    if(data & ((uint32_t)1 << n)) {
      send_data_bit(1);
    } else {
      send_data_bit(0);
    }
  }
}
 
uint32_t recv_data(int len) {
 
  //Serial.print("len: ");
  //Serial.println(len,DEC);
  uint32_t datain = 0;
  for(int32_t n=(len-1);n>=0;n--) {
    //Serial.println(n);
    int d = recv_one_bit();
    if(d != 0) {
      datain = datain | (uint32_t)((uint32_t)1 << n);
     // Serial.println("h");
     // Serial.println(((uint32_t)1<<n),HEX);
      //Serial.println(datain,HEX);
    }
  }
  return datain;
}
 
uint32_t read_op(int phy,int reg) {
   
  pinMode(data_pin,OUTPUT);
  send_data_bit(1);
   
  send_data_bit(0);
  send_data_bit(1);
  send_data_bit(1);
  send_data_bit(0);
   
  //send_data(1,2); // start bits
  //send_data(2,2); // read operation
  send_data(phy,5);
  send_data(reg,5);
 
  pinMode(data_pin,INPUT);//INPUT_PULLUP
   
  uint32_t in;
  int inA = recv_one_bit(); // turn around
  //int inB = recv_one_bit(); // this should be required as per spec, but a bit gets missed if I use it.
   
  in = recv_data(16);
 
  //Serial.println("ta bits:");
  //Serial.println(inA);
  //Serial.println(inB);
 
  outhigh();
  //send_data_bit(1);
   
  return in;
}
 
 
void write_op(int phy,int reg,uint32_t data) {
   
  pinMode(data_pin,OUTPUT);
  send_data_bit(1);
 
  send_data_bit(0);
  send_data_bit(1);
  send_data_bit(0);
  send_data_bit(1);
         
  send_data(phy,5);
  send_data(reg,5);
   
  send_data_bit(1);
  send_data_bit(0);
//  send_data(2,2); // Turnaround - required by spec, but causes issues.
   
  send_data(data,16);
}
 
void loop() {

  outhigh();
  delay(5000); // one second
   
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
   
  write_op(20,3,reg3);
  write_op(20,4,reg4);
  delay(1000*30); // 30s
}
