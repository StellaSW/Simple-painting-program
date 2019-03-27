#ifndef POINT_H
#define POINT_H

#include <iostream>
#include <string>
using namespace std;

class Point
{
public:
  Point();
  Point(int, int, float, float, float);
  ~Point();

  int getX();
  int getY();
  float getR();
  float getG();
  float getB();

  void setPosition(int, int);
  void setColour(float, float, float);

private:
  int newX;
  int newY;
  float newR;
  float newG;
  float newB;
};

#endif