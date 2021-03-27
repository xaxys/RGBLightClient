#pragma once

#include <QtWidgets/QMainWindow>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <memory>
#include "ui_RGBLightClient.h"
#include "resource.h"
#include "SSDPManager.h"
#include "RGBLightDetail.h"

class RGBLightClient : public QMainWindow {
    Q_OBJECT

    Ui::RGBLightClientClass ui;
    SSDPManager* ssdpManager = nullptr;
    RGBLightDetail* detailPage = nullptr;
    QLabel* status = nullptr;

public:
    RGBLightClient(SSDPManager* ssdp = new SSDPManager, QWidget *parent = nullptr);
    ~RGBLightClient();

private slots:
    void onRefreshButtonClicked();
    void onSSDPUpdated(const QList<RGBLightController*>&);
    void onListWidgetSelectionChanged(QListWidgetItem*);

};
