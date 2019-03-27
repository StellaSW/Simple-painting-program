#include "Point.h"


Point::Point(){

}

Point::Point(int x, int y, float r, float g, float b){
    newX = x;
    newY = y;
    newR = r;
    newG = g;
    newB = b;
}

Point::~Point(){

}
int Point::getX(){
    return newX;
}

int Point::getY(){
    return newY;
}

float Point::getR(){
    return newR;
}

float Point::getG(){
    return newG;
}

float Point::getB(){
    return newB;
}

void Point::setPosition(int x, int y){
    newX = x;
    newY = y;
}
void Point::setColour(float r, float g, float b){
    newR = r;
    newG = g;
    newB = b;
}
