#pragma once
#include<QDebug>

#include "object.h"
#include "point.h"
#include<vector>
#include<algorithm>

using namespace std;
/************************************************************************/
/* 场景类
/* 成员变量：存放建筑的数组
/************************************************************************/
class Scene
{

public:
    double bbox[4];
    double factor;
    vector<Object*> objList;
public:
    Scene() {

    }
    /*
    场景构建函数
    输入：从SHP文件读取的所有的点
          每个建筑的结束位置
    */
    Scene(vector<Point*> allPointList, vector<int> objIndex) {
        int startPosi = 0;
        int endPosi = 0;
        for (int i = 0; i < objIndex.size(); i++) {
            endPosi = objIndex[i];
            vector<Point*> b(endPosi-startPosi);
            copy(allPointList.begin() + startPosi, allPointList.begin() + endPosi, b.begin());
            Object *obj = new Object(b, i);
            objList.push_back(obj);
            startPosi = endPosi;
        }
    }

    Scene(vector<Object*> objects){


    }
    ~Scene() {}
};

