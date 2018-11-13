#ifndef PATH_H
#define PATH_H

#include <vector>
#include "node.h"
#include <list>
#include <iostream>
#include <QDebug>
using namespace std;
//路径类，用于表示路劲，计算路劲损失和延时扩展
class Path
{
public:
    list<Node*> nodes;
    double pathLoss;//路径损失
    double delay;//延时
    PathType path_type;
public:

    Path(list<Node*> _nodes,double mesh_size,PathType type){
        nodes = _nodes;
        pathProcess(nodes, mesh_size,type);
    }
    Path(){

    }

    ~Path()
    {
    }

    //凸包检测
    vector<Node*> convexhull(Node* p1, Node* p2){
        vector<Node*> convex;
        double ref = p1->z > p2->z ? p1->z : p2 ->z;
        Node*temp = p2->parent;
        while ( temp != p1){
            temp->type = diff;
            if (temp->z > ref ){
                cout << "@@@" <<temp->z<< endl;
                convex.push_back(temp);
            }
            temp = temp->parent;
        }
        return convex;
    }
    //
    double HorizontalDiff(vector<Node*> reflects){
        if (reflects.size() == 0)
        {
            vector<Node*> convex = convexhull(nodes.front(), nodes.back());
            double z = convex[0]->z;
            double v = (z - nodes.front()->z)*sqrt(2 * (nodes.back()->distane) / (0.05*convex[0]->distane*(nodes.back()->distane - convex[0]->distane)));
            return 20 * log(0.225/v);
        }
        else if (reflects.size() == 1)
        {
            vector<Node*> convex1 = convexhull(nodes.front(), reflects[0]);
            vector<Node*> convex2 = convexhull(reflects[0], nodes.back());
        }
        else if (reflects.size() == 2)
        {
            vector<Node*> convex1 = convexhull(nodes.front(), reflects[0]);
            vector<Node*> convex2 = convexhull(reflects[0], reflects[1]);
        }
        return 20 * log(0.225 / 30);;
    }

    void pathProcess(list<Node*> path,double mesh_size,PathType type){
        path_type = type;
        cout << "size:"<<path.size()<<endl;
        cout << "TYPE:" << type << endl;
        //处理每一条路径
        double d_3D;//三维距离
        double k = 600.0 / mesh_size;
        //取出所有的反射节点放入一个数组
        vector<Node*> reflects;
        vector<Node*> Allreflects;
        list<Node*>::iterator iter = path.begin();
        Node* Tx = path.front();
        Node* Rx = path.back();
        while(iter != path.end()){//遍历path
            qDebug() <<"test" <<endl;
            Node* node = *iter;
            if (node->type == reflect || node->type == reflect2){
                Allreflects.push_back(node);
            }
            iter++;
        }
        cout << Allreflects.size() <<endl;
        if (type == REFLECT){

            for (int i = Allreflects.size() - 1; i >= 0; i--)
            {
                if (Allreflects[i]->type == reflect)
                {
                    reflects.push_back(Allreflects[i]);
                    break;
                }
            }
        }
        else if (type == REFLECT_REFLECT)
        {
            for (int i = Allreflects.size() - 1; i >= 0; i--)
            {
                if (Allreflects[i]->type == reflect2)
                {
                    reflects.push_back(Allreflects[i]);
                    break;
                }
            }
            for (int i = Allreflects.size() - 1; i >= 0; i--)
            {
                if (Allreflects[i]->type == reflect)
                {
                    reflects.push_back(Allreflects[i]);
                    break;
                }
            }
        }
        if (reflects.size() == 0){//如果没有反射节点，直接计算距离和路径损耗
            double d = Rx->distane;//二维距离
            d_3D = sqrt(pow(Tx->z - Rx->z, 2) + pow(d, 2));
            pathLoss = -20 * log(d_3D*k);
            //double loss = HorizontalDiff(reflects);
            //pathLoss += loss;
            cout << "没有反射节点:" <<pathLoss << endl;
        }
        else if (reflects.size() == 1)
        {//如果只有一个反射节点
            double d1 = reflects[0]->distane;//反射点到tx的距离
            double d2 = Rx->distane;//rx到反射点的距离
            double z1 = Tx->z;
            double z2 = Rx->z;
            double z = (z1*d2 + z2*d1) / (d1 + d2);
            reflects[0]->z = z;
            d_3D = sqrt(pow(z1 - z2, 2) + pow(d1 + d2, 2));//总距离
            double i = (fabs(Tx->angle - reflects[0]->angle));
            if (i > 180)
            {
                i = 360 - i;
            }
            i = i / 2.0;
            i = i*3.14 / 180;
            cout << "角度" <<i<< endl;
            double gama1 = (sin(i) - sqrt(4.0 - cos(i)*cos(i)));
            double gama2 =  (sin(i) + sqrt(4.0 - cos(i)*cos(i)));
            double game = fabs(gama1/gama2);
            pathLoss = -20 * log(d_3D*k)+10*log(game);
            cout << "一个反射节点:" << pathLoss << endl;
        }
        else if (reflects.size() == 2)
        {//如果有两个反射节点
            double d1 = reflects[0]->distane;//反射点到tx的距离
            double d2 = reflects[1]->distane;
            double d3 = Rx->distane;//rx到反射点的距离
            double z1 = Tx->z;
            double z2 = Rx->z;
            double z_r1;
            double z_r2;
            if (z1 > z2)
            {
                z_r1 = (z1 - z2)*(d2 + d3) / (d1 + d2 + d3) + z2;
                z_r2 = (z1 - z2)*(d3) / (d1 + d2 + d3) + z2;
            }
            else
            {
                z_r1 = (z2 - z1)*(d1) / (d1 + d2 + d3) + z1;
                z_r2 = (z2 - z1)*(d1 + d2) / (d1 + d2 + d3) + z1;
            }
            reflects[0]->z = z_r1;
            reflects[1]->z = z_r2;
            d_3D = sqrt(pow(z1 - z2, 2) + pow(d1 + d2 + d3, 2));
            pathLoss = -20 * log(d_3D*k)-10 * log(4);
            cout << "两个个反射节点:" << pathLoss << endl;
        }
        else
        {
            pathLoss = -500;
        }
        //路径损失，单位dB
    }
};

#endif // PATH_H
