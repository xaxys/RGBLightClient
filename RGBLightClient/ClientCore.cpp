#include <QMenu>
#include <QAction>
#include <Windows.h>
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
        showMainWindow();
        break;
    case QSystemTrayIcon::DoubleClick:
        //Ë«»÷ÍÐÅÌÍ¼±ê
        showMainWindow();
        break;
    default:
        break;
    }
}

void ClientCore::showMainWindow() {
    if (mainWindow != nullptr) {
        mainWindow->showNormal();
        SetWindowPos(HWND(mainWindow->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }
    else {
        createMainWindow();
    }
}

void ClientCore::createMainWindow() {
    mainWindow = new RGBLightClient(ssdpManager);
    mainWindow->show();
    mainWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(mainWindow, &RGBLightClient::destroyed, this, [this]() { mainWindow = nullptr; });
}

void ClientCore::createSysTrayIcon() {
    sysTrayIcon = new QSystemTrayIcon(this);
    sysTrayIcon->setIcon(QIcon(":/RGBLightClient/SystrayIcon"));
    sysTrayIcon->setToolTip(QStringLiteral(APP_TITLE));
    sysTrayIcon->show();
    connect(sysTrayIcon, &QSystemTrayIcon::activated, this, &ClientCore::sysTrayActivated);

    auto menu = new QMenu;
    auto showWindow = new QAction(QStringLiteral("ÏÔÊ¾³ÌÐò"), this);
    auto exitAppAction = new QAction(QStringLiteral("ÍË³ö³ÌÐò"), this);
    connect(showWindow, &QAction::triggered, this, &ClientCore::showMainWindow);
    connect(exitAppAction, &QAction::triggered, this, []() { exit(0); });
    menu->addAction(showWindow);
    menu->addAction(exitAppAction);
    menu->setStyleSheet("QMenu{color:black;border-bottom:1px solid #ccc;}");
    sysTrayIcon->setContextMenu(menu);
}
