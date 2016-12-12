int current;
float voltage=0.0;
float prev_voltage=0.0;
float power=0.0;
float prev_power=0.0;
int pwm_value = 180;
int steps = 2;
int steps2 = 2;
int upper_limit_l = 8;
int upper_limit_r = 12;
int count_l = 0;
int count_r = 0;
int start_incr_l = 0;
int start_incr_r = 0;
long unsigned int time_stamp;

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
   sum+=analogRead(A4);
   value = sum/samples;
   new_value = value - 158;
   temp_current = new_value*1.272;
   if(temp_current<0)
   temp_current = 0;
   sum=0;
   suma+=temp_current;
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
  pinMode(A2,INPUT);
  pinMode(A4,INPUT);
  pinMode(2,OUTPUT);
  TCCR3B &= ~0b00000111;
  TCCR3B |= 0b00000001;
  Serial.begin(9600);
}

void loop() 
{  
 current = meas_current();
 voltage = meas_voltage();
 time_stamp = millis();
 power = current*voltage;
 
 if(count_l>2)
  {
    start_incr_l = 1;
    count_l = 2;
  }
 else
  start_incr_l = 0;
  
 if(count_r>2)
  {
    start_incr_r = 1;
    count_r = 2;
  }
 else
  start_incr_r = 0;
  
 if(power >= prev_power)
 {
  if(voltage > prev_voltage)
   {
     if(start_incr_l == 1)
     steps = steps*2;
     //steps2 = steps2/8;
     if(steps>upper_limit_l)
      steps = upper_limit_l;
     //if(steps2<1)
      steps2 = 1;
     pwm_value-=steps;
     if(pwm_value<5)
      pwm_value = 5;
     if(pwm_value>250)
      pwm_value = 250;
     analogWrite(2,pwm_value);
     count_l++;
     count_r = 0;
   }
  else 
   {
    if(start_incr_r == 1)
    steps2 = steps2+2;
    //steps = steps/8;
    if(steps2>upper_limit_r)
      steps2 = upper_limit_r;
    //if(steps<1)
      steps = 1;
    pwm_value+=steps2;
    if(pwm_value<5)
     pwm_value = 5;
    if(pwm_value>250)
     pwm_value = 250;
    analogWrite(2,pwm_value);
    //flag = 1;
    count_r++;
    count_l = 0;
   }
 }
else
 {
  if(voltage >= prev_voltage)
   {
     if(start_incr_r == 1)
     steps2 = steps2+2;
     //steps = steps/8;
     if(steps2>upper_limit_r)
      steps2 = upper_limit_r;
     //if(steps<1)
      steps = 1;
     pwm_value+=steps2;
     if(pwm_value<5)
      pwm_value = 5;
     if(pwm_value>250)
      pwm_value = 250;
     analogWrite(2,pwm_value);
     //flag2 = 1;
     count_r++;
     count_l = 0;
   }
  else
   {
    if(start_incr_l == 1)
    steps = steps*2;
    //steps2 = steps2/8;
    if(steps>upper_limit_l)
     steps = upper_limit_l;
    //if(steps2<1)
      steps2 = 1;
    pwm_value-=steps;
    if(pwm_value<5)
     pwm_value = 5;
    if(pwm_value>250)
     pwm_value = 250;
    analogWrite(2,pwm_value);
    count_r = 0;
    count_l++;
   }
 } 
 prev_power = power;
 prev_voltage = voltage;
 Serial.print(time_stamp);
 Serial.print(" ");
 Serial.print(voltage);
 Serial.print(" ");
 Serial.print(current);
 Serial.print(" ");
 Serial.println(pwm_value);
}
