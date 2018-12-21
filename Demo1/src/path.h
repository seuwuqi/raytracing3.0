#ifndef PATH_H
#define PATH_H

#include <vector>
#include "node.h"
#include <list>
#include <iostream>
#include <QDebug>
#include <math.h>
#include <QtMath>
#include "global.h"
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
private:
    const double permittivity = 5.1;
    const double C = 300000000;
    const double TxHeight = 2.2;
    const double RxHeight = 2.2;

public:

    Path(vector<Node*> nodes){
        this->nodeSet = nodes;
    }
    Path(){

    }

    ~Path()
    {

    }


    int getReflectCount(){
        int count = 0;
        for(int i = 0; i < nodeSet.size(); i++){
            if(nodeSet[i]->type ==NodeType::reflect){
                count++;
            }
        }
        return count;
    }


    int getDiffractCount(){
        int count = 0;
        for(int i = 0; i < nodeSet.size(); i++){
            if(nodeSet[i]->type ==NodeType::diff){
                count++;
            }
        }
        return count;
    }

    double freeSpace(double lambada, double d){

        return  (lambada/ (4*M_PI*d));

    }

    double R(double theta,double epsilon){

        return (sin(theta) - sqrt(epsilon - pow(cos(theta),2)))
                / (sin(theta) + sqrt(epsilon - pow(cos(theta),2)));
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


    double Electric(){
        double gain = channelGain(0);


    }

    double getAOA(){
        return nodeSet[nodeSet.size() - 2]->angle;
    }

    double getDOA(){
        return nodeSet.front()->angle;
    }


    void unitfy(double* a,double speed){
        double M = sqrt(pow(a[0],2)+ pow(a[1],2));
        a[0] = a[0] / M * speed;
        a[1] = a[1] / M * speed;
    }
    double getDopplerShift(){
        double AOA = getAOA();
//        qDebug() << "speed "<<txData->speed;
//        qDebug() << "speed2 "<<rxData->speed;

//        qDebug() << "txData" << txData->x2<< ","<<txData->x<<","<<txData->y2<< "," <<txData->y;
//        qDebug() << "rxData" << rxData->x2<< ","<<rxData->x<<","<<rxData->y2<< "," <<rxData->y;

        double TxDirection[2]{txData->x2 - txData->x,txData->y2 - txData->y};
        unitfy(TxDirection,txData->speed);
        double RxDirection[2]{rxData->x2 - rxData->x,rxData->y2 - rxData->y};
        unitfy(RxDirection,rxData->speed);
        double relativeSpeed[2] = {TxDirection[0] -RxDirection[0],
                                      TxDirection[1] -RxDirection[1]};
//        qDebug()<<TxDirection[0] <<","<< TxDirection[1];
//        qDebug()<<RxDirection[0] <<","<< RxDirection[1];
        double v = sqrt(pow(relativeSpeed[0],2)+(pow(relativeSpeed[1],2)));
//        qDebug() << "max v:" << v;
        if(path_type == PathType::NLOSb){
            v *= fabs(sin(qDegreesToRadians(AOA)));
        }
//        qDebug() << "v:" << v;
        return v /3.6 * 58.9/ 3;

    }
    double timeDelay(){
        double distance = 0;
        Node* prev = nullptr;
        Node* curr = nullptr;
        for(int i = 0; i < nodeSet.size(); i++){
            curr = nodeSet[i];
            if(prev != nullptr){
                double curr_distance = distanceOf(prev->x, prev->y, curr->x, curr->y);
                distance = distance + curr_distance;
            }
            prev = curr;
        }
//         qDebug() << "total distance:" << distance;
        double timeDelay = distance/(C/1000000);
//        qDebug() <<"timeDelay" << timeDelay;
        return timeDelay;
    }

    double channelGain(int startNode){
        if(path_type == PathType::LOSg){

        }else if(path_type == PathType::NLOSv){
            Node* tx = nodeSet[0];
            Node* d1 = nodeSet[1];
            Node* d2 = nodeSet[2];
            Node* rx = nodeSet[3];
            double distance1 = distanceOf(tx->x,tx->y,d1->x,d1->y);
            double distance2 = distanceOf(d1->x,d1->y,d2->x,d2->y);
            double distance3 = distanceOf(d2->x,d2->y,rx->x,rx->y);
            double freeSpaceLoss= freeSpace(3.0/58.9, distance1 + distance2 + distance3);
            double alpha;
            if(d1->type == NodeType::diff2){
                 alpha = 2*atan(10/(distance1 + 5));
            }else{
                 alpha = 2*atan(15/(distance1 + 5));
            }
            alpha = qDegreesToRadians(alpha);
            qDebug() << "alpha" << alpha;
            double v = alpha * sqrt( (2 * distance1 * distance3) / (0.051* (distance1 + distance2) ));
            qDebug() << "v" << v;
            double gain = 0.5 * exp(-0.95 * v);
            qDebug() << "gain" << gain;
            gain *= freeSpaceLoss;
            return gain;
        }
        double gain = 1;
        double distance = 0;
        Node* prev = nullptr;
        Node* curr = nullptr;
        for(int i = startNode; i < nodeSet.size(); i++){
            curr = nodeSet[i];
            if(prev != nullptr){
//                qDebug() << prev->x<< prev->y<< curr->x << curr->y;
                double curr_distance = distanceOf(prev->x, prev->y, curr->x, curr->y);
                distance = distance + curr_distance;
//                qDebug() << "distace += getdistance" << curr_distance;
//                qDebug() << "distance:" << distance;
                if(curr->type == NodeType::reflect){
//                    qDebug() << "reflect";
//                    qDebug() << prev->angle;
//                    qDebug() << curr->angle;
                    double tmp = fabs( (curr->angle - prev->angle + 360));
                    while(tmp > 90){
                        tmp -= 90;
                    }
                    double angle = qDegreesToRadians( tmp );
//                    qDebug() << angle ;
                    gain *= R(angle,permittivity);
//                    qDebug() <<  R(angle,permittivity);
                }else if(curr->type == NodeType::diff){
//                    qDebug() << "diff";
                    double diffLos = channelGain(i);
//                    qDebug() << "diffLos:" << diffLos;
                    gain *= diffLos;
                    break;
                }
            }
            prev = curr;
        }
//        qDebug() << "distance:" << distance;
        double freeSpaceLoss= freeSpace(3.0/58.9, distance);
//        qDebug() << "freeSpaceLoss" << freeSpaceLoss;
        gain *= freeSpaceLoss;
//        qDebug() << "gain" << gain;
        return fabs(gain);
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
