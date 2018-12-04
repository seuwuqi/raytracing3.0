#ifndef MESH_H
#define MESH_H

#include <stdio.h>
#include "voxel.h"
#include "object.h"
#include "scene.h"
#include<iostream>
#include<QDebug>
#include <QString>
using namespace std;
/************************************************************************/
/* 网格类
成员变量：
       网格大小 size
       存放Voxel的数组Voxel* voxelMesh
       接收节点对象 Rx；
*/
/************************************************************************/
class Mesh
{
public:
    //网格的尺寸，20、30
    int size;
    //voxel数组
    Voxel* voxelMesh;
    //rx
    Node* Rx;

    double* bbox;
    double factor;
public:
    Mesh(){

    }
    Mesh(int size,Scene* scene){
        this->size = size;
        voxelMesh = new Voxel[size*size];
        setMesh(scene,Rx);
        Rx->x = size/2;
        Rx->y = size/2;
    }

    Mesh(int size, Scene* scene,Node* rx){
        this->size = size;
        voxelMesh = new Voxel[ size * size];
        rx->x = rx->x * size;
        rx->y = rx->y * size;
        setMesh(scene,rx);
        Rx = rx;
        bbox = scene->bbox;
        factor =scene->factor;
    }

    ~Mesh(){

    }
    void setMesh(Scene* scene, Node* rx){
        qDebug() << scene->objList.size();
        vector<Object*> objArr = scene->objList;
        qDebug()<<"..";
        for (int i = 0; i < objArr.size(); i++){//遍历所有的建筑
            Object* obj = objArr[i];
            QString type = obj->type;
            obj->scale(size);
            //cout << "************" << obj.z;
            for(int j = 0; j < obj->edgeList.size(); j++){//遍历一个建筑的所有边
                Edge* edge = obj->edgeList[j];
                int xLow = floor(edge->xMin);
                int xHight = ceil(edge->xMax);
                int yLow = floor(edge->yMin);
                int yHight = ceil(edge->yMax);
                for (int k = xLow; k < xHight; k++){//遍历每条边跨度内的voxel
                    for (int l = yLow; l < yHight; l++){
                        int n = k*size + l;
                        voxelMesh[n].i = k;
                        voxelMesh[n].j = l;
                        voxelMesh[n].edgeList.push_back(edge);
                    }
                }
            }
        }

        //构建rx
        int i = floor(rx->x);
        int j = floor(rx->y);
        voxelMesh[i*size + j].i = i;
        voxelMesh[i*size + j].j = j;
        voxelMesh[i*size + j].containRx = true;
    }
};


#endif // MESH_H
