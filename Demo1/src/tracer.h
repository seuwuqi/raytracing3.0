#ifndef TRACER_H
#define TRACER_H
/************************************************************************/
/* 追踪器类
功能：追踪对应source和网格的所有点*/
/************************************************************************/
#include <stdio.h>
#include "node.h"
#include "point.h"
#include "mesh.h"
#include "path.h"
#include "QDebug"
class Tracer
{
public:
    Node* tx;
    Mesh* mesh;//网格
    Node* p;//节点指针
    int raySpacing = 1;//射线间隔
    const int maxReflectNum = 3;
    const int maxdiffractNum = 1;
    int reflectNum = 0;
    int diffractNum = 0;
    vector<Path*> allPath;
    vector<Node*> nodeList;
    double distanceBetweenTxRx;
private:


public:
    Tracer(){

    }
    Tracer(Mesh* mesh){
        this->mesh = mesh;
    }
    Tracer(Mesh*mesh, Node* Tx){
        this->mesh = mesh;
        this->tx = Tx;
        distanceBetweenTxRx = distance(Tx->x, Tx->y, mesh->Rx->x, mesh->Rx->y);
        qDebug() << "distanceBetweenTxRx" << distanceBetweenTxRx;
    }
    ~Tracer(){

    }

    double distance(double x1,double y1, double x2, double y2){
        return sqrt( pow(x1 - x2, 2) + pow (y1 - y2, 2));
    }
    void traceOneRay(Node* source){
        const double PI = 3.1415926;
        int px = 0, py = 0;//网格走向标志
        int i, j, n;//voxel[n] = i*size + j;表示访问的网格点
        double dx = 0, dy = 0;//距离下一条边
        double ax, ay;//增量
        double s_x, s_y, s_z;//发射源起点
        double phi;//弧度
        double k,b;//射线方程参数
        double angle = source->angle;//水平方向源的发射角度
        if (angle >= 360)
        {
            angle -= 360;
        }
        s_x = source->x;
        s_y = source->y;
        s_z = source->z;
        //计算弧度
        phi = angle * PI / 180;
        //计算射线斜率
        if (angle == 90 || angle == 270){
            k = 0.0;
        }
        else{
            k = tan(phi);
        }
        //计算b
        b = (s_y)-k * s_x;
        double reflectAngle = 0;//反射射线的角度
        ax = 1 / fabs(cos(phi));
        ay = 1 / fabs(sin(phi));
        if (angle == 0){
            px = 1;
            py = 0;
            dx = (ceil(s_x) - s_x);
            dy = 100000.0;
            ay = 100000.0;
        }
        else if (angle == 90) {
            px = 0;
            py = 1;
            dx = 100000.0;
            dy = (ceil(s_y) - s_y);
            ax = 100000.0;
        }
        else if (angle == 180){
            px = -1;
            py = 0;
            dx = (s_x - floor(s_x));
            dy = 100000.0;
            ay = 100000.0;
        }
        else if (angle == 270){
            px = 0;
            py = -1;
            dx = 100000.0;
            dy = dy = (ceil(s_y) - s_y);
            ax = 1000.0;
        }
        else if (0 < angle && angle < 90.0){//右上
            px = 1;
            py = 1;
            dx = (ceil(s_x) - s_x) / fabs(cos(phi));
            dy = (ceil(s_y) - s_y) / fabs(sin(phi));

        }
        else if (90.0 < angle && angle < 180.0){//左，上
            px = -1;
            py = 1;
            dx = (s_x - floor(s_x)) / fabs(cos(phi));
            dy = (ceil(s_y) - s_y) / fabs(sin(phi));

        }
        else if (180.0 < angle && angle < 270.0){//左下
            px = -1;
            py = -1;
            dx = (s_x - floor(s_x)) / fabs(cos(phi));
            dy = (s_y - floor(s_y)) / fabs(sin(phi));
        }
        else if (270.0 < angle && angle < 360.0){
            px = 1;
            py = -1;
            dx = (ceil(s_x) - s_x) / fabs(cos(phi));
            dy = (s_y - floor(s_y)) / fabs(sin(phi));
        }
        else{
            return ;
        }
        //确定第一个访问的网格
        i = floor(s_x);
        j = floor(s_y);
        //初始化Node指针
        Node* ret = nullptr;
        //循环，依次访问所有的网格，求交点
        while (0 <= i && i < mesh->size && 0 <= j && j < mesh->size){//判断该位置 ij 是否在网格范围内，如果在则检测交点，否则退出
            double _i = i;
            double _j = j;
            n = i*mesh->size + j;
            //更新ij 和dx, dy
            if (dx < dy){
                i = i + px;
                dx += ax;
            }
            else if (dy < dx){
                j = j + py;
                dy += ay;
            }
            else{
                i = i + px;
                j = j + py;
                dx += ax;
                dy += ay;
            }
            //检测是否遇到接收机
            if (mesh->voxelMesh[n].containRx){
//                qDebug() << "有Rx，但没有捕获" ;
                double A = source->A;
                double B = source->B;
                double C = source->C;
                //cout << "ABC" << A << "," << B << "," << C << endl;
                double x0 = mesh->Rx->x;
                double y0 = mesh->Rx->y;
                double z0 = mesh->Rx->z;
                double d = fabs((A*x0 + B*y0 + C)) / sqrt(A * A + B * B);//点到直线的距离
//                qDebug() << "点到直线距离:" << d;
                if (d < 0.05){//如果水平面上，接收机到射线的距离小于0.1，判定为接收到。
//                    qDebug()<<"捕获"<<endl;
                    ret = new Node(x0, y0);
                    ret->type = Rx;
                    verticalPlane(ret);
                    return;
                }
            }
            //取出所有边
            vector<Edge*> vp = mesh->voxelMesh[n].edgeList;
            if (vp.size() == 0){//如果该voxel没有建筑墙面，下一个Voxel
                continue;
            }

            //在每个小格中优先选择距离源点最近的相交节点
            double minD = 10000;
            //遍历所有边
            for (int i = 0; i < vp.size(); i++){//对此Voxel中的所有edge进行检测
                Edge* edge = vp[i];
                double D = 0;//距源点的距离
                double x1 = edge->startPoint->x;
                double y1 = edge->startPoint->y;
                double x2 = edge->endPoint->x;
                double y2 = edge->endPoint->y;
                //边的线段参数
                double k2 = edge->k;
                double b2 = edge->b;
                double x0, y0;//交点
                //计算交点
                Point* p0 = Intersection(source, source->direction, edge->startPoint, edge->endPoint);
                if(p0 == nullptr){
                    continue;
                }
                x0 = p0->x;
                y0 = p0->y;
                if (fabs(x0 - s_x) < 0.001 && !(angle == 90 || angle == 270)){//细节错误
                    continue;
                }
                if (fabs(y0 - s_y) < 0.001 && !(angle == 0 || angle == 360 || angle == 180)){//细节错误
                    continue;
                }
                //逆向传播无效
                if ((angle == 0 || angle == 360) && x0 <= s_x){
                    continue;
                }
                if (angle == 180 && x0 >= s_x){
                    continue;
                }
                if (0 < angle && angle < 180 && y0 <= s_y){
                    continue;
                }
                if (180 < angle && angle < 360 && y0 >= s_y){
                    continue;
                }
                if (x0 < _i || x0 > _i + 1 || y0 < _j || y0 > _j + 1){//是否在网格内
                    continue;
                }
                D = distance(x0, y0, s_x, s_y);
                if(D > minD){
                    continue;
                }
//                更新minD
                minD = D;
                double newAngle;
                //求镜像点
                double x_s, y_s;
                if (edge->vertical){
                    x_s = 2 * x0 - s_x;
                    y_s = s_y;
                } else{
                    double med = 2 * ((s_y - k2*s_x - b2) / (k2*k2 + 1));
                    x_s = s_x + k2*med;
                    y_s = s_y - med;
                }
                double fabsAngle = fabs(atan((y0 - y_s) / (x0 - x_s)));
                //求新的角度
                fabsAngle = fabsAngle * 180 / PI;
                if (y0 > y_s && x0 > x_s){
                    newAngle = fabsAngle;
                }
                else if (y0 > y_s && x0 < x_s){
                    newAngle = 180.0 - fabsAngle;
                }
                else if (y0 < y_s && x0 >x_s){
                    newAngle = 360 - fabsAngle;
                }
                else if (y0 < y_s && x0 <x_s){
                    newAngle = 180 + fabsAngle;
                }
                else if (y0 == y_s && x0 > x_s){
                    newAngle = 0;
                }
                else if (y0 == y_s && x0 < x_s){
                    newAngle = 180;
                }
                else if (y0 > y_s && x0 == x_s){
                    newAngle = 90;
                }
                else{
                    newAngle = 270;
                }

                //计算交点到端点的的距离，判断是否可以发生绕射
                double distanceToSP  = distance(x0,y0, edge->startPoint->x, edge->startPoint->y);
                double distanceToEP  = distance(x0,y0, edge->endPoint->x, edge->endPoint->y);
                if ( diffractNum == 0
                     && distanceToSP < 0.1
                     && distance(x0, y0, mesh->Rx->x, mesh->Rx->y) < distanceBetweenTxRx)
                {//垂直边缘绕射
                    ret = new Node(edge->startPoint->x, edge->startPoint->y);
                    ret->type = NodeType::diff;
                }else if(diffractNum == 0
                         && distanceToEP <0.1
                         && distance(x0, y0, mesh->Rx->x, mesh->Rx->y) < distanceBetweenTxRx){
                    //垂直边缘绕射
                    ret = new Node(edge->endPoint->x, edge->endPoint->y);
                    ret->type = NodeType::diff;
                }else{//反射
                    ret = new Node(x0, y0,newAngle);
                    ret->type = reflect;
                }
            }
            if(ret != nullptr){
                verticalPlane(ret);
                return;
            }
        }

    }

    Point* Intersection(Point* p1, Point* p2, Point* p3, Point* p4){
            double s1 = direction(p1,p2,p3);
//            qDebug() << s1;

            double s2 = direction(p1,p2,p4);
//            qDebug() << s2;
            if(s1 == 0){
                return new Point(p3->x,p3->y);
            }else if(s2 == 0){
                return new Point(p4->x, p4->y);
            }else if(s1 * s2 < 0){
                // p1p2  p3p4
                double m1 = p2->x - p1->x;
                double n1 = p2->y - p1->y;
                double m2 = p4->x - p3->x;
                double n2 = p4->y - p3->y;
//                qDebug() << "("<<m1<<","<<n1<<")";
//                qDebug() << "("<<m2<<","<<n2<<")";
                double A = n1;
                double B = -m1;
                double C = n2;
                double D = -m2;
                double Beta1 = n1*p1->x - m1*p1->y;
                double Beta2 = n2*p3->x - m2*p3->y;
                // xj = Mj / M
                double M = A*D - B*C;
                double M1 = Beta1*D - Beta2*B;
                double M2 = A*Beta2 - C*Beta1;
                double x = M1 / M;
                double y = M2 / M;
//                qDebug() << x <<"," << y;
                return new Point(x, y);
            }else{
                return nullptr;
            }
    }


    double direction(Point* p1, Point* p2, Point* p3){
        //(m1,n1)x(m2, n2)
        double m1 = p2->x - p1->x;
        double n1 = p2->y - p1->y;

        double m2 = p3->x - p1->x;
        double n2 = p3->y - p1->y;
        return m1*n2 - m2*n1;
    }
    //VPL算法
    void verticalPlane(Node* node){
        if( (node->type == NodeType::Tx)){
            for(int i = 0; i < 360; i += raySpacing){
                node->setAngle(i);
//                qDebug() << "type:" << node->type <<"angle:" << i ;
                nodeList.push_back(node);
//                qDebug() << "****处理TX****" <<"angle:" << i << "深度" << nodeList.size();
                traceOneRay(node);
                nodeList.pop_back();
            }
        }else if(node->type == NodeType::diff){
            diffractNum++;
            for(int i = 0; i < 360; i += raySpacing){
                node->setAngle(i);
                nodeList.push_back(node);
//                qDebug() << "****处理绕射****" << node->type <<"angle:" << i<< "深度" << nodeList.size() ;
                traceOneRay(node);
                nodeList.pop_back();
            }
//            qDebug() << "****处理绕射结束****"  ;
            diffractNum--;
        } else if(node->type == NodeType::reflect){
            nodeList.push_back(node);
            if( nodeList.size() < maxReflectNum + 1){
//                qDebug() << "****处理反射****" << "angle:" << node->angle << "深度" << nodeList.size();
                traceOneRay(node);
//                qDebug() << "反射处理结束";
            }else{

//                Path* path = new Path(nodeList);
//                allPath.push_back(path);
            }
            nodeList.pop_back();
        }else if(node->type == NodeType:: Rx){
            nodeList.push_back(node);
            vector<Node*> tmp;
            for(int i = 0; i < nodeList.size(); i++){
                //恢复到坐标表示
                double x = nodeList[i]->x / 30.0 * (mesh->factor) + mesh->bbox[0];
                double y = nodeList[i]->y / 30.0 * (mesh->factor) + mesh->bbox[1];
//                qDebug() << "....................";
//                qDebug() << x;
//                qDebug() << y;
                Node* ans = new Node(x,y,nodeList[i]->z);
                ans->setAngle(nodeList[i]->angle);
                ans->type = nodeList[i]->type;
                tmp.push_back(ans);
            }
            Path* path = new Path(tmp);
//            path->channelGain(0);
            //添加一条路径
            allPath.push_back(path);
            nodeList.pop_back();
        }

    }



};
#endif // TRACER_H
