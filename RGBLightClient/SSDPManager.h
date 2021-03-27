#pragma once

#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include <QNetworkAccessManager>
#include "RGBLightController.h"

class SSDPManager : public QObject {
	Q_OBJECT
	QTimer poll;
	QNetworkAccessManager manager;
	QHash<QHostAddress, QUdpSocket*> sockets;
	QHash<QHostAddress, RGBLightController*> lights;

public:
	int interval = 1000;

public:
	SSDPManager(QObject *parent = nullptr);
	~SSDPManager();
	void refreshSockets();
	void sendDiscover();
	void sendUpdate();

signals:
	void updated(const QList<RGBLightController*>&);

private:
	void parseDatagram(const QNetworkDatagram&);
	void addLight(const QHostAddress&, const RGBLightConfig&);

};
