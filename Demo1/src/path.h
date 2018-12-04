#ifndef PATH_H
#define PATH_H

#include <vector>
#include "node.h"
#include <list>
#include <iostream>
#include <QDebug>
#include <math.h>
#include <QtMath>
using namespace std;
//路径类，用于表示路劲，计算路劲损失和延时扩展
class Path
{
public:

    vector<Node*> nodeSet;
    list<Node*> nodes;
    double pathLoss;//路径损失
    double delay;//延时
    PathType path_type;
public:

    Path(vector<Node*> nodes){
        this->nodeSet = nodes;
    }
    Path(){

    }

    ~Path()
    {

    }



    double freeSpace(double lambada, double d){

        return  20*log10(lambada/ (4*M_PI*d));
    }

    double reflect(double theta, double epsilon){
        return sin(theta) - sqrt(epsilon - pow(cos(theta),2)/epsilon)
                /sin(theta) + sqrt(epsilon - pow(cos(theta),2)/epsilon);
    }

    double diffract(double phi1, double phi2, double k){
        double theta = M_PI - fabs(phi2 - phi1);
        double D = -1/sqrt(2 * M_PI * k)*(1/theta + 1/ (2*M_PI) - theta);
        return D;
    }



    double distanceOf(double lng1, double lat1, double lng2, double lat2){
        double radLat1 = qDegreesToRadians(lat1);
        double radLat2 = qDegreesToRadians(lat2);
        double a = radLat1 - radLat2;
        double b = qDegreesToRadians(lng1) - qDegreesToRadians(lng2);
        double s = 2 * asin(sqrt(pow(sin(a/2),2) + cos(radLat1) * cos(radLat2)
                                 * pow(sin(b/2),2)));
        s = s * 6378.137;
        s = round(s * 100000) / 100000;
        return s * 1000;
    }

    double getDistance(double L,double B, double L1, double B1 ){
        double a = 6378137.0;//地球长半径
        double f = 0.003352810664747;//地球扁率
        //转换成弧度
        B =  qDegreesToRadians(B);
        L =  qDegreesToRadians(L);
        B1 = qDegreesToRadians(B1);
        L1 = qDegreesToRadians(L1);
        double N = a/sqrt(1 - (2*f - f*f)*sin(B));
        double N1 = a/sqrt(1 - (2*f - f*f)*sin(B1));
        double x = N * cos(B)*cos(L);
        double y = N * cos(B) * sin(L);
        double x1 = N1 * cos(B1)*cos(L1);
        double y1 = N1 * cos(B1) * sin(L1);
        return sqrt( pow((x - x1),2)+pow((y - y1),2));
    }

    double channelGain(int startNode){
        double gain = 0;
        double distance = 0;
        Node* prev = nullptr;
        Node* curr = nullptr;
        for(int i = startNode; i < nodeSet.size(); i++){
            curr = nodeSet[i];
            if(prev != nullptr){
                qDebug() << prev->x<< prev->y<< curr->x << curr->y;
                double curr_distance = distanceOf(prev->x, prev->y, curr->x, curr->y);
                distance = distance + curr_distance;
                qDebug() << "distace += getdistance" << curr_distance;
                qDebug() << "distance:" << distance;
                if(curr->type == NodeType::reflect){
                    qDebug() << "reflect";
//                    gain *= reflect();
                }else if(curr->type == NodeType::diff){
                    qDebug() << "diff";
                    gain += channelGain(i);
                    break;
                }

            }
            prev = curr;
        }
        qDebug() << "seg distance:" << distance;

        double segGain= freeSpace(3.0/58.9, distance);
        qDebug() << "segGain" << segGain;
        gain += segGain;
        return gain;
    }
    //凸包检测
    vector<Node*> convexhull(Node* p1, Node* p2){

    }
    //
    double HorizontalDiff(vector<Node*> reflects){

    }

    void pathProcess(list<Node*> path,double mesh_size,PathType type){

    }
};

#endif // PATH_H
