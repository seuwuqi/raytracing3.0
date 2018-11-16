#pragma once
#include <vector>
#include "point.h"
#include "edge.h"
using namespace std;
class Object
{
public:
	vector<Point*> pointList;
	vector<Edge*> edgeList;
	double z;
	int index;
public:
    Object(){

    }
    Object(vector<Point*> pointList, int index) {
		this->pointList = pointList;
		this->index = index;
		this->z = pointList.front()->z;
        setEdgeList(pointList);
	}
    ~Object(){}

	void setEdgeList(vector<Point*> pointList) {
        vector<Point*>::iterator iter;

        for (iter = pointList.begin(); iter != pointList.end(); iter++){
            if (iter == pointList.end() - 1){
                Edge* edge = new Edge(*iter, *pointList.begin());
                edgeList.push_back(edge);
                continue;
            }
            Edge* edge = new Edge(*(iter), *(iter + 1));
            edge->index = index;
            edgeList.push_back(edge);
        }
	}

	void scale(double k, double x_v, double y_v) {
		for (int i = 0; i < pointList.size(); i++) {
			pointList[i]->x *= k;
			pointList[i]->x += x_v;
			pointList[i]->y *= k;
			pointList[i]->y += y_v;
		}
		setEdgeList(pointList);
	}

	void scale(double k) {
		z = z * k;
		for (int i = 0; i < pointList.size(); i++) {
			pointList[i]->x *= k;
			pointList[i]->y *= k;
			pointList[i]->z *= k;
		}
		edgeList.clear();
		setEdgeList(pointList);
    }
};
