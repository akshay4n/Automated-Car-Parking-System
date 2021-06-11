#include<Servo.h>
#include<LiquidCrystal.h>

int Contrast = 100;
Servo servo;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int trigpin = 4;
int echopin[] = {2, 3};
int trig = 4;
int echo = 5;

//slot stores information about the parking slots
struct slot
{
  int index_no; //index number starts from 1
  float distance; //distance from gate to the parking slot
  int occu;  //0=unoccupied ; 1=occupied
}slot_array[2];

//car_deteciton detects if the object is a vehicle
bool car_detection()
{
  return true;
}

//calc_distance calculates distance with the help of ultrasonic sensors,
//and saves it to slot
void calc_distance(int a, int pos)
{ 
 float duration=pulseIn(a,HIGH);
 float distance_cm=duration*0.034/2;

 lcd.clear();
 lcd.print("Slot dist:");
 lcd.setCursor(11,0);
 lcd.print(pos+1);
 lcd.setCursor(0,1);
 lcd.print(distance_cm);
 delay(1000);
 
  if(distance_cm <= 10)
  {
    slot_array[pos].index_no=pos+1;
    slot_array[pos].occu = 1;
  } 
  else 
  {
    slot_array[pos].index_no=pos+1;
    slot_array[pos].occu = 0;
  }
}

//empty_slot_func helps calc_distance function by initializing ultrasonic sensor
void empty_slot_func()
{ 
  for(int i=0;i<2;i++)
  {
    digitalWrite(trigpin,LOW);
    delay(2);
    digitalWrite(trigpin,HIGH);
    delay (10);
    digitalWrite(trigpin,LOW);
    //digitalRead(echopin[i],INPUT);
    calc_distance(echopin[i],i);
  }
  
}

//shortest_dist_slot obtains the minimum possible distanced empty parking slot
int shortest_dist_slot()
{
  int i=0,index_min;
  int min_ = 100;
  
  for(i=0;i<2;i++)
  {
    if(slot_array[i].occu == 0)
    {
      if(slot_array[i].distance < min_)
      {
        min_ = slot_array[i].distance;
        index_min = i+1;
      }
    }
  }
  return index_min;
}

//parking_lot_status determines whether there is an empty parking slot or not
bool parking_lot_status()
{
  bool b = false;

  for(int i=0;i<2;i++)
  {
      //lcd.clear();
      //lcd.print(slot_array[i].occu);
      //delay(1000);
    if(slot_array[i].occu == 0)
    {
      b = true;
      
    }
  }

  return b;
  //return true;
  
}

//calculates distance of object from gate
int dist()
{
  int duration1,distance1;
  digitalWrite(trig,HIGH);
  delay(10);
  digitalWrite(trig,LOW);
  duration1 = pulseIn(echo,HIGH);
  distance1 = (duration1/2) / 29.1;
  lcd.clear();
  lcd.print("Distance1:");
  lcd.setCursor(0,1);
  lcd.print(distance1);
  delay(1000);
  return distance1;
  //return 0;
}

//opens and closes gate
void gate_control()
{
  if (dist()<10)
  {
    //open gate
    lcd.clear();
    lcd.print("Gate opening...");
    for (int i=0;i<=90;i++)
    {
      servo.write(i);
      delay(10);
    }
    delay(1000);
    lcd.clear();
    lcd.print("Gate about to");
    lcd.setCursor(0,1);
    lcd.print("close!");
    delay(1000);//car should get in within this interval
    //goto here;
  }
 // here:
  if(dist()>10)
  {
    //close gate
    lcd.clear();
    lcd.print("Gate closing...");
    lcd.clear();
    lcd.print(servo.read());
    if(servo.read() > 45)
    {
      for (int i=90;i>=0;i--)
      {
          servo.write(i);
          delay(10);
      }
    }
    
    delay(100);
  }
  
  
  //the gate_control uses ultrasonic sensor to detect an object (preferably a car) at the gate
}

//lcd_display shows the parking slot where the car is to be parked
void lcd_display(int min_index)
{
    lcd.clear();
    lcd.print("Parking slot");
    lcd.setCursor(0,1);
    lcd.print("Number");
    lcd.setCursor(8,1);
    lcd.print(min_index);
    delay(3000);
    lcd.clear();
}

void setup() 
{
  // put your setup code here, to run once:
  pinMode(trigpin,OUTPUT);
  
  for(int i=0;i<2;i++)
  {
    pinMode(echopin[i],INPUT);
  }

//declare distance to the parking slots from the main gate, 1, 2 units respectively, which can be changed
  for(int i=0;i<2;i++)
  {
    slot_array[i].distance = i+1;
  }

  Serial.begin(9600);
  servo.attach(13);
  analogWrite(6,Contrast);
  lcd.begin(16,2);

}

void loop() {
  // put your main code here, to run repeatedly:
  bool if_car = car_detection(); //returns true always for now
  bool park_free = parking_lot_status();
  int min_dist_index;

  //close gate initially;

  if(servo.read() > 5)
  {
    //close gate
    lcd.clear();
    lcd.print("Gate closing...");
    lcd.clear();
    lcd.print(servo.read());
    if(servo.read() > 45)
    {
      for (int i=90;i>=0;i--)
      {
          servo.write(i);
          delay(10);
      }
    }
    
    delay(100);
  }
  if(if_car == true && park_free == true)
  {
    //LCD Displays parking space is available!!
    lcd.clear();
    lcd.print("Parking space");
    lcd.setCursor(1,1);
    lcd.print("available!");
    delay(3000);
    //lcd.clear();

    if(dist() < 10)
    {
      empty_slot_func();
      min_dist_index = shortest_dist_slot();
    
      lcd_display(min_dist_index);

      //servo motor,i.e. gate opens
      gate_control();
    
      //led_path() will guide the vehicle to its path
    }
    
    
  }

  else
  {
    //LCD Displays no parking space available
    lcd.setCursor(0,0);
    lcd.print("No Parking Space");
    lcd.setCursor(1,1);
    lcd.print("Available!");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Check Elsewhere!");
    lcd.setCursor(1,1);
    lcd.print("Thank You!");
    delay(3000);
  }
  
}
