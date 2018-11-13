#include "echoserver.h"
#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"
#include <QtCore/QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include "mesh.h"
#include "echoserver.h"
#include "tracer.h"
QT_USE_NAMESPACE

//! [constructor]
EchoServer::EchoServer(quint16 port, bool debug, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Echo Server"),
                                            QWebSocketServer::NonSecureMode, this)),
    m_debug(debug)
{
    if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        if (m_debug)
            qDebug() << "Echoserver listening on port" << port;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &EchoServer::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &EchoServer::closed);
    }
    updateBuilding();
}
//! [constructor]

EchoServer::~EchoServer()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

//! [onNewConnection]
void EchoServer::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &EchoServer::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &EchoServer::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &EchoServer::socketDisconnected);

    m_clients << pSocket;
}
//! [onNewConnection]

//! [processTextMessage]
void EchoServer::processTextMessage(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (m_debug)
        qDebug() << "Message received:" << message;
    if (pClient) {
        if(message == "1"){
            updateBuilding();
            QJsonObject qjss=QJsonObject::fromVariantMap(mapMap);
            QJsonDocument doc(qjss);
            QString strJson(doc.toJson(QJsonDocument::Compact));
            pClient->sendTextMessage(strJson);

            //QString bound = '';
        }else if(message == "2"){
//          射线追踪

            QString data = rayTracing();
            pClient->sendTextMessage(data);
        }else if(message == '3'){
//          更新道路
            updateRoad();
            QJsonObject qjss=QJsonObject::fromVariantMap(mapMap);
            QJsonDocument doc(qjss);
            QString strJson(doc.toJson(QJsonDocument::Compact));
            pClient->sendTextMessage(strJson);
        }else if(message == "cordinate"){
            QMap<QString, QVariant> map;
            map.insert("xmax",xmax);
            map.insert("xmin",xmin);
            map.insert("ymax",ymax);
            map.insert("ymin",ymin);
            QJsonObject qjss = QJsonObject::fromVariantMap(map);
            QJsonDocument doc(qjss);
            QString strJson(doc.toJson(QJsonDocument::Compact));
            pClient->sendTextMessage(strJson);
        }else{//Tx Rx

            QJsonDocument jsonDocument = QJsonDocument::fromJson(message.toLocal8Bit().data());
               if(jsonDocument.isNull())
               {
                   qDebug()<< "String NULL"<< message.toLocal8Bit().data();
               }
               QJsonObject jsonObject = jsonDocument.object();

               QString type = jsonObject["type"].toString();
               if(type == "tx"){
                   double longitude =  jsonObject["longitude"].toDouble();
                   double latitude=  jsonObject["latitude"].toDouble();
                   double x = (longitude - xmin)/(xmax - xmin);
                   double y = (latitude - ymin)/(xmax - xmin);
                   receivedTx = new Node(x,y);
               }else if(type == "rx"){
                   double longitude =  jsonObject["longitude"].toDouble();
                   double latitude=  jsonObject["latitude"].toDouble();
                   double x = (longitude - xmin)/(xmax - xmin);
                   double y = (latitude - ymin)/(xmax - xmin);
                   receivedRx = new Node(x, y);
               }else{
                   qDebug() << "other type..";
               }



        }

    }
}
//! [processTextMessage]

//! [processBinaryMessage]
void EchoServer::processBinaryMessage(QByteArray message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (m_debug)
        qDebug() << "Binary Message received:" << message;
    if (pClient) {
        pClient->sendBinaryMessage(message);
    }
}
//! [processBinaryMessage]

//! [socketDisconnected]
void EchoServer::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (m_debug)
        qDebug() << "socketDisconnected:" << pClient;
    if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}
//! [socketDisconnected]

QString EchoServer::rayTracing(){
    FilePoint *filePoint = new FilePoint();
    FileManager *fileManager = new FileManager("E:/QT5_11_1/raytracing1.1/rayTracing1.1/RayTracing/BUILDING_nanjing.shp",
                                               "E:/QT5_11_1/raytracing1.1/rayTracing1.1/RayTracing/BUILDING_nanjing.dbf");
    fileManager->readDbfFile(filePoint);
    fileManager->readShpFile(filePoint);
    qDebug() <<"xmax:"<<filePoint->Xmax;
    qDebug() <<"xmin"<<filePoint->Xmin;
    qDebug() <<"ymax"<<filePoint->Ymax;
    qDebug() <<"ymin"<<filePoint->Ymin;
    filePoint->uniformlize(xmax - xmin);
    Scene* scene = new Scene(filePoint->allPointList, filePoint->index);
    Node* rx = new Node(receivedRx->x * 30, receivedRx->y * 30, 0.0, true);
    Mesh* mesh = new Mesh(30, scene, rx);
    //射线追踪器
    Tracer*  tracer = new Tracer(mesh);
    //source = new  Node(14.5, 7.5, 16);
    //tracer->traceAll(source);
    for (double i = 0; i < 360;i+= 2)
    {
        Node* source = new Node(receivedTx->x * mesh->size, receivedTx->y * mesh->size, i);
        tracer->srcList.push_back(source);
        source->vSpread[0] = -90 * 3.14 / 180;
        source->vSpread[1] = 90 * 3.14 / 180;
        source->type = Tx;
        source->z = 0.5;
        source->range[0] = source->range[1] = 0.2;
        tracer->traceAll(source);
        //qDebug() << "i:" << i ;
    }

    QMap<QString,QVariant> data;
    for(int i = 0; i < tracer->allPath.size(); i++){
        Path* path = tracer->allPath[i];
        list<Node*> nodes = path->nodes;
        list<Node*>:: iterator iter;
        QList<QVariant> list;
        for(iter = nodes.begin(); iter != nodes.end(); iter++){
            QMap<QString,QVariant> map;
            Node* node = *iter;
            //将node的坐标映射成前段展示的坐标
            Point* point = filePoint->resume(node,xmax - xmin,mesh->size,80);
            map.insert("x", (point->x));
            map.insert("y", (point->y));
            map.insert("z", (point->z));
            list.append(QVariant(map));
        }
        data.insert(QString::number(i,10),QVariant(list));
    }
    QJsonObject qjss=QJsonObject::fromVariantMap(data);
    QJsonDocument doc(qjss);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    return strJson;
}


void EchoServer::updateRoad(){
    mapMap.clear();
    FilePoint *filePoint = new FilePoint();
    FileManager *fileManager = new FileManager("./R.shp","./R.dbf");
    fileManager->readRoadDbf(filePoint);
    fileManager->readRoadShp(filePoint);
    qDebug() <<"xmax:"<<filePoint->Xmax;
    qDebug() <<"xmin"<<filePoint->Xmin;
    qDebug() <<"ymax"<<filePoint->Ymax;
    qDebug() <<"ymin"<<filePoint->Ymin;
    QMap<QString,QVariant> boundMap;
    filePoint->uniformlize(80,0,xmax - xmin);//(0,1)
    Scene *scene = new Scene(filePoint->allPointList, filePoint->index);
    QJsonObject qjs;
    for(int i = 0; i < scene->objList.size(); i++){
        QMap<QString,QVariant> myMap;
        QList<QVariant> posList;
        for(int j=0;j<scene->objList[i]->pointList.size();j++){
            QMap<QString,QVariant> tempMap;
            tempMap.insert(QString("x"),QVariant((scene->objList[i]->pointList[j]->x )));
            tempMap.insert(QString("y"),QVariant((scene->objList[i]->pointList[j]->y )));
//            tempMap.insert(QString("x"),QVariant((scene->objList[i]->pointList[j]->x - 0.5)*80));
//            tempMap.insert(QString("y"),QVariant((scene->objList[i]->pointList[j]->y - 0.5)*80));
            tempMap.insert(QString("z"),QVariant(0) );
            posList.append(QVariant(tempMap));
        }

        mapMap.insert(QString::number(i,10),QVariant(posList));
    }
}
void EchoServer::updateBuilding(){
    mapMap.clear();
    FilePoint *filePoint = new FilePoint();
    FileManager *fileManager = new FileManager("E:/QT5_11_1/raytracing1.1/rayTracing1.1/RayTracing/BUILDING_nanjing.shp",
                                               "E:/QT5_11_1/raytracing1.1/rayTracing1.1/RayTracing/BUILDING_nanjing.dbf");
    fileManager->readDbfFile(filePoint);
    fileManager->readShpFile(filePoint);
    qDebug() <<"xmax:"<<filePoint->Xmax;
    qDebug() <<"xmin"<<filePoint->Xmin;
    qDebug() <<"ymax"<<filePoint->Ymax;
    qDebug() <<"ymin"<<filePoint->Ymin;
    QMap<QString,QVariant> boundMap;
    xmax =filePoint->Xmax;
    xmin =filePoint->Xmin;
    ymax =filePoint->Ymax;
    ymin =filePoint->Ymin;
    filePoint->uniformlize(80,50,xmax - xmin);//(0,1)
    Scene *scene = new Scene(filePoint->allPointList, filePoint->index);
    QJsonObject qjs;
    for(int i = 0; i < scene->objList.size(); i++){
        QMap<QString,QVariant> myMap;
        QList<QVariant> posList;
        for(int j=0;j<scene->objList[i]->pointList.size();j++){
            QMap<QString,QVariant> tempMap;
            tempMap.insert(QString("x"),QVariant((scene->objList[i]->pointList[j]->x )));
            tempMap.insert(QString("y"),QVariant((scene->objList[i]->pointList[j]->y)));
            tempMap.insert(QString("z"),QVariant(scene->objList[i]->pointList[j]->z ));
//            tempMap.insert(QString("x"),QVariant((scene->objList[i]->pointList[j]->x - 0.5)*80));
//            tempMap.insert(QString("y"),QVariant((scene->objList[i]->pointList[j]->y - 0.5)*80));
//            tempMap.insert(QString("z"),QVariant(scene->objList[i]->pointList[j]->z *10) );
            posList.append(QVariant(tempMap));
        }

        mapMap.insert(QString::number(i,10),QVariant(posList));
    }

}
