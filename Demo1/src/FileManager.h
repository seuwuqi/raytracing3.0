#pragma once
#include "FilePoint.h"
#include <iostream>
#include <QDebug>
class FileManager
{
private:
    FILE* srcFile = nullptr;
    FILE* dbfFile = nullptr;
    int RecordNum;
//    const char* shpName = "E:\\QT5_11_1\\rayTracing-master\\rayTracing-master\\RayTracing\\BUILDING_nanjing.shp";
//    const char* dbfName = "E:\\QT5_11_1\\rayTracing-master\\rayTracing-master\\RayTracing\\BUILDING_nanjing.dbf";
//    const char* shpName ="G:\\nanjing\\BUILDING_nanjing.shp";
//    const char* dbfName="G:\\nanjing\\BUILDING_nanjing.dbf";
public:
    FileManager(){
        srcFile = fopen("G:\\nanjing\\BUILDING_nanjing.shp", "rb");
        dbfFile = fopen("G:\\nanjing\\BUILDING_nanjing.dbf", "r");
    }
    FileManager(const char* pathName1, const char* pathName2) {
        srcFile = fopen(pathName1, "rb");
        dbfFile = fopen(pathName2, "r");
    }
	~FileManager() {}


    void readRoadShpHead(FilePoint* filePoint){
        if (srcFile == nullptr)
        {
            qDebug() << "打开文件失败";
            return;
        }
        int FileCode;
        int Unused;
        int FileLength;
        int Version;
        int ShapeType;
        double Zmin;
        double Zmax;
        double Mmin;
        double Mmax;
        fread(&FileCode, sizeof(int), 1, srcFile);

        for (int i = 0; i < 5; i++)
            fread(&Unused, sizeof(int), 1, srcFile);

        fread(&FileLength, sizeof(int), 1, srcFile);
        fread(&Version, sizeof(int), 1, srcFile);
        fread(&ShapeType, sizeof(int), 1, srcFile);
        fread(&(filePoint->Xmin), sizeof(double), 1, srcFile);
        fread(&(filePoint->Ymin), sizeof(double), 1, srcFile);
        fread(&(filePoint->Xmax), sizeof(double), 1, srcFile);
        fread(&(filePoint->Ymax), sizeof(double), 1, srcFile);
        fread(&Zmin, sizeof(double), 1, srcFile);
        fread(&Zmax, sizeof(double), 1, srcFile);
        fread(&Mmin, sizeof(double), 1, srcFile);
        fread(&Mmax, sizeof(double), 1, srcFile);
        cout << filePoint->Xmin << "," << filePoint->Xmax << "," << filePoint->Ymin << "," << filePoint->Ymax << endl;
    }
    void readShpHead(FilePoint* filePoint){
        if (srcFile == nullptr)
        {
            return;
        }
        int FileCode;
        int Unused;
        int FileLength;
        int Version;
        int ShapeType;
        double Zmin;
        double Zmax;
        double Mmin;
        double Mmax;
        fread(&FileCode, sizeof(int), 1, srcFile);

        for (int i = 0; i < 5; i++)
            fread(&Unused, sizeof(int), 1, srcFile);

        fread(&FileLength, sizeof(int), 1, srcFile);
        fread(&Version, sizeof(int), 1, srcFile);
        fread(&ShapeType, sizeof(int), 1, srcFile);
        fread(&(filePoint->Xmin), sizeof(double), 1, srcFile);
        fread(&(filePoint->Ymin), sizeof(double), 1, srcFile);
        fread(&(filePoint->Xmax), sizeof(double), 1, srcFile);
        fread(&(filePoint->Ymax), sizeof(double), 1, srcFile);
        fread(&Zmin, sizeof(double), 1, srcFile);
        fread(&Zmax, sizeof(double), 1, srcFile);
        fread(&Mmin, sizeof(double), 1, srcFile);
        fread(&Mmax, sizeof(double), 1, srcFile);
        cout << filePoint->Xmin << "," << filePoint->Xmax << "," << filePoint->Ymin << "," << filePoint->Ymax << endl;
    }

    void readDbfHead(){

        if (dbfFile == NULL)
        {
            //cout << "fail to open the file!" << endl;
            return;
        }
        int i, j;
        char version;

        fread(&version, 1, 1, dbfFile);

        cout << version << "version!" << endl;
        char date[3];
        for (i = 0; i < 3; i++)
        {
            fread(date + i, 1, 1, dbfFile);
        }

        fread(&RecordNum, sizeof(int), 1, dbfFile);
        if (RecordNum < 0)
        {
            RecordNum = 13146;
        }
        cout << "the number of records:" << RecordNum << endl;
//        cout << sizeof(short)<<","<<sizeof(int)<<endl;
        short HeaderByteNum;
        fread(&HeaderByteNum, sizeof(short), 1, dbfFile);

        short RecordByteNum;
        fread(&RecordByteNum, sizeof(short), 1, dbfFile);

        short Reserved1;
        fread(&Reserved1, sizeof(short), 1, dbfFile);

        char Flag4s;
        fread(&Flag4s, sizeof(char), 1, dbfFile);

        char EncrypteFlag;

        fread(&EncrypteFlag, sizeof(char), 1, dbfFile);

        int Unused;
        for (i = 0; i < 3; i++)
        {
            fread(&Unused, sizeof(int), 1, dbfFile);
        }

        char MDXFlag;
        fread(&MDXFlag, sizeof(char), 1, dbfFile);
        //ID
        char LDriID;
        fread(&LDriID, sizeof(char), 1, dbfFile);

        short Reserved2;
        fread(&Reserved2, sizeof(short), 1, dbfFile);



        char name[11];

        char fieldType;

        int Reserved3;

        char fieldLength;

        char decimalCount;

        short Reserved4;

        char workID;

        short Reserved5[5];

        char mDXFlag1;

        int fieldscount;

        fieldscount = (HeaderByteNum - 32) / 32;
//		cout << fieldscount << endl;

        for (i = 0; i < fieldscount; i++)
        {

            //FieldName----11   bytes

            fread(name, 11, 1, dbfFile);


            fread(&fieldType, sizeof(char), 1, dbfFile);

            //Reserved3----4     bytes

            Reserved3 = 0;

            fread(&Reserved3, sizeof(int), 1, dbfFile);

            //FieldLength--1     bytes

            fread(&fieldLength, sizeof(char), 1, dbfFile);
            //DecimalCount-1   bytes

            fread(&decimalCount, sizeof(char), 1, dbfFile);

            //Reserved4----2     bytes

            Reserved4 = 0;

            fread(&Reserved4, sizeof(short), 1, dbfFile);

            //WorkID-------1    bytes

            fread(&workID, sizeof(char), 1, dbfFile);

            //Reserved5----10   bytes

            for (j = 0; j < 5; j++)
            {

                fread(Reserved5 + j, sizeof(short), 1, dbfFile);

            }

            //MDXFlag1-----1 bytes

            fread(&mDXFlag1, sizeof(char), 1, dbfFile);

        }

        char terminator;

        fread(&terminator, sizeof(char), 1, dbfFile);
    }
    void readRoadShp(FilePoint* filePoint){//读取道路文件R.shp
        // 	PolyLine{//数据结构
        // 	Double[4]               Box         // 当前线状目标的坐标范围
        // 	Integer                 NumParts    // 当前线目标所包含的子线段的个数
        // 	Integer                 NumPoints   // 当前线目标所包含的顶点个数
        // 	Integer[NumParts]       Parts       // 每个子线段的第一个坐标点在 Points 的位置
        // 	Point[NumPoints]        Points      // 记录所有坐标点的数组
        // }
            readShpHead(filePoint);
            int RecordNumber;
            int ContentLength;
            int id = 0;
            int num = 0;
            while ((fread(&RecordNumber, sizeof(int), 1, srcFile) != 0))
            {
                fread(&ContentLength, sizeof(int), 1, srcFile);
                int shapeType; //几何类型
                double Box[4]; //坐标范围，表示当前面坐标的范围
                int NumParts; //表示构成当前面状目标的子环的个数
                int NumPoints; //表示构成当前面状目标所包含的坐标点个数
                int *Parts;   //记录了每个子环的坐标在 Points 数组中的起始位置
                //读几何类型
                fread(&shapeType, sizeof(int), 1, srcFile);
                // 读 Box
                for (int i = 0; i < 4; i++)
                    fread(Box + i, sizeof(double), 1, srcFile);
                // 读 NumParts 和 NumPoints
                fread(&NumParts, sizeof(int), 1, srcFile);
                fread(&NumPoints, sizeof(int), 1, srcFile);
                //装填index
                //object的起始位置
                id += NumPoints;
                filePoint->index.push_back(id);
                Parts = new int[NumParts];

                for (int i = 0; i < NumParts; i++)
                {
                    fread(Parts + i, sizeof(int), 1, srcFile);
                }//读取每个子环的起始位置
                double *PointsX;
                double *PointsY;
                PointsX = new double[NumPoints];
                PointsY = new double[NumPoints];
                for (int i = 0; i < NumPoints; i++){
                    fread(PointsX + i, sizeof(double), 1, srcFile);
                    fread(PointsY + i, sizeof(double), 1, srcFile);
                    Point* point = new Point(PointsX[i], PointsY[i]);
                    filePoint->allPointList.push_back(point);
                }
                delete[] PointsX;
                delete[] PointsY;
                delete[] Parts;
                num++;
                //totol = num - 1;
            }
            cout << num << endl;
    }
    void readRoadDbf(FilePoint *filePoint){
        readDbfHead();
        string width;
            string direction;
            string length;
            char *name;
            char   deleteFlag;
            char media[64];
            //读取dbf文件记录开始
            int i, j;
            fread(&deleteFlag, sizeof(char), 1, dbfFile);
            for (i = 0; i < RecordNum; i++){
                for (j = 0; j < 64; j++)
                    strcpy(media + j, "");
                for (j = 0; j < 1; j++)
                    fread(media + j, sizeof(char), 1, dbfFile);
                direction = media;
                //读取name
                for (j = 0; j < 64; j++)
                    strcpy(media + j, "");
                for (j = 0; j < 10; j++)
                    fread(media + j, sizeof(char), 1, dbfFile);
                name = media;
                filePoint->pathName.push_back(name);
                //roadList.push_back(raod);
                for (j = 0; j < 41; j++)
                    fread(media + j, sizeof(char), 1, dbfFile);
            }

    }
	void readShpFile(FilePoint *filePoint) {
\
        readShpHead(filePoint);
        int RecordNumber;
		int ContentLength;
		int id = 0;
		int num = 0;
		while ((fread(&RecordNumber, sizeof(int), 1, srcFile) != 0))
		{
			fread(&ContentLength, sizeof(int), 1, srcFile);
			int shapeType; 
			double Box[4]; 
			int NumParts; 
			int NumPoints; 
			int *Parts;   
			fread(&shapeType, sizeof(int), 1, srcFile);
			for (int i = 0; i < 4; i++)
				fread(Box + i, sizeof(double), 1, srcFile);
			fread(&NumParts, sizeof(int), 1, srcFile);
			fread(&NumPoints, sizeof(int), 1, srcFile);
			id += NumPoints;
			filePoint->index.push_back(id);
			Parts = new int[NumParts];

			for (int i = 0; i < NumParts; i++)
			{
				fread(Parts + i, sizeof(int), 1, srcFile);
			}

			double *PointsX;
			double *PointsY;
			double *PointsZ;
			double *PointsM;


			PointsX = new double[NumPoints];
			PointsY = new double[NumPoints];
			for (int i = 0; i < NumPoints; i++) {
				fread(PointsX + i, sizeof(double), 1, srcFile);
				fread(PointsY + i, sizeof(double), 1, srcFile);
				Point *point=new Point(PointsX[i], PointsY[i], filePoint->hightArr[num]);
				filePoint->allPointList.push_back(point);
			}
			delete[] PointsX;
			delete[] PointsY;
			num++;
		}
	}
	void readDbfFile(FilePoint *filePoint) {
        readDbfHead();
		int FID_BUILDI;
		string MAPID;
		string ID;
		string CLASS;
		string SHOW;
		string _3D;
		double HEIGHT;
		string CARTO_ID, ADMINCODE;
		int FID;
		int ID_1;
		double Z;
		char   deleteFlag;

        char media[32];
        int i,j;
		fread(&deleteFlag, sizeof(char), 1, dbfFile);
        for (i = 0; i < RecordNum; i++)
		{

            for (j = 0; j < 31; j++)
				strcpy(media + j, "");
			int len;
            for (j = 0; j < 9; j++)
				len = fread(media + j, sizeof(char), 1, dbfFile);
			if (len == 0) {
				return;
			}
			FID_BUILDI = atoi(media);

			for (j = 0; j < 31; j++)
				strcpy(media + j, "");
			for (j = 0; j < 10; j++)
				fread(media + j, sizeof(char), 1, dbfFile);

			MAPID = media;
			// 		cout << "MAPID: " << MAPID << endl;


			for (j = 0; j < 31; j++)

				strcpy(media + j, "");
			for (j = 0; j < 10; j++)

				fread(media + j, sizeof(char), 1, dbfFile);

			ID = media;
			// 		cout << "ID: " << ID << endl;

			for (j = 0; j < 31; j++)

				strcpy(media + j, "");
			for (j = 0; j < 20; j++)

				fread(media + j, sizeof(char), 1, dbfFile);

			CLASS = media;
			// 		cout << "CLASS: " << CLASS << endl;

			for (j = 0; j < 31; j++)

				strcpy(media + j, "");
			for (j = 0; j < 4; j++)
				fread(media + j, sizeof(char), 1, dbfFile);

			SHOW = media;
			// 		cout << "show: " << SHOW << endl;

			//3d
			for (j = 0; j < 31; j++)

				strcpy(media + j, "");
			for (j = 0; j < 30; j++)

				fread(media + j, sizeof(char), 1, dbfFile);

			_3D = media;
			// 		cout << "3d: " << _3D << endl;

			/*
			*/
			for (j = 0; j < 31; j++)

				strcpy(media + j, "");
			for (j = 0; j < 6; j++)

				fread(media + j, sizeof(char), 1, dbfFile);

			HEIGHT = atof(media);
			filePoint->hightArr[i] = HEIGHT;
//			cout << HEIGHT << endl;
			/*
			*/
			//CARTO_ID
			for (j = 0; j < 31; j++)

				strcpy(media + j, "");
			for (j = 0; j < 20; j++)

				fread(media + j, sizeof(char), 1, dbfFile);

			CARTO_ID = media;
			// 		cout << "carto_id: " << CARTO_ID << endl;


			//admincode
			for (j = 0; j < 31; j++)

				strcpy(media + j, "");
			for (j = 0; j < 6; j++)

				fread(media + j, sizeof(char), 1, dbfFile);

			ADMINCODE = media;
			// 		cout << "admincode: " << ADMINCODE << endl;

			for (j = 0; j < 31; j++)

				strcpy(media + j, "");
			for (j = 0; j < 4; j++)

				fread(media + j, sizeof(char), 1, dbfFile);
			FID = atoi(media);
			// 		cout << "fid: " << FID << endl;


			//id
			for (j = 0; j < 31; j++)

				strcpy(media + j, "");
			for (j = 0; j < 5; j++)

				fread(media + j, sizeof(char), 1, dbfFile);

			ID_1 = atoi(media);
			// 		cout << "id: " << ID_1 << endl;

			//Z
			for (j = 0; j < 31; j++)

				strcpy(media + j, "");
			for (j = 0; j < 20; j++)

				fread(media + j, sizeof(char), 1, dbfFile);

			Z = atof(media);
			// 		cout << "z: " << Z << endl;

		}
	}
};

