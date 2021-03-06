/*
  Maximum Power Point Tracking - Power sweep and MOSFET tester 

  This is the code for acquiring the IV characteristic of the 
  power source - laboratory one/solar cell emulator one. 
  
  The circuit:
  * A5 - Analog Input from the current sensor
  * A2 - Analog Input from the voltage sensor
  * 2 - PWM output to the MOSFET of the power converter
  * Resitor divider as the voltage sensor (56K an 22k OUTPUT across 22K)
  * ACS712 based current sensor

  Created 08 JUNE 2016
  By Kartik Samtani

*/
int current; // stores current flowing into the converter - at input
float voltage=0.0; // stores the voltage applied across the input terminals of the converter
byte pwm_value = 0; // by defualt the PWM value applied to the MOSFET of the DC-DC converter
long unsigned int time_stamp; // a variable which stores the appropriate time stamp for data logging purpose

int meas_current() // Routine to measure the current flowing into the dc-dc converter using ACS712 sensor, Output - current value in mA in integer format
{
  int i = 0;
  int samples = 100; // number of samples over which the average is to be taken
  float sum=0; // accumulator variable for finding the average current
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
   value = sum/samples; // callibration steps determined by experimentation using the laboratory source which gave a constant current flowing through the sensor
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
   avg_current = (suma/200)*10; // this step makes the 3 digit value of current in mA insesitive at the unit's place
   return avg_current+40; // while returning the value of current, an offset of 40 was given to get the actual value of current for data-logging purposes
}


float meas_voltage() // Routine to measure the voltage across the input terminals of dc-dc converter, Output - voltage value in volts in integer format
{
  int i = 0;
  int samples = 200; // number of values over which the average is to be taken
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
  pinMode(A1,INPUT); // Declaring pins as inputs or outputs 
  pinMode(A5,INPUT);
  pinMode(2,OUTPUT);
  TCCR3B &= ~0b00000111; // This is to configure the Timer 3 register of ATmel 2560 to give PWM of frequency 32.5KHz at pin 2
  TCCR3B |= 0b00000001;
  Serial.begin(9600); // Configure the UART protocol to work at 9600 bauds
}

void loop() {
 analogWrite(2,pwm_value++); // continuous incrementing of PWM values to get IV characteristic of the power source
 current = meas_current(); // acquire current and voltage values
 voltage = meas_voltage();
 time_stamp = millis();
 Serial.print(time_stamp); // log the data acquired through UART
 Serial.print(" ");
 Serial.print(voltage);
 Serial.print(" ");
 Serial.print(current);
 Serial.print(" ");
 Serial.println(pwm_value-1);
 if(pwm_value==250) // upper limit on the PWM values
  pwm_value+=6;
}

