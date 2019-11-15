#include <math.h>
#include <stdio.h>
#define EPS 1e-3
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double abs(double x){
  return ( x > EPS)? x : -x;
}
double max_abs(double *vect ,int n){
  if (!n)
    return 0;
  double max = abs(vect[0]);
  printf("max %f\n", max);
  for (int i = 1; i < n; ++i){
    printf("max %f v_i %f\n", max,abs(vect[i]));
    if (max < abs(vect[i]))
      max = abs(vect[i]);
  }
  return max;
}

int sgn(double val) {
    return (0 < val) - (val < 0);
}
void setwheelspeed(int wheel_n, int speed){
  printf("[%d , %d]\n",wheel_n,speed);
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
  
  double v_max = max_abs(angular_v,wheel_count);
  printf("max %f\n", v_max);
  for(int i=0; i < wheel_count; i++){
    //angular_v[i] = 0;
    printf("i %d\nv_i %f \n",i,angular_v[i]);
    printf("x %d\nmax_x %f \n",abs((int)(angular_v[i]*precision)),precision*( (abs(v_max) > EPS)? v_max: 1));
    setwheelspeed(i,sgn(angular_v[i])*map(abs((int)(angular_v[i]*precision)),0,(int)(precision * ( (v_max > EPS)? v_max: 1)),0,250));
  }  
}

int main(int argc, char const *argv[])
{
  while(1){

  float vx,vy,angv;
  scanf("%f %f %f",&vx,&vy,&angv);
  move(vx,vy,angv);
  }

  return 0;
}