#include <QHostAddress>
#include <QUdpSocket>
#include <QAction>
#include <QNetworkInterface>
#include <QNetworkDatagram>
#include <QTimer>
#include "RGBLightClient.h"

RGBLightClient::RGBLightClient(SSDPManager* ssdp, QWidget *parent) : ssdpManager(ssdp), QMainWindow(parent) {
    ui.setupUi(this);
    status = new QLabel(this);
    ui.statusBar->addWidget(status);
    ui.statusBar->setSizeGripEnabled(false);
    setWindowTitle(QStringLiteral(APP_TITLE));

    connect(ssdpManager, &SSDPManager::updated, this, &RGBLightClient::onSSDPUpdated);
    ssdpManager->sendUpdate();
}

RGBLightClient::~RGBLightClient() {
}

void RGBLightClient::onSSDPUpdated(const QList<RGBLightController*>& lights) {
    ui.listWidget->setUpdatesEnabled(false);
    ui.listWidget->clear();
    for (const auto& light : lights) {
        auto item = new QListWidgetItem(ui.listWidget);
        item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
        item->setText(light->getName());
        item->setToolTip(light->getAddress().toString());
        item->setData(1, QVariant::fromValue(light));
        ui.listWidget->addItem(item);
    }
    ui.listWidget->setUpdatesEnabled(true);
    status->setText(QStringLiteral("找到%1个RGB设备").arg(lights.length()));
}

void RGBLightClient::onListWidgetSelectionChanged(QListWidgetItem* item) {
    if (detailPage) {
        ui.verticalLayout_detailPage->removeWidget(detailPage);
        detailPage->deleteLater();
    }
    auto light = item->data(1).value<RGBLightController*>();
    detailPage = new RGBLightDetail(light->getConfig(), ui.tabWidget);
    ui.verticalLayout_detailPage->addWidget(detailPage);

    connect(detailPage, &RGBLightDetail::commandExec, light, &RGBLightController::sendMessage);
    connect(light, &RGBLightController::received, detailPage, &RGBLightDetail::onReceiveMessage);
    detailPage->syncFromLight();
}

void RGBLightClient::onRefreshButtonClicked() {
    ssdpManager->refreshSockets();
}
