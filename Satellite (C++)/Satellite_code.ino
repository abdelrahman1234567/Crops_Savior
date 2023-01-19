#define Rec1 A0
#define Rec2 A1
#define Laser 8
int BIT = 0;
int Temp = 0;
int Hum = 0;
int Rain = 0;
int CO2 = 0;

void setup() {
  pinMode(Rec1,INPUT);
  pinMode(Rec2,INPUT);
  pinMode(Laser,OUTPUT);
  Serial.println(9600);
}

void loop() {
  // Reading Humidity
  for(int i = 0;i < 10;i++){
    if(analogRead(Rec1) >= 1000){
      Hum += (1<<i);
    }
    delay(100);
  }
delay(3000);
  // Reading temperature
  for(int i = 0;i < 10;i++){
    if(analogRead(Rec1) >= 1000){
      Temp += (1<<i);
    }
    delay(100);
  }
delay(3000);
  // Reading CO2 concentration
  for(int i = 0;i < 10;i++){
    if(analogRead(Rec2) >= 1000){
      CO2 += (1<<i);
    }
    delay(100);
  }
delay(3000);
  // Reading Rain quantity
  for(int i = 0;i < 8;i++){
    if(analogRead(Rec2) >= 1000){
       Rain += (1<<i);
    }
    delay(100);
  }
  delay(100);
  Send(Hum);
  Send(Temp);
  Send(CO2);
  Send(Rain);
}
void Send(int number){
  int Laser;
  if(LaserIdx == 1)Laser = 53;
  else Laser = 52;
  digitalWrite(Laser, HIGH);
  delay(10);
  digitalWrite(Laser, LOW);
  for(int j=0;j<10;j++){
    if(number&(1<<j)){
      digitalWrite(Laser, HIGH);
      delay(100);
    }
    else{
      digitalWrite(Laser, LOW);
      delay(100);
    }
  }
  digitalWrite(Laser,LOW);
  delay(3000);
}
