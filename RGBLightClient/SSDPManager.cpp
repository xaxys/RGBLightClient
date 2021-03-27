#include <QNetworkInterface>
#include <QNetworkDatagram>
#include <QNetworkReply>
#include <QEventLoop>
#include <QUrl>
#include "SSDPManager.h"
#include <QNetworkRequest>
#include <QXmlStreamReader>

SSDPManager::SSDPManager(QObject *parent) : QObject(parent), manager(QNetworkAccessManager(this)) {
    // connect ssdp poll
    connect(&poll, &QTimer::timeout, this, [this]() {
        refreshSockets();
        sendDiscover();
    });

    refreshSockets();
    sendDiscover();
    poll.start(interval);
}

SSDPManager::~SSDPManager() {
    poll.stop();
}

void SSDPManager::refreshSockets() {
    // old ip list
    auto oldIPs = QSet<QHostAddress>(sockets.keyBegin(), sockets.keyEnd());
    for (const auto& ip : QNetworkInterface::allAddresses()) {
        // skip ip still existed
        if (oldIPs.contains(ip)) {
            oldIPs.remove(ip);
            continue;
        }
        // add new ip
        auto sock = new QUdpSocket(this);
        sock->bind(ip, 0, QUdpSocket::ShareAddress);
        connect(sock, &QUdpSocket::readyRead, this, [sock, this]() {
            while (sock->hasPendingDatagrams()) {
                parseDatagram(sock->receiveDatagram());
            }
        });
        sockets.insert(ip, sock);
    }

    // remove ip not exist any longer
    for (const auto& ip : oldIPs) {
        sockets[ip]->deleteLater();
        sockets.remove(ip);
    }
}

void SSDPManager::sendUpdate() {
    emit updated(lights.values());
}

void SSDPManager::sendDiscover() {
    static QHostAddress mcastGroup("239.255.255.250");
    static QByteArray search(
"M-SEARCH * HTTP/1.1\r\n\
Host: 239.255.255.250:1900\r\n\
Man: \"ssdp:discover\"\r\n\
ST: upnp:rootdevice\r\n"
    );
    for (const auto& sock : sockets) {
        sock->writeDatagram(search, mcastGroup, 1900);
    }
}

void SSDPManager::parseDatagram(const QNetworkDatagram& dgram) {
    QRegExp rx("LOCATION: (.*\\.xml)");
    QHostAddress address = dgram.senderAddress();
    if (rx.indexIn(dgram.data().data()) != -1) {
        QUrl url = rx.capturedTexts()[1];
        manager.get(QNetworkRequest(url));
        connect(&manager, &QNetworkAccessManager::finished, this, [this, address](QNetworkReply* reply) {
            if (lights.contains(address)) return;
            QXmlStreamReader xml(reply->readAll());
            RGBLightConfig config = RGBLightConfig::fromXml(xml);
            if (xml.hasError()) {
                qDebug() << "Error >> " << xml.errorString();
            } else {
                addLight(address, config);
            }
            reply->deleteLater();
        });
    }
}

void SSDPManager::addLight(const QHostAddress& address, const RGBLightConfig& config) {
    auto light = new RGBLightController(address, config);
    connect(light, &RGBLightController::disconnected, this, [this, light]() {
        lights.remove(light->getAddress());
        light->deleteLater();
        sendUpdate();
    });
    lights.insert(address, light);
    sendUpdate();
}
