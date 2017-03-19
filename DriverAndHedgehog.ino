#define SPEED 120
#define RIGHT_K 0.95
#define LEFT_K 1

#define HEDGEHOG_PACKET_SIZE 13
#define HEDGEHOG_PACKET_HEADER_SIZE 5

#define ADDRESS         0
#define PACKET_TYPE     1
#define DATA_CODE       2
#define DATA_AMOUNT     4
#define COMMAND_MODE    5
/*
0 – stop
1 – power control
2 – speed control	N
*/
#define MOVEMENT        6
/*
1	uint8_t	Type of movement (for modes 1 and 2):
0 – move forward
1 – move backward
2 – rotate right (clockwise)
3 – rotate left (counterclockwise)	
*/
#define PERCENTS        7
#define RESERVED	8
#define CONTROL_TYPE    10
/*
0 – control by sliders in motors window
1 – control by arrows in main window
*/
#define CHECKSUM        11

#define STEP 25
#define LEFT_WHEEL_ENABLE 8
#define RIGHT_WHEEL_ENABLE 9
#define DIR_LEFT_WHEEL 12
#define DIR_RIGHT_WHEEL 13
#define SIGNAL_LEFT_WHEEL 10
#define SIGNAL_RIGHT_WHEEL 11
#define PWM_LEFT_WHEEL 5
#define PWM_RIGHT_WHEEL 6

signed int leftWheelVelocity = 0;
signed int rightWheelVelocity = 0;

bool rideForward = false;
bool rideBack = false;
bool rightTurn = false;
bool leftTurn = false;

const int hedgehogPacketHeader[HEDGEHOG_PACKET_HEADER_SIZE] = {0xff, 0x47, 0x00, 0x10, 0x06};

byte hedgehogBuf[HEDGEHOG_PACKET_SIZE];

byte headehogBufOfs = 0;

bool isCommandPacket(byte *buf)
{
  for(uint8_t i = 0; i < HEDGEHOG_PACKET_HEADER_SIZE; ++i){
    if (buf[i] != hedgehogPacketHeader[i]){
      return false;
    }
  }
  return true;
}

void hedgehogListening()
{
 byte incomingByte;
 int totalReceivedInLoop = 0;
 bool packetReceived = false;

 while(Serial.available()){
   if (headehogBufOfs >= HEDGEHOG_PACKET_SIZE){
     break;
   }
   
   ++totalReceivedInLoop;
   
   if (totalReceivedInLoop > 100){
     break;// too much data without required header
   }
   
   incomingByte = Serial.read();
   
   // check first 5 bytes for constant value 
   if (headehogBufOfs < HEDGEHOG_PACKET_HEADER_SIZE){
     if (incomingByte != hedgehogPacketHeader[headehogBufOfs]) {
       headehogBufOfs = 0;// restart bufer fill
       continue;
     }
   }
   
   hedgehogBuf[headehogBufOfs++] = incomingByte;
   
   if (headehogBufOfs == HEDGEHOG_PACKET_SIZE){ // received packet with required header
     packetReceived = true;
     headehogBufOfs = 0;// restart bufer fill
     break; 
   }
 }
 
 if (packetReceived){
   //CalcCrcModbus(hedgehogBuf, HEDGEHOG_PACKET_SIZE);// calculate CRC checksum of packet
   //if ((hedgehogBuf[CHECKSUM] == 0)&&(hedgehogBuf[CHECKSUM + 1] == 0)){// checksum success
   switch(hedgehogBuf[COMMAND_MODE]){
     case 0x00:
      leftWheelVelocity = 0;
      rightWheelVelocity = 0;
      rideBack = false;
      rideForward = false;
      rightTurn = false;
      leftTurn = false;
     break;
     
     case 0x01:
     break;
     
     case 0x02:
      rideBack = false;
      rideForward = false;
      rightTurn = false;
      leftTurn = false;
      switch(hedgehogBuf[MOVEMENT]){ 
        case 0:
          rideForward = true;
          leftWheelVelocity = (int)(SPEED * LEFT_K);
          rightWheelVelocity = (int)(SPEED * RIGHT_K);
        break;
        
        case 1:
          rideBack = true;
          leftWheelVelocity = (int)(-SPEED * LEFT_K);
          rightWheelVelocity = (int)(-SPEED * RIGHT_K);
        break;
        
        case 2:
          rightTurn = true;
          leftWheelVelocity = (int)(SPEED * LEFT_K);
          rightWheelVelocity = (int)(-SPEED * RIGHT_K);
        break;
        
        case 3:
          leftTurn = true;
          leftWheelVelocity = (int)(-SPEED * LEFT_K);
          rightWheelVelocity = (int)(SPEED * RIGHT_K);          
        break;
      }
     break;
   }
   packetReceived = 0;
   
   Serial.print("direction = ");
   if (rideForward){
     Serial.println("ride forward");
     digitalWrite(DIR_LEFT_WHEEL, LOW);
     digitalWrite(DIR_RIGHT_WHEEL, LOW);
   }
   else if(rideBack){
     Serial.println("ride back");
     digitalWrite(DIR_LEFT_WHEEL, HIGH);
     digitalWrite(DIR_RIGHT_WHEEL, HIGH);
   }
   else if(rightTurn){
     Serial.println("right turn");
     digitalWrite(DIR_LEFT_WHEEL, LOW);
     digitalWrite(DIR_RIGHT_WHEEL, HIGH);            
   }
   else if(leftTurn){
     Serial.println("left turn");
     digitalWrite(DIR_LEFT_WHEEL, HIGH);
     digitalWrite(DIR_RIGHT_WHEEL, LOW);      
   }
   else{
     Serial.println("stop");
   }
    
   analogWrite(PWM_LEFT_WHEEL, abs(leftWheelVelocity));
   analogWrite(PWM_RIGHT_WHEEL, abs(rightWheelVelocity));
  
   
   Serial.print("Left wheel velocity = ");
   Serial.println(leftWheelVelocity);   
   Serial.print("Right wheel velocity = ");
   Serial.println(rightWheelVelocity);
 }
//}
}

void setup()
{
  TCCR0B = TCCR0B & 0b11111000 | 2; // для увеличения частоты шима (около 7 кГц)
  
  pinMode(LEFT_WHEEL_ENABLE, OUTPUT);
  pinMode(RIGHT_WHEEL_ENABLE, OUTPUT);
  digitalWrite(LEFT_WHEEL_ENABLE, HIGH);
  digitalWrite(RIGHT_WHEEL_ENABLE, HIGH);
  
  pinMode(PWM_LEFT_WHEEL, OUTPUT);
  pinMode(PWM_RIGHT_WHEEL, OUTPUT);
  
  pinMode(SIGNAL_LEFT_WHEEL, OUTPUT);
  pinMode(SIGNAL_RIGHT_WHEEL, OUTPUT);
  digitalWrite(SIGNAL_LEFT_WHEEL, HIGH);
  digitalWrite(SIGNAL_RIGHT_WHEEL, HIGH);
  
  pinMode(DIR_LEFT_WHEEL, OUTPUT);
  pinMode(DIR_RIGHT_WHEEL, OUTPUT);
  digitalWrite(DIR_LEFT_WHEEL, LOW);
  digitalWrite(DIR_RIGHT_WHEEL, LOW);
  
  Serial.begin(115200);
}

void loop()
{
  hedgehogListening();
}
