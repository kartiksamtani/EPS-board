//#include <SPI.h>
//#include <SD.h>

//const int chipSelect = 4;
int current;
float voltage=0.0;
byte pwm_value = 0;
long unsigned int time_stamp;
long unsigned int t1;
long unsigned int t2;

int meas_current()
{
  int i = 0;
  int samples = 100;
  float sum=0;
  int value;
  int new_value;
  float temp_current;
  int current;
  int suma=0;
  int avg_current=0.0;
  
  for(int j=0;j<20;j++)
  {
  for(i=0;i<samples;i++)
   sum+=analogRead(A5);
   value = sum/samples;
   new_value = value - 158;
   temp_current = new_value*1.272;
   if(temp_current<0)
   temp_current = 0;
   sum=0;
   //current = (int)(temp_current/10);
   //current = current;
   suma+=temp_current;
   //suma+=temp_current;
   }
   avg_current = (suma/200)*10;
   return avg_current+40;
}


float meas_voltage()
{
  int i = 0;
  int samples = 200;
  float sum=0;
  int value;
  int new_value;
  float voltage;
  
  for(i=0;i<samples;i++)
  sum+=analogRead(A2); 
  value = sum/samples;
  new_value = value;
  voltage = (float)new_value*5/1023;
  sum = 0;
  return voltage*3.5;
}


void setup() {
  // put your setup code here, to run once:
  pinMode(A1,INPUT);
  pinMode(A5,INPUT);
  pinMode(2,OUTPUT);
  TCCR3B &= ~0b00000111;
  TCCR3B |= 0b00000001;
  //Serial.begin(9600);
    // Open serial communications and wait for port to open:
  Serial.begin(9600);
  /*while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  // pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  SD.remove("datalog.txt");*/
}

void loop() {
  // put your main code here, to run repeatedly:
 analogWrite(2,pwm_value++);
 //t1 =millis();
 current = meas_current();
 //t2 =millis();
 //Serial.println(t2 - t1);
 voltage = meas_voltage();
 time_stamp = millis();
 //
// dataString = String(time_stamp) + " " + String(voltage) + " " + String(current) + " " + String(pwm_value-1);
 Serial.print(time_stamp);
 Serial.print(" ");
 Serial.print(voltage);
 Serial.print(" ");
 Serial.print(current);
 Serial.print(" ");
 Serial.println(pwm_value-1);
 //
 //
 /*File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }*/
  //t2 =millis();
  //Serial.println(t2 - t1);
 
 if(pwm_value==250)
  pwm_value+=6;
}

