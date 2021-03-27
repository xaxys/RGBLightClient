#pragma once

#include <QWidget>
#include <QTimer>
#include <QMetaEnum>
#include "ui_RGBLightDetail.h"
#include "RGBLightController.h"

class RGBLightDetail : public QWidget {

public:
	enum RGBMode {
		CONSTANT,
		BLINK,
		BREATH,
		CHASE,
		RAINBOW,
		STREAM,
		ANIMATION,
		CUSTOM,
	};

private:
	Q_OBJECT
	Q_ENUM(RGBMode)
	Ui::RGBLightDetail ui;
	const RGBLightConfig config;
	RGBMode mode = RGBMode::CONSTANT;
	QColor color = QColor(255, 255, 255);
	int brightness = 255;
	int temperature = 6500;
	QTimer syncer;

signals:
	void commandExec(QString);

public slots:
	void onReceiveMessage(QString);

private slots:
	void onPickColorButtonClicked();
	void onRedSliderChanged(int);
	void onGreenSliderChanged(int);
	void onBlueSliderChanged(int);
	void onBrightnessSliderChanged(int);
	void onTemperatureSliderChanged(int);
	void onColorLineEditChanged(QString);
	void onConstantButtonClicked();
	void onBlinkButtonClicked();
	void onBreathButtonClicked();
	void onChaseButtonClicked();
	void onRainbowButtonClicked();
	void onStreamButtonClicked();
	void onAnimationButtonClicked();
	void onCustomButtonClicked();

public:
	RGBLightDetail(const RGBLightConfig&, QWidget *parent = nullptr);
	~RGBLightDetail();
	void setColor(const QColor& color);
	bool setColor(const QString& str);
	void setBrightness(int brightness);
	bool setBrightness(const QString& str);
	void setTemperature(int temperature);
	bool setTemperature(const QString& str);
	void setRGBMode(RGBMode mode);
	void syncFromLight();

};
