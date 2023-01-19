#define Reciever1 A0
int BIT = 0;
int number = 1;
void setup() {
  pinMode(Reciever1, INPUT);
  Serial.begin(9600);
}

void loop() {
  if(analogRead(Reciever1) >= 1000){
    delay(10);
    for(int i = 1;i < 8;i++){
      if(analogRead(Reciever1) >= 1000){
        number += (1<<i);
      }
      delay(100);
    }
    Serial.println(number);
  }
}
