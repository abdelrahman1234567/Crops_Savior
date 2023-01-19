//Libraries
#include <SPI.h>
#include <DHT.h>

//Constants
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
#define LED 2 
#define DO 8 
#define Laser1 53
#define Laser2 52


//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value
int alarm = 0; 
float sensor_volt; 
float RS_gas; 
float ratio; 
//-Replace the name "R0" with the value of R0 in the demo of First Test -/ 
float R0 = 3; 
const int clock = 9;


void resetsensor() { //this function keeps the sketch a little shorter
  SPI.setDataMode(SPI_MODE0);
  SPI.transfer(0x15);
  SPI.transfer(0x55);
  SPI.transfer(0x40);
}

void setup() {
  Serial.begin(9600);
  SPI.begin(); //see SPI library details on arduino.cc for details
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV32); //divide 16 MHz to communicate on 500 kHz
  pinMode(clock, OUTPUT);
  pinMode(LED, OUTPUT); 
  pinMode(DO, INPUT); 
  pinMode(Laser1,OUTPUT);
  pinMode(Laser2,OUTPUT);
  dht.begin();
  delay(100);
}

void loop(){
  TCCR1B = (TCCR1B & 0xF8) | 1 ; //generates the MCKL signal
  analogWrite (clock, 128) ;

  resetsensor(); //resets the sensor - caution: afterwards mode = SPI_MODE0!

  //Calibration word 1
  unsigned int result1 = 0;
  unsigned int inbyte1 = 0;
  SPI.transfer(0x1D); //send first byte of command to get calibration word 1
  SPI.transfer(0x50); //send second byte of command to get calibration word 1
  SPI.setDataMode(SPI_MODE1); //change mode in order to listen
  result1 = SPI.transfer(0x00); //send dummy byte to read first byte of word
  result1 = result1 << 8; //shift returned byte
  inbyte1 = SPI.transfer(0x00); //send dummy byte to read second byte of word
  result1 = result1 | inbyte1; //combine first and second byte of word

  resetsensor(); //resets the sensor

  //Calibration word 2; see comments on calibration word 1
  unsigned int result2 = 0;
  byte inbyte2 = 0;
  SPI.transfer(0x1D);
  SPI.transfer(0x60);
  SPI.setDataMode(SPI_MODE1);
  result2 = SPI.transfer(0x00);
  result2 = result2 << 8;
  inbyte2 = SPI.transfer(0x00);
  result2 = result2 | inbyte2;
  
  resetsensor(); /  /resets the sensor

  //Calibration word 3; see comments on calibration word 1
  unsigned int result3 = 0;
  byte inbyte3 = 0;
  SPI.transfer(0x1D);
  SPI.transfer(0x90);
  SPI.setDataMode(SPI_MODE1);
  result3 = SPI.transfer(0x00);
  result3 = result3 << 8;
  inbyte3 = SPI.transfer(0x00);
  result3 = result3 | inbyte3;

  resetsensor(); //resets the sensor

  //Calibration word 4; see comments on calibration word 1
  unsigned int result4 = 0;
  byte inbyte4 = 0;
  SPI.transfer(0x1D);
  SPI.transfer(0xA0);
  SPI.setDataMode(SPI_MODE1);
  result4 = SPI.transfer(0x00);
  result4 = result4 << 8;
  inbyte4 = SPI.transfer(0x00);
  result4 = result4 | inbyte4;

  //now we do some bitshifting to extract the calibration factors out of the calibration words;
  long c1 = (result1 >> 1) & 0x7FFF;
  long c2 = ((result3 & 0x003F) << 6) | (result4 & 0x003F);
  long c3 = (result4 >> 6) & 0x03FF;
  long c4 = (result3 >> 6) & 0x03FF;
  long c5 = ((result1 & 0x0001) << 10) | ((result2 >> 6) & 0x03FF);
  long c6 = result2 & 0x003F;

  resetsensor(); //resets the sensor

  //Pressure:
  unsigned int presMSB = 0; //first byte of value
  unsigned int presLSB = 0; //last byte of value
  unsigned int D1 = 0;
  SPI.transfer(0x0F); //send first byte of command to get pressure value
  SPI.transfer(0x40); //send second byte of command to get pressure value
  delay(35); //wait for conversion end
  SPI.setDataMode(SPI_MODE1); //change mode in order to listen
  presMSB = SPI.transfer(0x00); //send dummy byte to read first byte of value
  presMSB = presMSB << 8; //shift first byte
  presLSB = SPI.transfer(0x00); //send dummy byte to read second byte of value
  D1 = presMSB | presLSB; //combine first and second byte of value

  resetsensor(); //resets the sensor

  //Temperature:
  unsigned int tempMSB = 0; //first byte of value
  unsigned int tempLSB = 0; //last byte of value
  unsigned int D2 = 0;
  SPI.transfer(0x0F); //send first byte of command to get temperature value
  SPI.transfer(0x20); //send second byte of command to get temperature value
  delay(35); //wait for conversion end
  SPI.setDataMode(SPI_MODE1); //change mode in order to listen
  tempMSB = SPI.transfer(0x00); //send dummy byte to read first byte of value
  tempMSB = tempMSB << 8; //shift first byte
  tempLSB = SPI.transfer(0x00); //send dummy byte to read second byte of value
  D2 = tempMSB | tempLSB; //combine first and second byte of value

  //calculation of the real values by means of the calibration factors and the maths
  //in the datasheet. const MUST be long
  const long UT1 = (c5 << 3) + 20224;
  const long dT = D2 - UT1;
  const long TEMP = 200 + ((dT * (c6 + 50)) >> 10);
  const long OFF  = (c2 * 4) + (((c4 - 512) * dT) >> 12);
  const long SENS = c1 + ((c3 * dT) >> 10) + 24576;
  const long X = (SENS * (D1 - 7168) >> 14) - OFF;
  long PCOMP = ((X * 10) >> 5) + 2500;
  float TEMPREAL = TEMP / 10;
  float PCOMPHG = PCOMP * 750.06 / 10000; // mbar*10 -> mmHg === ((mbar/10)/1000)*750/06

  //2-nd order compensation only for T < 20°C or T > 45°C

  long T2 = 0;
  float P2 = 0;

  if (TEMP < 200) {
    T2 = (11 * (c6 + 24) * (200 - TEMP) * (200 - TEMP) ) >> 20;
    P2 = (3 * T2 * (PCOMP - 3500) ) >> 14;
  }
  else if (TEMP > 450)
  {
    T2 = (3 * (c6 + 24) * (450 - TEMP) * (450 - TEMP) ) >> 20;
    P2 = (T2 * (PCOMP - 10000) ) >> 13;
  }

  if ((TEMP < 200) || (TEMP > 450)){
    const float PCOMP2 = PCOMP - P2;
    float PCOMPHG2 = PCOMP2 * 750.06 / 10000; // mbar*10 -> mmHg === ((mbar/10)/1000)*750/06
  }
  hum = dht.readHumidity(); //Read Humidity
  temp= dht.readTemperature(); //Read temperature
  
  int sensorValue = analogRead(A0); 
  sensor_volt = ((float)sensorValue / 1024) * 5.0; 
  RS_gas = (5.0 - sensor_volt) / sensor_volt; // Depend on RL on yor module 
  ratio = RS_gas / R0; // ratio = RS/R0 

  Send(hum,1);
  Send(temp,1);
  Send(Rs_gas,2);
  Send(PCOM2,2);
  delay(16500);
}

void Send(int number, int LaserIdx){
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
