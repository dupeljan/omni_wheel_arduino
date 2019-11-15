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
#define WHEEL_COUNT 4


class Movements{
int wheel_count = WHEEL_COUNT;
double rot_angle; // rad
double alpha[WHEEL_COUNT];  // rad
double delta[WHEEL_COUNT];  // rad
double r_rot_point[WHEEL_COUNT];  // cm
double wheel_r;                   // cm 
long t_previous_call;       // sec * 10^6
public: 
  Movements(){
    rot_angle = 0.;
    t_previous_call = micros();
  }
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


void move(double vx, double vy, double r){
  const int wheel_count = 4;
  const double R = 1; // wheel radius
  const double rot_angle = 0;
  const double delta = 0.78539816339; // pi/4
  double angular_v[wheel_count];
  const int precision = 1000;
  
  angular_v[1] = (vx * cos(rot_angle - delta) + vy*sin(rot_angle - delta) ) / (double)(R * cos(delta)); 
  angular_v[0] = (vx * cos(rot_angle + delta) + vy*sin(rot_angle + delta) ) / (double)(R * cos(delta)); 
  angular_v[3] = (vx * cos(rot_angle + delta) + vy*sin(rot_angle + delta) ) / (double)(R * cos(delta)); 
  angular_v[2] = (vx * cos(rot_angle - delta) + vy*sin(rot_angle - delta) ) / (double)(R * cos(delta)); 
    
    
    Serial.println(vx);
    Serial.println(vy);
  double v_max = max_abs(angular_v,wheel_count);
  for(int i=0; i < wheel_count; i++){
    Serial.println( String(i));
    Serial.println(angular_v[i],4);
    Serial.println();
    int v = abs(vx) + abs(vy);
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
  int i;
  x[0]=x[1]="";
  for(i = 2;command[i] != ' ';i++)x[0] += command[i];
  for(;i < command.length();i++)x[1] += command[i];
}

Vector vec;
String command, x[2];
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
     Serial.println(x[0]);
     Serial.println(x[1]);
     Serial.println();
     
     move(atof(x[0].c_str()),atof(x[1].c_str()),0);
    }
    else if (command[0] == 'T'){
        setwheelspeed(0,255);
        delay(1000);
        setwheelspeed(0,0);
        delay(1000);
        setwheelspeed(0,-255);
        delay(1000);
        setwheelspeed(0,0);
    }  
  }
}
