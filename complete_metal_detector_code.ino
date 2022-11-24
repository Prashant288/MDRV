
#include<SoftwareSerial.h>
#define vinPin A5
#define buz 9
#define pulsePin A4

#define led 10

long sumExpect=0; //running sum of 64 sums 
long ignor=0;   //number of ignored sums
long diff=0;        //difference between sum and avgsum
long pTime=0;
long buzPeriod=0; 


#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7
//#define EN1 6
//#define EN2 5

SoftwareSerial mySerial(0, 1); // RX, TX

String data;
int btVal;

void setup() 
{
  Serial.begin(9600);
  pinMode(pulsePin, OUTPUT); 
  digitalWrite(pulsePin, LOW);
  pinMode(vinPin, INPUT);  
  pinMode(buz, OUTPUT);
  digitalWrite(buz, LOW);
  pinMode(led, OUTPUT);

  //Serial.begin(115200);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  //pinMode(EN1, OUTPUT);
  //pinMode(EN2, OUTPUT);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
  //analogWrite(EN1,63);
  //analogWrite(EN2,63);
  mySerial.begin(9600);
}

void loop() 
{
  int minval=1023;
  int maxval=0;
  long unsigned int sum=0;
  for (int i=0; i<256; i++)
  {
    //reset the capacitor
    pinMode(vinPin,OUTPUT);
    digitalWrite(vinPin,LOW);
    delayMicroseconds(20);
    pinMode(vinPin,INPUT);
    applyPulses();
    
    //read the charge of capacitor
    int val = analogRead(vinPin); //takes 13x8=104 microseconds
    minval = min(val,minval);
    maxval = max(val,maxval);
    sum+=val;
    
    long unsigned int cTime=millis();
    char buzState=0;
    if (cTime<pTime+10)
    {
      if (diff>0)
        buzState=1;
      else if(diff<0)
        buzState=2;
    }
    if (cTime>pTime+buzPeriod)
    {
      if (diff>0)
      buzState=1;
      else if (diff<0)
      buzState=2;
      pTime=cTime;   
    }
    if (buzPeriod>300)
    buzState=0;

    if (buzState==0)
    {
      digitalWrite(led, LOW);
      noTone(buz);
    }  
    else if (buzState==1)
    {
      tone(buz,2000);
      digitalWrite(led, HIGH);
    }
    
    else if (buzState==2)
    {
      tone(buz,500);
      digitalWrite(led, HIGH);
    }
  }

  //subtract minimum and maximum value to remove spikes
  sum-=minval; 
  sum-=maxval;
  
  if (sumExpect==0) 
  sumExpect=sum<<6; //set sumExpect to expected value
  long int avgsum=(sumExpect+32)>>6; 
  diff=sum-avgsum;
  if (abs(diff)<avgsum>>10)
  {
    sumExpect=sumExpect+sum-avgsum;
    ignor=0;
  } 
  else 
    ignor++;
  if (ignor>64)
  { 
    sumExpect=sum<<6;
    ignor=0;
  }
  if (diff==0) 
    buzPeriod=1000000;
  else 
  buzPeriod=avgsum/(2*abs(diff));  


while (mySerial.available())
 {  
  {  
      data = mySerial.readStringUntil('\n');
      //Serial.print(str);             
  } 
    
    btVal = (data.toInt());
    //Serial.print("BlueTooth Value ");
    //Serial.println(btVal);    



  switch (btVal) 
   {
      case 1:                                
        //Serial.println("Forward");
        forward();
        break;

      case 2:                 
       //Serial.println("Reverse");
        reverse();
        break;

      case 3:         
       //Serial.println("Left");
        left();
        break;
        
      case 4:                     
        //Serial.println("Right");
        right();
        break;
        
      case 5:                                            
        //Serial.println("Stop");
        stoprobot();
        break;      

  }

 } 
 
                                                              
   if (mySerial.available() < 0)                              
    {
     //Serial.println("No Bluetooth Data ");          
    }
}

void applyPulses()
{
    for (int i=0;i<3;i++) 
    {
      digitalWrite(pulsePin,HIGH); //take 3.5 uS
      delayMicroseconds(3);
      digitalWrite(pulsePin,LOW);  //take 3.5 uS
      delayMicroseconds(3);
    }
}


void forward()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void reverse()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void left()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void right()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
}

void stoprobot()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
}
