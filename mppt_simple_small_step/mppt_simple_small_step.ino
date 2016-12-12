/*
  Maximum Power Point Tracking - Simple Small 

  This is the code of Maximum Power Point Tracking 
  which uses simple perturb and observe method to 
  reach the maximum power point with a fixed step 
  size which is very small. This large step size
  causes the converter reach maximum power point very slowly.
  
  The circuit:
  * A5 - Analog Input from the current sensor
  * A2 - Analog Input from the voltage sensor
  * 2 - PWM output to the MOSFET of the power converter

  Created 08 JUNE 2016
  By Kartik Samtani

*/

int current; // stores current flowing into the converter - at input
float voltage=0.0; // stores the voltage applied across the input terminals of the converter
float prev_voltage=0.0; // stores the voltage applied across the input terminals of the converter at the previous instant of computation
float power=0.0; // stores the power drawn by the converter at its input terminals from the source
float prev_power=0.0; // stores the power drawn by the converter at its input terminals from the source at the previous instant of computation
int pwm_value = 180; // by defualt the PWM value applied to the MOSFET of the DC-DC converter
int steps = 2; // step size of the PWM values if the operating point of the solar cell is such that V_(solar) < V_(mpp)
int steps2 = 2; // step size of the PWM values if the operating point of the solar cell is such that V_(solar) < V_(mpp)
long unsigned int time_stamp; // a variable which stores the appropriate time stamp for data logging purpose

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
   current = (int)(temp_current/10);
   current = current * 10;
   suma+=current;
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
  pinMode(A2,INPUT); // Declaring pins as inputs or outputs 
  pinMode(A5,INPUT);
  pinMode(2,OUTPUT);
  TCCR3B &= ~0b00000111; // This is to configure the Timer 3 register of ATmel 2560 to give PWM of frequency 32.5KHz at pin 2
  TCCR3B |= 0b00000001;
  Serial.begin(9600); // Configure the UART protocol to work at 9600 bauds
}

void loop() 
{
 current = meas_current(); // acquire current and voltage values
 voltage = meas_voltage();
 
 time_stamp = millis(); // get time stamp
 power = current*voltage; // compute power
 
 if(power >= prev_power)
 {
  if(voltage > prev_voltage) // region of operation where V_(solar) < V_(mpp)
   {
     pwm_value-=steps;
     if(pwm_value<5) // placing lower and upper bounds on the PWM values
      pwm_value = 5;
     if(pwm_value>250)
      pwm_value = 250;
     analogWrite(2,pwm_value);
   }
  else // region of operation where V_(solar) > V_(mpp)
   {
    //steps = 2;
    pwm_value+=steps2;
    if(pwm_value<5) // placing lower and upper bounds on the PWM values
     pwm_value = 5;
    if(pwm_value>250)
     pwm_value = 250;
    analogWrite(2,pwm_value);
   }
 }
else // region of operation where V_(solar) > V_(mpp)
 {
  if(voltage >= prev_voltage)
   {
     pwm_value+=steps2;
     if(pwm_value<5) // placing lower and upper bounds on the PWM values
      pwm_value = 5;
     if(pwm_value>250)
      pwm_value = 250;
     analogWrite(2,pwm_value);
   }
  else // region of operation where V_(solar) < V_(mpp)
   {
    pwm_value-=steps;
    if(pwm_value<5) // placing lower and upper bounds on the PWM values
     pwm_value = 5;
    if(pwm_value>250)
     pwm_value = 250;
    analogWrite(2,pwm_value);
   }
 } 
 prev_power = power; // Updating the values of prev_power and prev_voltage
 prev_voltage = voltage;
 
 Serial.print(time_stamp); // logging current, voltage and pwm values with the time stamp
 Serial.print(" ");
 Serial.print(voltage);
 Serial.print(" ");
 Serial.print(current);
 Serial.print(" ");
 Serial.println(pwm_value);  
}

