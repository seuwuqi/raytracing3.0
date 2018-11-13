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
    Mesh* mesh;//网格
    Node* p;//节点指针
    const int maxReflectNum = 2;
    int reflectNum = 0;
    vector<Node*> srcList;
    vector<list<Node*>> pathArr;
    vector<Path*> allPath;
private:

    Node* insertNode(Node* source, Node* p, Node* ret,bool& off){
        //搭建树结构
            while (p != source && ret->distane < p->distane)
            {
                p = p->parent;
            }
            //在temp处插入ret
            ret->parent = p;//当前节点的父节点是temp
            if (source->type == reflect2 && (ret->type == reflect||ret->type == Rx))
            {//发射源是二次反射节点
                ret->rightChild = p->rightChild;
                if (p->rightChild != nullptr)
                {//防止空指针异常
                    p->rightChild->parent = ret;
                }
                p->rightChild = ret;
                while (p->rightChild != nullptr)
                {
                    p = p->rightChild;
                }
                off = true;
            }
            else if (source->type == reflect){//发射源是一次反射节点
                if (ret->type == reflect)
                {
                    ret->type = reflect2;
                }
                else
                {
                    ret->type = diff2;
                }
                ret->leftChild = p->leftChild;
                if (p->leftChild != nullptr)
                {//防止空指针异常
                    p->leftChild->parent = ret;
                }
                p->leftChild = ret;
                while (p->leftChild != nullptr)
                {
                    p = p->leftChild;
                }
            }
            else
            {//发射源是Tx或发射源是二次反射点
                ret->rightChild = p->rightChild;
                if (p->rightChild != nullptr)
                {//防止空指针异常
                    p->rightChild->parent = ret;
                }
                p->rightChild = ret;
                while (p->rightChild != nullptr)
                {
                    p = p->rightChild;
                }
            }
            return p;
    }

    void addPath(Node* rx,PathType type){
        qDebug() << "add path" <<endl;
        Node* temp = rx;
        list<Node*> one_path;
        bool find_r = false;
        bool find_r2 = false;
        while (temp != nullptr)
        {

            one_path.push_front(temp);
            temp = temp->parent;
        }
        qDebug() << "test over" << endl;
        Path* path = new Path(one_path,mesh->size,type);
        pathArr.push_back(one_path);
        allPath.push_back(path);
    }
    void verticalSpread(Node * source){
        Node* p = source;
        if (source->type == Tx)
        {//只存在完全越过和部分阻挡
    //		cout << "****************" << endl;
            while (p->rightChild != nullptr)
            {
                p = p->rightChild;
                double Z = p->z;//节点所在建筑物的高度
                double D = p->distane;
                if (p->type == reflect || p->type == newSouece1 || p->type == newSouece2)
                {//反射节点

                    //计算光线照射到此距离时的范围up，down
                    double down = source->range[0] + D*tan(source->vSpread[0]);
                    double up = source->range[1] + D*tan(source->vSpread[1]);
                    if (down > Z){//完全越过,不是反射墙
    //					cout << "完全越过" << endl;
                        p->type = diff;//重新设置为绕射节点
                    }
                    else
                    {//部分阻挡
                        p->range[0] = max(down,0.0);//被照射部分的下界
                        p->range[1] = Z;//被照射部分的上界
                        p->vSpread[0] = atan((p->range[0] - source->range[0]) / D);
    //					cout << p->range[1] << "," << source->range[1] << "," << D << endl;
                        p->vSpread[1] = atan((p->range[1] - source->range[1]) / D);
                        source->vSpread[0] = p->vSpread[1];
    //					cout << "x:" << p->x << "y:" << p->y << "z:" << p->z;
    //					cout << "改变后的下扩展" << source->vSpread[0] * 180 / 3.14 << endl;
                    }

                }
                else
                {
                }
            }
        }
        else if (source->type == reflect)
        {//如果源是一次反射节点，可能存在完全阻挡，部分阻挡，完全越过

            //cout << "***************************" << endl;
            while (p->leftChild != nullptr)
            {

                p = p->leftChild;
                double Z = p->z;//节点所在建筑物的高度
                double D = p->distane;
                if (p->type == reflect2)
                {//反射节点
    //				cout << "++++++++++++++fanshejiedian++++++++++++++++++++++++" << endl;
                    //计算光线照射到此距离时的范围up，down
                    double down = source->range[0] + D*tan(source->vSpread[0]);
                    double up = source->range[1] + D*tan(source->vSpread[1]);
                    if (down > Z){//完全越过,不是反射墙
    //					cout << "完全越过" << endl;
                        p->type = diff2;//重新设置为后向绕射节点
                    }
                    else if (0<up && up <= Z){//完全阻挡
    /*					cout << "完全阻挡" << endl;*/
                        p->range[0] = max(down, 0.0);
                        p->range[1] = up;
                        p->vSpread[0] = atan((p->range[0] - source->range[0]) / D);
                        p->vSpread[1] = atan((p->range[1] - source->range[1]) / D);
                        source->vSpread[0] = 90;//保证后面的节点不再发生反射
                    }
                    else if (up < 0){//没有照射到
    /*					cout << "没有照射到" << endl;*/
                        p->type = diff2;
                    }
                    else{//部分阻挡
    /*					cout << "部分阻挡" << endl;*/
                        p->range[0] = max(down, 0.0);//被照射部分的下界
                        p->range[1] = Z;//被照射部分的上界
                        p->vSpread[0] = atan((p->range[0] - source->range[0]) / D);
                        p->vSpread[1] = atan((p->range[1] - source->range[1]) / D);
                        source->range[0] = source->range[1];
                        source->vSpread[0] = p->vSpread[1];
    // 					cout << "x:" << p->x << "y:" << p->y << "z:" << p->z;
    // 					cout << "改变后的下扩展" << source->vSpread[0] * 180 / 3.14 << endl;
                    }

                }
                else
                {

                }
            }
        }
    }
public:
    Tracer(){

    }
    Tracer(Mesh* mesh){
        this->mesh = mesh;
    }
    ~Tracer(){

    }
    //射线追踪函数，追踪从参数source出发的射线在mesh中的节点路径，返回最后一个节点。
    Node* trace(Node* source){
        p = source;//节点指针指向将要追踪的节点
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
            return nullptr;
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
                //cout << "有Rx，但没有捕获" << endl;
                double A = source->A;
                double B = source->B;
                double C = source->C;
                //cout << "ABC" << A << "," << B << "," << C << endl;
                double x0 = mesh->Rx->x;
                double y0 = mesh->Rx->y;
                double z0 = mesh->Rx->z;
                double d = fabs((A*x0 + B*y0 + C)) / sqrt(A * A + B * B);//点到直线的距离
                if (d < 0.05){//如果水平面上，接收机到射线的距离小于0.1，判定为接收到。
                    qDebug()<<"捕获"<<endl;
                    ret = new Node(x0,y0);
                    double D = sqrt((x0 - s_x)*(x0 - s_x) + (y0 - s_y)*(y0 - s_y));
                    ret->type = Rx;
                    ret->z = z0;
                    ret->distane = D;

                    //插入ret
                    ret->parent = p;
                    if (p->type == reflect)
                    {
                        p->leftChild = ret;
                    }
                    else
                    {
                        p->rightChild = ret;
                    }
                    p = ret;
                    PathType type = INCIDENT;
                    if (source->type == Tx)
                    {
                        type = PathType::INCIDENT;
                    }
                    else if (source->type == reflect)
                    {
                        type = PathType::REFLECT;
                    }
                    else if (source->type == reflect2)
                    {
                        type = PathType::REFLECT_REFLECT;
                    }
                    else if (source->type == diff)
                    {
                        type = PathType::DIFFRECT;
                    }
                    else if (source->type == diff2)
                    {
                        type = PathType::DIFFRECT_DEFFRECT;
                    }
                    source->isReceived = true;

                    //添加路径
                    addPath(p,type);
                }
            }
            //取出所有边
            vector<Edge*> vp = mesh->voxelMesh[n].edgeList;
            if (vp.size() == 0){//如果该voxel没有建筑墙面，下一个Voxel
                continue;
            }

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
                if (angle == 90 || angle == 270){//射线垂直
                    if (edge->vertical){//边垂直
                        continue;
                    }
                    else{
                        x0 = s_x;
                        y0 = k2*x0 + b2;
                    }
                }
                else{//射线不垂直
                    if (edge->vertical){//边垂直
                        x0 = x1;
                        y0 = k*x0 + b;
                    }
                    else{//一般情况
                        x0 = -((b - b2) / (k - k2));
                        y0 = (k*b2 - k2 * b) / (k - k2);
                    }
                }


                //判断交点是否合法
                if (x0 > x1 && x0 > x2){
                    continue;
                }
                if (x0 < x1 && x0 < x2){
                    continue;
                }
                if (y0 < y1 && y0 < y2){
                    continue;
                }
                if (y0 > y1 && y0 > y2){
                    continue;
                }
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
                //求距离
                //cout << x0  << ","<< y0 << endl;
                D = sqrt((x0 - s_x)*(x0 - s_x) + (y0 - s_y)*(y0 - s_y));
                //求新的角度
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
                ret = new Node(x0, y0,newAngle);//一定要用带newAngle参数的构造方程，否则射线的参数ABC不能被初始化
                ret->distane = D;//录入当前节点到源点的距离
                //判断是否发生反射
                double dif = fabs(angle - edge->nAngle);
                if (dif > 180.0){
                    dif = 360.0 - dif;
                }
                if (dif > 90)
                {//反射节点
                    ret->type = reflect;
                    ret->z = edge->hight;
                    if ( fabs(edge->startPoint->x - x0) < 0.005 || fabs(edge->endPoint->x - x0) < 0.005 )
                    {//拐点绕射
                        ret->newSrc = true;
                    }
                }
                else
                {
                    ret->type = diff;
                    ret->z = edge->hight;
                }
                //插入节点
                bool off = false;//为了二次反射退出检测设置的开关标志
                p = insertNode(source, p, ret,off);//插入新的节点，返回最后一个节点
                if (off)
                {
                    return p;
                }
            }//for循环结束
        }//while结束
            //在搜索范围内循环，利用dx dy确定访问voxel.i，j，逐个搜索，直到超出网格范围
        //p已经指向最后一个节点，从前到后依次设置垂直方向的扩展
        return p;
    }
    void traceAll(Node* source){
        qDebug() << "enter traceALl function" ;
        Node* p1 = trace(source);
            verticalSpread(source);//调整垂直方向的传播，必要时删除节点，或终止二叉树
            int count = 0;
            while (p1 != nullptr)
            {
                count++;
                qDebug() << "count:" << count;
                if(p1->type == reflect)
                {

                    qDebug() << "遇到一个反射点" ;
                    if (p1->newSrc)
                    {
                        qDebug() << "一次拐角绕射源" << endl;
                        /*p1->type = newSouece1;*/
                        for ( int i = 0; i < 360; i+=2)
                        {//如果是拐角，各个角度再trace一次
                            qDebug() << "processing i:" << i;
                            Node * newS = new Node(p1->x,p1->y,i);
                            newS->distane = p1->distane;
                            newS->parent = source;
                            srcList.push_back(newS);
                            newS->vSpread[0] = p1->vSpread[0];
                            newS->vSpread[1] = p1->vSpread[1];
                            newS->type = p1->type;
                            newS->z = p1->z;
                            newS->range[0] = p1->range[0];
                            newS->range[1] = p1->range[1];
                            trace(newS);
                        }
                        qDebug() << "over";
                    }
                    Node* p2 = trace(p1);
                    verticalSpread(p1);//完成一次二级搜索后，调整垂直方向传播范围
                    if (p1->isReceived)
                    {
                        source->isReceived = true;
                    }
                    while (p2->type != reflect)
                    {
                        if (p2->type == reflect2){
                            if (p2->newSrc)
                            {
                                /*p2->type = newSouece2;*/
                                for (int i = 0; i < 360; i += 20)
                                {
                                    Node * newS = new Node(p2->x, p2->y, i);
                                    newS->parent = p1;
                                    newS->distane = p2->distane;
                                    srcList.push_back(newS);
                                    newS->vSpread[0] = p2->vSpread[0];
                                    newS->vSpread[1] = p2->vSpread[1];
                                    newS->type = p2->type;
                                    newS->z = p2->z;
                                    newS->range[0] = p2->range[0];
                                    newS->range[1] = p2->range[1];
                                    trace(newS);
                                }
                            }
                            Node* p3 = trace(p2);
                            verticalSpread(p2);

                            if (p2->isReceived)
                            {
                                source->isReceived = true;
                            }
                        }
                        p2 = p2->parent;
                    }
                }
                p1 = p1->parent;
            }
    }


};
#endif // TRACER_H
