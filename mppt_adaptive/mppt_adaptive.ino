/*
  Maximum Power Point Tracking - Adaptive 

  This is the code of Maximum Power Point Tracking 
  which uses adaptive perturb and observe method to 
  reach the maximum power point very fast along with 
  minimal oscillations about the maximum power point
  
  The circuit:
  * A5 - Analog Input from the current sensor
  * A2 - Analog Input from the voltage sensor
  * 2 - PWM output to the MOSFET of the power converter
  * Resitor divider as the voltage sensor (56K an 22k OUTPUT across 22K)
  * ACS712 based current sensor

  Created 15 JUNE 2016
  By Kartik Samtani

*/

int current; // stores current flowing into the converter - at input
float voltage=0.0; // stores the voltage applied across the input terminals of the converter
float prev_voltage=0.0; // stores the voltage applied across the input terminals of the converter at the previous instant of computation
float power=0.0; // stores the power drawn by the converter at its input terminals from the source
float prev_power=0.0; // stores the power drawn by the converter at its input terminals from the source at the previous instant of computation
int pwm_value = 180; // by defualt the PWM value applied to the MOSFET of the DC-DC converter
int steps = 2; // Initial step size of the PWM values if the operating point of the solar cell is such that V_(solar) < V_(mpp)
int steps2 = 2; // Initial step size of the PWM values if the operating point of the solar cell is such that V_(solar) > V_(mpp)
int upper_limit_l = 8; // Upper limit on the value of 'steps' variable
int upper_limit_r = 12; // Upper limit on the value of 'steps2' variable
int count_l = 0; // a counter variable 
int count_r = 0; // a counter variable
int start_incr_l = 0; // a signal variable to indicate when to start incrementing the value of 'steps' variable
int start_incr_r = 0; // a signal variable to indicate when to start incrementing the value of 'steps2' variable
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
   suma+=temp_current;
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
 
 if(count_l>2) // if the solar cell/ laboratory power source operates in the region V_(solar) < V_(mpp) for 2 or more iterations then enable incrementing of 'steps' variable
  {
    start_incr_l = 1;
    count_l = 2;
  }
 else
  start_incr_l = 0;
  
 if(count_r>2) // if the solar cell/ laboratory power source operates in the region V_(solar) > V_(mpp) for 2 or more iterations then enable incrementing of 'steps2' variable
  {
    start_incr_r = 1;
    count_r = 2;
  }
 else
  start_incr_r = 0; // else don't increment the values of 'steps' and 'steps2'
  
 if(power >= prev_power) 
 {
  if(voltage > prev_voltage)  // region of operation where V_(solar) < V_(mpp)
   {
     if(start_incr_l == 1)
     steps = steps+2; // increment the value of 'steps' variable by 2 as long as start_incr_l is 1
     if(steps>upper_limit_l) // limit the growth of 'steps' variable when it goes beyond its upper limit
      steps = upper_limit_l;
      steps2 = 1; // reduce the value of 'steps2' to 1 the moment power source starts operating in region V_(solar) < V_(mpp)
     pwm_value-=steps;
     if(pwm_value<5) // place lower and upper bounds on the PWM values
      pwm_value = 5;
     if(pwm_value>250)
      pwm_value = 250;
     analogWrite(2,pwm_value); // output the PWM signal from pin 2
     count_l++; // increment count_l as long as power source/solar cell operates in this region
     count_r = 0; // reset count_r
   }
  else // region of operation where V_(solar) > V_(mpp)
   {
    if(start_incr_r == 1)
    steps2 = steps2+2; // increment the value of 'steps2' variable by 2 as long as start_incr_r is 1
    if(steps2>upper_limit_r) // limit the growth of 'steps2' variable when it goes beyond its upper limit
      steps2 = upper_limit_r;
      steps = 1; // reduce the value of 'steps' to 1 the moment power source starts operating in region V_(solar) < V_(mpp)
    pwm_value+=steps2;
    if(pwm_value<5) // place lower and upper bounds on the PWM values
     pwm_value = 5;
    if(pwm_value>250)
     pwm_value = 250;
    analogWrite(2,pwm_value); // output the PWM signal from pin 2
    //flag = 1;
    count_r++; // increment count_l as long as power source/solar cell operates in this region
    count_l = 0; // reset count_l
   }
 }
else
 {
  if(voltage >= prev_voltage) // region of operation where V_(solar) > V_(mpp)
   {
     if(start_incr_r == 1)
     steps2 = steps2+2; // increment the value of 'steps2' variable by 2 as long as start_incr_r is 1
     if(steps2>upper_limit_r) // limit the growth of 'steps2' variable when it goes beyond its upper limit
      steps2 = upper_limit_r;
      steps = 1; // reduce the value of 'steps' to 1 the moment power source starts operating in region V_(solar) < V_(mpp)
     pwm_value+=steps2;
     if(pwm_value<5) // place lower and upper bounds on the PWM values
      pwm_value = 5;
     if(pwm_value>250)
      pwm_value = 250;
     analogWrite(2,pwm_value); // output the PWM signal from pin 2
     //flag2 = 1;
     count_r++;  // increment count_l as long as power source/solar cell operates in this region
     count_l = 0; // reset count_l
   }
  else // region of operation where V_(solar) < V_(mpp)
   {
    if(start_incr_l == 1)
    steps = steps+2; // increment the value of 'steps' variable by 2 as long as start_incr_l is 1
    if(steps>upper_limit_l) // limit the growth of 'steps' variable when it goes beyond its upper limit
     steps = upper_limit_l;
      steps2 = 1; // reduce the value of 'steps2' to 1 the moment power source starts operating in region V_(solar) < V_(mpp)
    pwm_value-=steps;
    if(pwm_value<5) // place lower and upper bounds on the PWM values
     pwm_value = 5;
    if(pwm_value>250)
     pwm_value = 250;
    analogWrite(2,pwm_value); // output the PWM signal from pin 2
    count_r = 0; // reset count_r
    count_l++; // increment count_l as long as power source/solar cell operates in this region
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
