#pragma once
#include <QHostAddress>
#include <QWebSocket>
#include <QXmlStreamReader>

struct RGBLightConfig {
	QString name;
	QString UDN;
	QString serialNumber;
	QString URLBase;
	QString presentationURL;
	QString manufacturer;
	QString manufacturerURL;
	QString modelName;
	QString modelNumber;
	QString modelURL;

	static RGBLightConfig fromXml(QXmlStreamReader& xml);
};

class RGBLightController : public QObject {
	Q_OBJECT

	const QString name;
	const QHostAddress address;
	const RGBLightConfig config;
	const int port = 81;
	QWebSocket webSocket;

signals:
	void connected();
	void disconnected();
	void errored(QString);
	void received(QString);

private slots:
	void onConnected();
	void onDisconnected();
	void onWebSocketError(QAbstractSocket::SocketError);
	void onMessageReceived(QString);

public slots:
	void sendMessage(QString);

public:
	RGBLightController(const QHostAddress&, const RGBLightConfig&, QObject* parent = nullptr);
	~RGBLightController();
	QString getName();
	RGBLightConfig getConfig();
	QHostAddress getAddress();

private:
	void connectToLight();

};

