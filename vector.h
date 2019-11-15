 
#ifndef VECTOR
#define VECTOR
struct Vector
{
  int x,y;
  Vector(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}

  void set_coordinates(int x_ = 0, int y_ = 0){ 
    x = x_; y = y_; 
    }
  
  float countangle() {
    return atan(y/x); 
  }
  int countlength() {
    return sqrt( x * x + y * y);
  }
  int countpwr_x() // Вектора правой направленности
  {
    return 255 * (-x - y);
  }
  int countpwr_y() // Вектора левой направленности
  {
    return 255 * (x - y);
  }
};

#endif