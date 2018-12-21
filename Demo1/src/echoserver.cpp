#include "echoserver.h"
#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"
#include <QtCore/QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include "mesh.h"
#include "echoserver.h"
#include "tracer.h"
#include "QJsonArray"
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
    connect(pSocket, &QWebSocket::disconnected, this, &EchoServer::socketDisconnected);
    qDebug() << "socket connected:";
    m_clients << pSocket;
}
//! [onNewConnection]

//! [processTextMessage]
void EchoServer::processTextMessage(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (m_debug)
//        qDebug() << "Message received:" << message;
    if (pClient) {
        if(message == "1"){
//            updateBuilding();
//            QJsonObject qjss=QJsonObject::fromVariantMap(mapMap);
//            QJsonDocument doc(qjss);
//            QString strJson(doc.toJson(QJsonDocument::Compact));
//            pClient->sendTextMessage(strJson);

            //QString bound = '';
        }else if(message == "2"){
//          射线追踪
//            QString data = rayTracing();

            if(rxData->dynamic){
                QJsonObject json = DynamicVPL();
                QJsonDocument doc(json);
                QString strJson(doc.toJson(QJsonDocument::Compact));
                pClient->sendTextMessage(strJson);
            }else{
                QJsonObject json = VPL(false,0);
                QJsonDocument doc(json);
                QString strJson(doc.toJson(QJsonDocument::Compact));
                pClient->sendTextMessage(strJson);
            }

        }else if(message == '3'){
//          更新道路
//            updateRoad();
//            QJsonObject qjss=QJsonObject::fromVariantMap(mapMap);
//            QJsonDocument doc(qjss);
//            QString strJson(doc.toJson(QJsonDocument::Compact));
//            pClient->sendTextMessage(strJson);
        }else if(message == "cordinate"){
//            QMap<QString, QVariant> map;
//            map.insert("xmax",xmax);
//            map.insert("xmin",xmin);
//            map.insert("ymax",ymax);
//            map.insert("ymin",ymin);
//            QJsonObject qjss = QJsonObject::fromVariantMap(map);
//            QJsonDocument doc(qjss);
//            QString strJson(doc.toJson(QJsonDocument::Compact));
//            pClient->sendTextMessage(strJson);
        }else{//
            QJsonDocument jsonDocument = QJsonDocument::fromJson(message.toLocal8Bit().data());
               if(jsonDocument.isNull())
               {
                   qDebug()<< "String NULL"<< message.toLocal8Bit().data();
               }
               QJsonObject jsonObject = jsonDocument.object();
               QString type = jsonObject["type"].toString();
               if(type == "vehicle"){
                   qDebug()<< jsonObject;
                   updateVehicle(jsonObject);
               }
               else if(type == "senario"){
                    sceneDate = jsonObject;
                    updateScene(sceneDate);
               }
        }
    }
}
//! [processTextMessage]


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

QJsonObject EchoServer::DynamicVPL(){
//    updateScene(sceneDate);
    double a1 = 3.6;
    double a2 = 5;

    double Txstartx = txData->x;
    double Txstarty = txData->y;

    double Txendx = txData->x2;
    double Txendy = txData->y2;


    double Rxstartx = rxData->x;
    double Rxstarty = rxData->y;

    double Rxendx = rxData->x2;
    double Rxendy = rxData->y2;


    double Txspeed = txData->speed/3.6;
    double Rxspeed = rxData->speed/3.6;


    double Txdistance = txData->distance;
    double Rxdistance = rxData->distance;

    double Txt1 = Txspeed / a1;
    double Txt3 = Txspeed / a2;

    double Rxt1 = Rxspeed / a1;
    double Rxt3 = Rxspeed / a2;

    double Txt2 = (Txdistance - 0.5*a1*Txt1*Txt1 - 0.5*a2*Txt2*Txt2)/Txspeed;
    double Rxt2 = (Rxdistance - 0.5*a1*Rxt1*Rxt1 - 0.5*a2*Rxt2*Rxt2)/Rxspeed;

    double Txtotal = Txt1 + Txt2 + Txt3;
    double Rxtotal = Rxt1 + Rxt2 + Rxt3;

    double T = Txtotal > Rxtotal ? Txtotal : Rxtotal;

    if(T > 30){
        T = 30;
    }
    double txmoved;
    double rxmoved;
    double txV;
    double rxV;
    QJsonArray result;
    for(double t = 0; t <= T; t++){
        if(t < Txt1){
             txmoved = 0.5*a1*t*t;
             txV = a1*t;
        }else if(t < Txt1 + Txt2){
             txmoved = 0.5*a1*Txt1*Txt1 + Txspeed * (t - Txt1);
             txV = Txspeed;
        }else if(t < Txtotal){
             txmoved = Txdistance - 0.5*a2*(Txtotal - t)*(Txtotal - t);
             txV = Txspeed - a2*(t - Txt1 - Txt2);
        }else{
            txmoved = Txdistance;
            txV = 0;
        }

        if(t < Rxt1){
            rxmoved = 0.5*a1*t*t;
            rxV = a1*t;
        }else if(t < Rxt1 + Rxt2){
            rxmoved = 0.5*a1*Rxt1*Rxt1 + Rxspeed * (t - Txt1);
            rxV = Rxspeed;
        }else if(t < Rxtotal){
            rxmoved = Rxdistance - 0.5*a2*(Rxtotal - t)*(Rxtotal - t);
            rxV = Rxspeed - a2*(t - Rxt1 - Rxt2);
        }else{
            rxmoved = Rxdistance;
            rxV = 0;
        }

        qDebug() << "txmoved:" << txmoved;
        qDebug() << "rxmoved:" << rxmoved;
        qDebug() << "txv:" << txV;
        qDebug() << "rxv:" << rxV;

        txData->speed = txV;
        txData->x = Txstartx + (Txendx - Txstartx) * txmoved / Txdistance;
        txData->y = Txstarty + (Txendy - Txstarty) * txmoved / Txdistance;
        rxData->speed = rxV;
        rxData->x = Rxstartx + (Rxendx - Rxstartx) * rxmoved / Rxdistance;
        rxData->y = Rxstarty + (Rxendy - Rxstarty) * rxmoved / Rxdistance;

        qDebug() <<"tx_x" << txData->x *30;
        qDebug() <<"tx_y" << txData->y *30;
        qDebug() <<"rx_x" << rxData->x *30;
        qDebug() <<"rx_y" << rxData->y *30;
        QJsonObject paths = VPL(true,t);

        result.append(paths);
    }

    QJsonObject dynamic;
    dynamic.insert("type", "dynamic");
    dynamic.insert("result", result);
    return dynamic;
}


QJsonObject EchoServer::VPL(bool isDynamic,double time){
//    if(!isDynamic){
//         updateScene(sceneDate);
//    }
    updateScene(sceneDate);
    qDebug() << "scene size : "<<scene->objList.size();
    Node* rx = new Node(rxData->x, rxData->y, 0.0, true);
    Mesh* mesh = new Mesh(40, scene, rx);
    Node* source = new Node(txData->x * mesh->size, txData->y * mesh->size, 0);
    source->type = NodeType::Tx;
//    射线追踪器
    Tracer*  tracer = new Tracer(mesh,source,carList);
//    tracer->verticalPlane(source);
    tracer->verticalPlane(source);
    tracer->processNLOSv();
//    tracer->processLOS();
    if(vehicleAdded){
        carList.pop_back();
        vehicleAdded = false;
    }
    qDebug()<<tracer->allPath.size()<<" paths has been accepted";
//    QMap<QString,QVariant> data;
    QJsonObject json;
    json.insert("type", QString("output"));
    QJsonArray paths;
    double totalgain = 0;
    double N1 = 0;
    double N2 = 0;
    double M = 0;
    for(int i = 0; i < tracer->allPath.size(); i++){
        QJsonObject path;
        Path* p = tracer->allPath[i];
        double gain = p->channelGain(0);
        totalgain += gain;
        M += gain * gain;
        path.insert("pathloss",  20 * log10(gain));
        double delay = p->timeDelay();
        path.insert("timeDelay",delay);
        N1 += gain * gain * delay;
        N2 += gain * gain * delay * delay;
        path.insert("type",p->path_type);
        path.insert("reflectNum",p->getReflectCount());
        path.insert("diffractNum",p->getDiffractCount());
        path.insert("dopplerShift",p->getDopplerShift());
        path.insert("DOA", p->getDOA());
        path.insert("AOA", p->getAOA());
        if(!isDynamic){
            QJsonArray nodeList;
            for(int j = 0; j < p->nodeSet.size() ; j++){
                Node* node = p->nodeSet[j];
                QJsonObject point;
                point.insert("x",node->x);
                point.insert("y",node->y);
                point.insert("z",node->z);
                nodeList.insert(j,point);
            }
            path.insert("nodeList",nodeList);
        }
        paths.insert(i,path);
    }
    json.insert("averageDelay",N1 / M);
    json.insert("rms", sqrt(N2/M - pow(N1/M,2)));
    json.insert("totalgain", 20 * log10(totalgain));
    json.insert("paths",paths);
    json.insert("time",time);

//    double x = nodeList[i]->x / mesh->size * (mesh->factor) + mesh->bbox[0];
//    double y = nodeList[i]->y / mesh->size * (mesh->factor) + mesh->bbox[1];
    json.insert("tx_x",txData->x *factor + xmin);
    json.insert("tx_y",txData->y * factor + ymin);
    json.insert("rx_x",rxData->x * factor + xmin);
    json.insert("rx_y",rxData->y * factor + ymin);
    json.insert("tx_v",txData->speed);
    json.insert("rx_v",rxData->speed);
    return json;
}

void EchoServer::updateScene(QJsonObject jsonObject){
    QJsonArray features = jsonObject["features"].toArray();
    QJsonArray bbox = jsonObject["bbox"].toArray();
    scene = new Scene();
//    reset
    scene->bbox[0] = xmin = bbox[0].toDouble();
    scene->bbox[1] = ymin = bbox[1].toDouble();
    scene->bbox[2] = xmax = bbox[2].toDouble();
    scene->bbox[3] = ymax = bbox[3].toDouble();
    scene->factor = factor = (xmax - xmin) > (ymax - ymin)? (xmax - xmin) : (ymax - ymin);
    scene->objList.clear();
    qDebug() << xmin << ymin << xmax << ymax;
    for(QJsonValue feature : features){
        Object* obj = new Object();
        QJsonArray coordinates = feature["coordinates"].toArray();
        double z = feature["z"].toString().toDouble();
        obj->z = z;
        for(QJsonValue coord : coordinates){
            double x = coord.toArray()[0].toDouble();
            double y = coord.toArray()[1].toDouble();
            x = (x - xmin) / factor;
            y = (y - ymin) / factor;
            obj->pointList.push_back(new Point(x, y ,z ));

        }

        obj->setEdgeList(obj->pointList);
        scene->objList.push_back(obj);
    }

    for(int i = 0; i < carList.size(); i++){
        scene->objList.push_back(carList[0]);
    }
}


void EchoServer::updateVehicle(QJsonObject vehicle){
    QString vehicleType = vehicle["vehicleType"].toString();
    bool dynamic = vehicle["dynamic"].toBool();
    int speed = vehicle["speed"].toString().toInt();
    QJsonArray location1 = vehicle["location1"].toArray();
    QJsonArray location2 = vehicle["location2"].toArray();
    double distance = vehicle["distance"].toDouble();
    double longitude =  location1[0].toDouble();
    double latitude=  location1[1].toDouble();
    double x1 = (longitude - xmin)/(factor);
    double y1 = (latitude - ymin)/(factor);
    double lng2 = location2[0].toDouble();
    double lat2 = location2[1].toDouble();
    double x2 = (lng2 - xmin)/(factor);
    double y2 = (lat2 - ymin)/(factor);
    qDebug() << "speed" << speed;
    if(vehicleType == "tx"){
        txData = new Vehicle();
        txData->x = x1;
        txData->y = y1;
        txData->x2 = x2;
        txData->y2= y2;
        txData->speed = speed;
        txData->dynamic = dynamic;
        txData->type = vehicleType;
        txData->distance = distance;
    }else if(vehicleType == "rx"){
        rxData = new Vehicle();
        rxData->x = x1;
        rxData->y = y1;
        rxData->x2 = x2;
        rxData->y2= y2;
        rxData->speed = speed;
        rxData->dynamic = dynamic;
        rxData->type = vehicleType;
        rxData->distance = distance;
    }else{//障碍车
        if(vehicleAdded){
            carList.clear();
            vehicleAdded = false;
        }
        qDebug() << vehicle;
        Object* obstaclCar = new Object();
        obstaclCar->type = "vehicle";
        double z = vehicle["height"].toDouble();
        QJsonArray bbox = vehicle["bbox"].toArray();
        for(QJsonValue coor : bbox){
            QJsonObject point = coor.toObject();
            double lng = point["x"].toDouble();
            double lat = point["y"].toDouble();
            double x = (lng - xmin)/(factor);
            double y = (lat - ymin)/(factor);
            obstaclCar->pointList.push_back(new Point(x,y,z));
        }
        obstaclCar->setEdgeList(obstaclCar->pointList);
        carList.push_back(obstaclCar);
        vehicleAdded = true;
        qDebug() << carList.size();
        for(int i = 0; i < carList[0]->pointList.size(); i++){
            qDebug() << carList[0]->pointList[i]->x * 30;
            qDebug() << carList[0]->pointList[i]->y * 30;
            qDebug() << carList[0]->pointList[i]->z;
        }
        qDebug() << carList[0]->pointList.size();
        qDebug() << carList[0]->edgeList.size();
    }
}



//void EchoServer::updateRoad(){
//    mapMap.clear();
//    FilePoint *filePoint = new FilePoint();
//    FileManager *fileManager = new FileManager("./R.shp","./R.dbf");
//    fileManager->readRoadDbf(filePoint);
//    fileManager->readRoadShp(filePoint);
//    qDebug() <<"xmax:"<<filePoint->Xmax;
//    qDebug() <<"xmin"<<filePoint->Xmin;
//    qDebug() <<"ymax"<<filePoint->Ymax;
//    qDebug() <<"ymin"<<filePoint->Ymin;
//    QMap<QString,QVariant> boundMap;
//    filePoint->uniformlize(80,0,xmax - xmin);//(0,1)
//    Scene *scene = new Scene(filePoint->allPointList, filePoint->index);
//    QJsonObject qjs;
//    for(int i = 0; i < scene->objList.size(); i++){
//        QMap<QString,QVariant> myMap;
//        QList<QVariant> posList;
//        for(int j=0;j<scene->objList[i]->pointList.size();j++){
//            QMap<QString,QVariant> tempMap;
//            tempMap.insert(QString("x"),QVariant((scene->objList[i]->pointList[j]->x )));
//            tempMap.insert(QString("y"),QVariant((scene->objList[i]->pointList[j]->y )));
//            tempMap.insert(QString("x"),QVariant((scene->objList[i]->pointList[j]->x - 0.5)*80));
//           tempMap.insert(QString("y"),QVariant((scene->objList[i]->pointList[j]->y - 0.5)*80));
//            tempMap.insert(QString("z"),QVariant(0) );
//            posList.append(QVariant(tempMap));
//        }

//        mapMap.insert(QString::number(i,10),QVariant(posList));
//    }
//}




//void EchoServer::updateBuilding(){
//    mapMap.clear();
//    FilePoint *filePoint = new FilePoint();
//    FileManager *fileManager = new FileManager("E:/QT5_11_1/raytracing1.1/rayTracing1.1/RayTracing/BUILDING_nanjing.shp",
//                                               "E:/QT5_11_1/raytracing1.1/rayTracing1.1/RayTracing/BUILDING_nanjing.dbf");
//    fileManager->readDbfFile(filePoint);
//    fileManager->readShpFile(filePoint);
//    qDebug() <<"xmax:"<<filePoint->Xmax;
//    qDebug() <<"xmin"<<filePoint->Xmin;
//    qDebug() <<"ymax"<<filePoint->Ymax;
//    qDebug() <<"ymin"<<filePoint->Ymin;
//    QMap<QString,QVariant> boundMap;
//    xmax =filePoint->Xmax;
//    xmin =filePoint->Xmin;
//    ymax =filePoint->Ymax;
//    ymin =filePoint->Ymin;
//    filePoint->uniformlize(80,50,xmax - xmin);//(0,1)
//    Scene *scene = new Scene(filePoint->allPointList, filePoint->index);
//    QJsonObject qjs;
//    for(int i = 0; i < scene->objList.size(); i++){
//        QMap<QString,QVariant> myMap;
//        QList<QVariant> posList;
//        for(int j=0;j<scene->objList[i]->pointList.size();j++){
//            QMap<QString,QVariant> tempMap;
//            tempMap.insert(QString("x"),QVariant((scene->objList[i]->pointList[j]->x )));
//            tempMap.insert(QString("y"),QVariant((scene->objList[i]->pointList[j]->y)));
//            tempMap.insert(QString("z"),QVariant(scene->objList[i]->pointList[j]->z ));
//           tempMap.insert(QString("x"),QVariant((scene->objList[i]->pointList[j]->x - 0.5)*80));
//           tempMap.insert(QString("y"),QVariant((scene->objList[i]->pointList[j]->y - 0.5)*80));
//          tempMap.insert(QString("z"),QVariant(scene->objList[i]->pointList[j]->z *10) );
//            posList.append(QVariant(tempMap));
//        }

//        mapMap.insert(QString::number(i,10),QVariant(posList));
//    }

//}
