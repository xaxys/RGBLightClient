#include "ClientCore.h"

ClientCore::ClientCore(QObject *parent) : QObject(parent) {
    ssdpManager = new SSDPManager(this);
    createSysTrayIcon();
    createMainWindow();
}

ClientCore::~ClientCore() {
}

void ClientCore::sysTrayActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        //µ¥»÷ÍÐÅÌÍ¼±ê
        if (mainWindow != nullptr) {
            mainWindow->showNormal();
        } else {
            createMainWindow();
        }
        break;
    case QSystemTrayIcon::DoubleClick:
        //Ë«»÷ÍÐÅÌÍ¼±ê
        if (mainWindow != nullptr) {
            mainWindow->showNormal();
        } else {
            createMainWindow();
        }
        break;
    default:
        break;
    }
}

void ClientCore::createMainWindow() {
    mainWindow = new RGBLightClient(ssdpManager);
    mainWindow->show();
    mainWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(mainWindow, &RGBLightClient::destroyed, this, [this]() {mainWindow = nullptr; });
}

void ClientCore::createSysTrayIcon() {
    sysTrayIcon = new QSystemTrayIcon(this);
    sysTrayIcon->setIcon(QIcon(":/RGBLightClient/SystrayIcon"));
    sysTrayIcon->setToolTip(QStringLiteral(APP_TITLE));
    sysTrayIcon->show();
    connect(sysTrayIcon, &QSystemTrayIcon::activated, this, &ClientCore::sysTrayActivated);
}
