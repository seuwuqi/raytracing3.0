#pragma once
#include <qlist.h>
#include "point.h"
#include <list>
#include <vector>
using namespace std;
class FilePoint
{
public:
	vector<int> index;
	double Xmin = 0.0;
	double Ymin = 0.0;
	double Xmax = 0.0;
	double Ymax = 0.0;
	double Zmin = 0.0;
	double Zmax = 0.0;
    double denominator;
    double *hightArr = new double[20000];
	vector<Point*> allPointList;
    vector<QString> pathName;
public:
	FilePoint();
	~FilePoint();
	void expendIndex(int num) {
		index.push_back(num);
	}
	void expendPointList(Point *point) {
		allPointList.push_back(point);
	}


    void uniformlize(double denominator) {
		for (int i = 0; i < allPointList.size(); i++) {
            allPointList[i]->x = (allPointList[i]->x - Xmin) / (denominator);
            allPointList[i]->y = (allPointList[i]->y - Ymin) / (denominator);
			allPointList[i]->z = allPointList[i]->z / 600.0;
		}
	}



    Point* resume(Point* point, double denominator, double k,double shift){//网格运算的坐标-->前段显示的坐标
        double x = ((point->x / k* denominator + Xmin) -(Xmax + Xmin) / 2 ) / (Xmax - Xmin)*shift;
        double y = ( (point->y /k * denominator + Ymin)- (Ymax + Ymin) / 2) / (Xmax - Xmin)*shift;;
        double z = point->z/k;
        return new Point(x, y ,z);
    }

	void uniformlize(double shift, double shiftZ) {
		for (int i = 0; i < allPointList.size(); i++) {
			allPointList[i]->x = (allPointList[i]->x - (Xmax + Xmin) / 2) / (Xmax - Xmin)*shift;
			allPointList[i]->y = (allPointList[i]->y - (Ymax + Ymin) / 2) / (Xmax - Xmin)*shift;
            allPointList[i]->z = allPointList[i]->z / 600*shiftZ;
		}
	}

    void uniformlize(double shift, double shiftZ,double denominator) {
        for (int i = 0; i < allPointList.size(); i++) {
            allPointList[i]->x = (allPointList[i]->x - (Xmax + Xmin) / 2) / (denominator)*shift;
            allPointList[i]->y = (allPointList[i]->y - (Ymax + Ymin) / 2) / (denominator)*shift;
            allPointList[i]->z = allPointList[i]->z / 800*shiftZ;
        }
    }
};
