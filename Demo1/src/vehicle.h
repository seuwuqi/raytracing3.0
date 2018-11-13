#ifndef VEHICLE_H
#define VEHICLE_H
#include"point.h"
class vehicle
{
public:
    double width = 0.1;
    double length = 0.2;
    double hight = 0.1;
    double velocity = 10;
    Point position;
    vehicle(){

    }
    vehicle(Point p){
        position = p;
    }
    vehicle(double width,double length,double hight,double velocity)
    {
        this->width = width;
        this->length = length;
        this->hight = hight;
        this->velocity = velocity;
    }

    ~vehicle()
    {
    }
};


#endif // VEHICLE_H
