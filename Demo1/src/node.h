#ifndef NODE_H
#define NODE_H
#include "point.h"
#include <math.h>
#include "type.h"

class Node:public Point
{
public:
    double angle;//发射角度
    double A, B, C;//射线参数
    bool isRx;
    NodeType type = none;
    Node* parent = nullptr;
    Node* leftChild = nullptr;
    Node* rightChild = nullptr;
    double range[2];
    double vSpread[2];
    double distane = 0;//和源节点的水平距离
    bool newSrc = false;
    bool isReceived = false;
    double P;
public:
    Node(){

    }
    Node(double x, double y, double angle = 0,bool rx = false) :Point(x,y)
    {
        this->angle = angle;
        isRx = rx;
        if (angle == 90 || angle == 270)
        {
            B = 0, A = 1, C = -x;
        }
        else{
            B = 1;
            A = -tan(angle*3.1415926/180);
            C = -(y + A * x);
        }
        range[0] = range[1] = vSpread[0] = vSpread[1] = 0.0;
    }

    ~Node()
    {
    }
};
#endif // NODE_H
