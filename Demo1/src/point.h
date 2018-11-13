#pragma once


class Point
{
public:
	double x = 0;
	double y = 0;
	double z = 0;
public:
    Point(){}
    Point(double x,double y,double z = 0.0){
        this->x = x;
        this->y = y;
        this->z = z;
    }
    Point(Point* p){
        x=p->x;
        y=p->y;
        z=p->z;
    }
    ~Point(){

    }
};
