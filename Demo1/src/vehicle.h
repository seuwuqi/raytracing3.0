#ifndef VEHICLE_H
#define VEHICLE_H
#include"point.h"
#include"node.h"
#include"QString"
class Vehicle:public Node
{
public:
    double width;
    double length;
    double hight;
    double speed;
    double x2;
    double y2;
    double lng;
    double lat;
    QString type;
    bool dynamic;
    double distance;
    Vehicle(){

    }

    ~Vehicle()
    {

    }
};


#endif // VEHICLE_H
