#include <math.h>
#include "vector.h"
#include <stdlib.h>
#define lIN1 12
#define lIN2 13
#define lIN3 10
#define lIN4 11

#define rIN1 4
#define rIN2 5
#define rIN3 6
#define rIN4 7

#define EPS 1e-3
#define PI 3.14159265359
#define DELTA 0.78539816339
#define ALPHA 0.916146


class Movements{
static const int wheel_count = 4;
const int precision = 1000;
double rot_angle = 0.; // rad
double alpha[wheel_count] = { ALPHA, 2 * PI - ALPHA, PI - ALPHA, PI + ALPHA };  // rad
double delta[wheel_count] = { DELTA, -DELTA, -DELTA, DELTA };  // rad
double r_rot_point[wheel_count] = { 11.607, 11.607, 11.607, 11.607 };  // cm
double wheel_r = 0.24;                   // cm 
double angular_v[wheel_count];    // rad/sec
double v_angl_previous = 0.;            // rad/sec
double vx_previous;               // cm/sec
double vy_previous;               // cm/sec
long t_previous_call;       // sec * 10^6
long t_ang_v_comp = 0;      // sec * 10^6

void add_rot_angl(double angl);
public: 
  Movements(){
    t_previous_call = micros();
  }
  
  boolean is_rotating(){
    return abs(v_angl_previous) > EPS;
  }
  void reset(){
    rot_angle = 0;
    v_angl_previous = 0;
  }
    
  void move(double vx,double vy,double v_angl);
  void move();  
};


void setup() {
  pinMode(lIN1, OUTPUT);
  pinMode(lIN2, OUTPUT);
  pinMode(lIN3, OUTPUT);
  pinMode(lIN4, OUTPUT);
  pinMode(rIN1, OUTPUT);
  pinMode(rIN2, OUTPUT);
  pinMode(rIN3, OUTPUT);
  pinMode(rIN4, OUTPUT);
  Serial.begin(9600);
  
}


int sgn(double val) {
    return (0 < val) - (val < 0);
}

double abs_d(double x){
  return ( x > EPS)? x : -x;
}

double max_abs(double *vect ,int n){
  if (!n)
    return 0;
  double max = abs_d(vect[0]);
  for (int i = 1; i < n; ++i)
    if (max < abs_d(vect[i]))
      max = abs_d(vect[i]);
  return max;
}

void setwheelspeed(int wheel, int speed)
{
  switch (wheel)
  {
  case 0:
    analogWrite(rIN1,(speed > 0)?speed:0);
    analogWrite(rIN2,(speed >= 0)?0:abs(speed));
    break;
  case 1:
    analogWrite(rIN4,(speed > 0)?speed:0);
    analogWrite(rIN3,(speed >= 0)?0:abs(speed));
    break;
  case 2:
    analogWrite(lIN1,(speed > 0)?speed:0);
    analogWrite(lIN2,(speed >= 0)?0:abs(speed));
    break;
  case 3:
    analogWrite(lIN4,(speed > 0)?speed:0);
    analogWrite(lIN3,(speed >= 0)?0:abs(speed));
    break;
  }
}

void Movements::add_rot_angl(double angl){
  rot_angle += angl;
  while (  abs_d(rot_angle) > 2 * PI)
     rot_angle -= sgn(rot_angle)*2*PI; 
}

void Movements::move(){
 move(vx_previous,vy_previous,v_angl_previous); 
}

void Movements::move(double vx,double vy,double v_angl){
  // actual_v = 1.0875
  vx_previous = vx;
  vy_previous = vy; 
  add_rot_angl(v_angl_previous * (t_ang_v_comp + micros()-t_previous_call) * 1e-6); // plus ang_v computing time
  
  for(int i = 0; i < wheel_count; i++)
     angular_v[i] = (vx * cos(rot_angle + delta[i]) + vy*sin(rot_angle + delta[i]) + r_rot_point[i]*v_angl*sin(alpha[i] - delta[i])) / (double)(wheel_r * cos(delta[i])); 
  //angular_v[1] = (vx * cos(rot_angle - delta) + vy*sin(rot_angle - delta) ) / (double)(wheel_r * cos(delta)); 
  //angular_v[0] = (vx * cos(rot_angle + delta) + vy*sin(rot_angle + delta) ) / (double)(wheel_r * cos(delta)); 
  //angular_v[3] = (vx * cos(rot_angle + delta) + vy*sin(rot_angle + delta) ) / (double)(wheel_r * cos(delta)); 
  //angular_v[2] = (vx * cos(rot_angle - delta) + vy*sin(rot_angle - delta) ) / (double)(wheel_r * cos(delta)); 
    
  v_angl_previous = v_angl;
  t_previous_call = micros();  
  Serial.println("fi");
  Serial.println(rot_angle);
  double v_max = max_abs(angular_v,wheel_count);
  for(int i=0; i < wheel_count; i++){
    Serial.print("Angular v");
    Serial.println( String(i));
    Serial.println(angular_v[i],4);
    Serial.println();
    
    //int v = abs(vx) + abs(vy) + abs(r_rot_point_max*v_angl);
    setwheelspeed(i,sgn(angular_v[i])*map(abs((int)(angular_v[i]*precision)),0,(int)(precision * ( (v_max > EPS)? v_max: 1)),0,250));
  }
  
  
    
  
}
void start_moving(Vector vec)
{
  setwheelspeed(0, vec.countpwr_x());
  setwheelspeed(1, vec.countpwr_y());
  setwheelspeed(2, vec.countpwr_y());
  setwheelspeed(3, vec.countpwr_x());
}

void rotate(bool dir)
{
    setwheelspeed(0,200 * (dir?1:-1));
    setwheelspeed(1,200 * (!dir?1:-1));
    setwheelspeed(2,200 * (dir?1:-1));
    setwheelspeed(3,200 * (!dir?1:-1));
}

void parse(String command, String x[]){
  const int count = 3;
  int i,j;
  for (i=0; i < count; i++)
    x[i] = "";
  for(j = 0,i = 2; j < count - 1; i++, j++)
    for(;command[i] != ' ';i++)x[j] += command[i];
  for(;i < command.length();i++)x[count - 1] += command[i];
}

Vector vec;
String command, x[2];
Movements car;
int i;
void loop() {
  if (Serial.available())
  {
    command = Serial.readStringUntil('\n');
     if(command[0] == 'L')
    {
      parse(command,x);
      vec.set_coordinates(atof(x[0].c_str()),atof(x[1].c_str()));
      start_moving(vec);
    }
    else if (command[0] == 'R')
    {
      rotate(String(command[2]).toInt());
    }
   
   else if (command[0] == 'C')
    {
     parse(command,x);
     Serial.println();
     for(int i = 0; i < 3; i++){
        Serial.print("X" + String(i) + " ");
        Serial.println(x[i]);        
     }
     Serial.println();
     
     car.move(atof(x[0].c_str()),atof(x[1].c_str()),atof(x[2].c_str()));
    }
    else if (command[0] == 'r'){
     car.reset(); 
    }
    else if (command[0] == 'T'){
      for(int i = 0; i < 4; i++){
        setwheelspeed(i,250);
        delay(100);
        setwheelspeed(i,0);
        delay(100);
    }
    delay(300);
    }  
}
  else if (car.is_rotating()){
     car.move(); 
   }

}
