#pragma once

#include <QObject>
#include <QSystemTrayIcon>
#include "SSDPManager.h"
#include "resource.h"
#include "RGBLightClient.h"

class ClientCore : public QObject {
	Q_OBJECT

	QSystemTrayIcon* sysTrayIcon = nullptr;
	SSDPManager* ssdpManager = nullptr;
	RGBLightClient* mainWindow = nullptr;

public:
	ClientCore(QObject *parent = nullptr);
	~ClientCore();

private:
	void sysTrayActivated(QSystemTrayIcon::ActivationReason reason);
	void createMainWindow();
	void createSysTrayIcon();

};
