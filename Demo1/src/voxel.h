#ifndef VOXEL_H
#define VOXEL_H
#include <stdio.h>
#include <vector>
#include "edge.h"
#include "Point.h"
#include <math.h>
#include "node.h"
#include <iostream>
using namespace std;
/************************************************************************/
/* Voxel类
   存放edge类型的数组
   相交检测
   */
/************************************************************************/
class Voxel
{
public:
    int i = 0;
    int j = 0;
    vector<Edge*> edgeList;
    bool containRx = false;
public:
    Voxel(){

    }
    ~Voxel(){

    }
    //遍历voxel内部所有的边，判断有没有相交，若有返回相交的新node，否则返回空
    Node* intersectTest(Node* source){
        if (edgeList.size()== 0){//如果该voxel没有建筑墙面，返回空指针
                return nullptr;
            }
            const double PI = 3.1415926;
            double s_x = source->x;//起点
            double s_y = source->y;
            double angle = source->angle;
            double phi = angle * PI / 180;//弧度角
            double k = 0.0;
            //计算射线斜率
            if (angle == 90 || angle == 270){
                k = 0.0;
            }
            else{
                k = tan(phi);
            }
            double b = (s_y)-k * s_x;
            double D = 0;
            double currDistance = 100000.0;
            double reflectAngle = 0;//反射射线的角度
            Node * intersetPoint = nullptr;
            vector<Edge*> vp = edgeList;

            for (int i = 0; i < vp.size(); i++){
                Edge* edge = vp[i];
                double x1 = edge->startPoint->x;
            //	cout << "x1:" << x1 << endl;
                double y1 = edge->startPoint->y;
            //	cout << "y1:" << y1 << endl;
                double x2 = edge->endPoint->x;
            //	cout << "x2:" << x2 << endl;
                double y2 = edge->endPoint->y;
            //	cout << "y2:" << y2 << endl;
                double k2 = edge->k;
            //	cout << "k2:" << k2 << endl;
                double b2 = edge->b;
            //	cout << "b2:" << b2 << endl;
            //	cout <<"k:" <<k <<"b:"<< b << endl;
                double x0, y0;
                if (angle == 90 || angle == 270){
                    if (edge->vertical){
        //				cout << "没有交点" << endl;
                        continue;
                    }
                    else{
                        x0 = s_x;
                        y0 = k2*x0 + b2;
                    }
                }
                else{
                    if (edge->vertical){
            //			cout << "遇到垂直边@@" << endl;
                        x0 = x1;
                        y0 = k*x0 + b;
                    }
                    else{
                        x0 = -((b - b2) / (k - k2));
                        y0 = (k*b2 - k2 * b) / (k - k2);
                    }
                }
            //	cout << "x0:" << x0 << endl;
            //	cout << "y0:" << y0 << endl;
        //		cout << "angle:" << angle << endl;
                if (x0 > x1 && x0 > x2){
        //			cout << "point do not exist in this edge" << endl;
                    continue;
                }
                if (x0 < x1 && x0 < x2){
        //			cout << "point do not exist in this edge" << endl;
                    continue;
                }

                if (y0 < y1 && y0 < y2){
        //         	cout << "point do not exist in this edge" << endl;
                    continue;
                }
                if (y0 > y1 && y0 > y2){
        //			cout << "point do not exist in this edge" << endl;
                    continue;
                }


                if (fabs(x0 - s_x) < 0.001 && !(angle==90 || angle == 270)){
        //			cout << "ERROR!!" << endl;
                    continue;
                }
                if (fabs(y0 - s_y) < 0.001 && !(angle == 0 || angle == 360 || angle == 180)){
        //			cout << "ERROR!!" << endl;
                    continue;
                }

                if ((angle == 0 || angle == 360)&&x0 <= s_x){
                    continue;
                }
                if (angle == 180 && x0 >= s_x){
                    continue;
                }
                if (0 < angle && angle < 180 && y0 <= s_y){
            //		cout << "error,next!" << endl;
                    continue;
                }
                if (180 < angle && angle < 360 && y0 >= s_y){
            //		cout << "error,next!" << endl;
                    continue;
                }

                if (x0 < i || x0 > i + 1 || y0 < j || y0 > j + 1){
            //		cout << "交点不在这个cell中" << endl;
                    continue;
                }

                D = sqrt((x0 - s_x)*(x0 - s_x) + (y0 - s_y)*(y0 - s_y));
            //	cout << "distance:" << D << endl;
                if (D > currDistance){
                    continue;
                }
                else
                {
                    currDistance = D;
                }

                //求镜像点
                double x_s, y_s;
                if (edge->vertical){
                    x_s = 2 * x0 - s_x;
                    y_s = s_y;
                }
                else{
                    double med = 2 * ((s_y - k2*s_x - b2) / (k2*k2 + 1));
                    x_s = s_x + k2*med;
                    y_s = s_y - med;
                }
            //	cout << "xs,ys" << x_s << "," << y_s << endl;
                double fabsAngle = fabs(atan((y0 - y_s) / (x0 - x_s)));
                //求新的角度
                fabsAngle = fabsAngle * 180 / PI;
            //	cout << "fabsAngle :" << fabsAngle << endl;
                if (y0 > y_s && x0 > x_s){
                    reflectAngle = fabsAngle;
                }
                else if (y0 > y_s && x0 < x_s){
                    reflectAngle = 180.0 - fabsAngle;
                }
                else if (y0 < y_s && x0 >x_s){
                    reflectAngle = 360 - fabsAngle;
                }
                else if (y0 < y_s && x0 <x_s){
                    reflectAngle = 180 + fabsAngle;
                }
                else if (y0 == y_s && x0 > x_s){
                    reflectAngle = 0;
                }
                else if (y0 == y_s && x0 < x_s){
                    reflectAngle = 180;
                }
                else if (y0 > y_s && x0 == x_s){
                    reflectAngle = 90;
                }
                else{
                    reflectAngle = 270;
                }
            //	cout << "reflectAngle:" << reflectAngle << endl;
            //	cout << "retain" << endl;

                intersetPoint = new Node(x0,y0,reflectAngle);
            }
            //for循环结束
            return intersetPoint;
    }
    Node* intersectTest2(Node* source){

    }
};

#endif // VOXEL_H
