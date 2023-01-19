#define Laser1 53

int X = 1;

void setup() {
  pinMode(Laser1, OUTPUT);
  Serial.begin(9600);
  delay(4000);
}

void loop() {
  digitalWrite(Laser1, HIGH);
  delay(10);
  digitalWrite(Laser1, LOW);
  for(int j=0;j<8;j++){
    if(X&(1<<j)){
      digitalWrite(Laser1, HIGH);
      delay(100);
    }
    else{
      digitalWrite(Laser1, LOW);
      delay(100);
    }
  }
  digitalWrite(Laser1,LOW);
  X++;
  delay(3000);
}
