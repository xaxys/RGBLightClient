#include "RGBLightController.h"

RGBLightConfig RGBLightConfig::fromXml(QXmlStreamReader& xml) {
	RGBLightConfig config;
	while (!xml.atEnd() && !xml.hasError()) {
		if (xml.isStartElement()) {
			if (xml.name() == "friendlyName") {
				config.name = xml.readElementText();
			}
			else if (xml.name() == "UDN") {
				config.UDN = xml.readElementText();
			}
			else if (xml.name() == "serialNumber") {
				config.serialNumber = xml.readElementText();
			}
			else if (xml.name() == "URLBase") {
				config.URLBase = xml.readElementText();
			}
			else if (xml.name() == "presentationURL") {
				config.presentationURL = xml.readElementText();
			}
			else if (xml.name() == "manufacturer") {
				config.manufacturer = xml.readElementText();
			}
			else if (xml.name() == "manufacturerURL") {
				config.manufacturerURL = xml.readElementText();
			}
			else if (xml.name() == "modelName") {
				config.modelName = xml.readElementText();
			}
			else if (xml.name() == "modelNumber") {
				config.modelNumber = xml.readElementText();
			}
			else if (xml.name() == "modelURL") {
				config.modelURL = xml.readElementText();
			}
		}
		xml.readNext();
	}
	return config;
}

RGBLightController::RGBLightController(const QHostAddress& address, const RGBLightConfig& config, QObject* parent)
	: config(config), name(config.name), address(address), QObject(parent) {
	connect(&webSocket, &QWebSocket::connected, this, &RGBLightController::onConnected);
	connect(&webSocket, &QWebSocket::disconnected, this, &RGBLightController::onDisconnected);
	connect(&webSocket, qOverload<QAbstractSocket::SocketError>(&QWebSocket::error), this, &RGBLightController::onWebSocketError);
	connect(&webSocket, &QWebSocket::textMessageReceived, this, &RGBLightController::onMessageReceived);
	connectToLight();
}

RGBLightController::~RGBLightController() {
	webSocket.close();
}

void RGBLightController::onConnected() {
	emit connected();
}

void RGBLightController::onDisconnected() {
	emit disconnected();
}

void RGBLightController::onWebSocketError(QAbstractSocket::SocketError error) {
	qDebug() << __FILE__ << __LINE__ << (int)error << webSocket.errorString();
	emit errored(webSocket.errorString());
	webSocket.close();
}

void RGBLightController::onMessageReceived(QString msg) {
	emit received(msg);
}

void RGBLightController::sendMessage(QString msg) {
	webSocket.sendTextMessage(msg);
}

void RGBLightController::connectToLight() {
	auto url = QString("ws://%1:%2").arg(address.toString()).arg(port);
	webSocket.open(QUrl(url));
}

QString RGBLightController::getName() {
	return name;
}

QHostAddress RGBLightController::getAddress() {
	return address;
}

RGBLightConfig RGBLightController::getConfig() {
	return config;
}
