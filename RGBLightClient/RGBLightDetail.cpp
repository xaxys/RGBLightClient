#include <QColorDialog>
#include <Windows.h>
#include "RGBLightDetail.h"

static const auto HTML_TEMPLATE = QStringLiteral(R"[===](
<p>
设备名: %1 <br/>
制造商: %2 <a href="%3">%3</a> <br/>
型号: %4 [ %5 ] <a href="%6">%6</a> <br/>
设备网页: <a href="%7">%7</a> <br/>
序列号: %8 <br/>
唯一标识符: %9 <br/>
</p>
)[===]");

RGBLightDetail::RGBLightDetail(const RGBLightConfig& config, QWidget* parent) : config(config), QWidget(parent) {
	ui.setupUi(this);
	auto info = new QTextDocument(this);
	info->setHtml(HTML_TEMPLATE.arg(config.name)
							  .arg(config.manufacturer)
							  .arg(config.manufacturerURL)
							  .arg(config.modelName)
							  .arg(config.modelNumber)
							  .arg(config.modelURL)
							  .arg(config.URLBase+config.presentationURL)
							  .arg(config.serialNumber)
							  .arg(config.UDN));
	ui.textBrowser->setDocument(info);
	ui.textBrowser->setOpenLinks(false);
	connect(ui.textBrowser, &QTextBrowser::anchorClicked, this, [](const QUrl& url) {
		ShellExecuteA(NULL, "open", url.toString().toStdString().c_str(), "", "", SW_SHOW);
	});
	// connect(&syncer, &QTimer::timeout, this, &RGBLightDetail::syncFromLight);
	// syncer.start(1000);
}

RGBLightDetail::~RGBLightDetail() {
	// syncer.stop();
}

void RGBLightDetail::onRedSliderChanged(int value) {
	setColor(QColor::fromRgb(value, color.green(), color.blue()));
}

void RGBLightDetail::onGreenSliderChanged(int value) {
	setColor(QColor::fromRgb(color.red(), value, color.blue()));
}

void RGBLightDetail::onBlueSliderChanged(int value) {
	setColor(QColor::fromRgb(color.red() , color.green(), value));
}

void RGBLightDetail::onBrightnessSliderChanged(int value) {
	setBrightness(value);
}

void RGBLightDetail::onTemperatureSliderChanged(int value) {
	setTemperature(value);
}

void RGBLightDetail::onColorLineEditChanged(QString str) {
	if (setColor(str)) {
		ui.lineEdit_color->setStyleSheet("color:black;");
	} else {
		ui.lineEdit_color->setStyleSheet("color:red;");
	}
}

void RGBLightDetail::onConstantButtonClicked() {
	setRGBMode(RGBMode::CONSTANT);
}

void RGBLightDetail::onBlinkButtonClicked() {
	setRGBMode(RGBMode::BLINK);
}

void RGBLightDetail::onBreathButtonClicked() {
	setRGBMode(RGBMode::BREATH);
}

void RGBLightDetail::onChaseButtonClicked() {
	setRGBMode(RGBMode::CHASE);
}

void RGBLightDetail::onRainbowButtonClicked() {
	setRGBMode(RGBMode::RAINBOW);
}

void RGBLightDetail::onStreamButtonClicked() {
	setRGBMode(RGBMode::STREAM);
}

void RGBLightDetail::onAnimationButtonClicked() {
	setRGBMode(RGBMode::ANIMATION);
}

void RGBLightDetail::onCustomButtonClicked() {
	setRGBMode(RGBMode::CUSTOM);
}

void RGBLightDetail::onReceiveMessage(QString msg) {
	auto list = msg.splitRef(" ");
	if (list[0] == "color") {
		setColor(list[1].toString());
	} else if (list[0] == "temperature") {
		setTemperature(list[1].toString());
	} else if (list[0] == "brightness") {
		setBrightness(list[1].toString());
	} else if (list[0] == "mode") {
		setRGBMode((RGBMode)list[1].toUInt());
	}
}

void RGBLightDetail::setColor(const QColor& color) {
	if (this->color == color) return;
	this->color = color;

	auto style = QString("background-color:rgb(%1,%2,%3);").arg(color.red()).arg(color.green()).arg(color.blue());
	ui.label_color->setStyleSheet(style);

	ui.horizontalSlider_red->setValue(color.red());
	ui.horizontalSlider_green->setValue(color.green());
	ui.horizontalSlider_blue->setValue(color.blue());

	ui.label_red->setText(QString::number(color.red()));
	ui.label_green->setText(QString::number(color.green()));
	ui.label_blue->setText(QString::number(color.blue()));

	QString hexColor = "#" + QString::number(color.rgb(), 16).right(6).toUpper();
	ui.lineEdit_color->setText(hexColor);

	emit commandExec(QString("color %1").arg(hexColor));
}

bool RGBLightDetail::setColor(const QString& str) {
	QStringRef hex = &str;
	if (str.startsWith("#")) hex = str.midRef(1);
	if (hex.length() == 6) {
		bool ok = false;
		if (QRgb rgb = hex.toUInt(&ok, 16); ok) {
			setColor(QColor::fromRgb(rgb));
			return true;
		}
	}
	return false;
}

void RGBLightDetail::setBrightness(int brightness) {
	if (this->brightness == brightness) return;
	this->brightness = brightness;
	ui.horizontalSlider_brightness->setValue(brightness);
	ui.label_brightness->setText(QString::number(brightness));
	emit commandExec(QString("brightness %1").arg(brightness));
}

bool RGBLightDetail::setBrightness(const QString& str) {
	bool ok = false;
	if (int num = str.toUInt(&ok); ok) {
		setBrightness(num);
		return true;
	}
	return false;
}

void RGBLightDetail::setTemperature(int temperature) {
	if (this->temperature == temperature) return;
	this->temperature = temperature;
	ui.horizontalSlider_temperature->setValue(temperature);
	ui.label_temprature->setText(QString::number(temperature));
	emit commandExec(QString("temperature %1").arg(temperature));
}

bool RGBLightDetail::setTemperature(const QString& str) {
	QStringRef text = &str;
	if (str.endsWith("K")) text.chop(1);
	bool ok = false;
	if (int num = text.toUInt(&ok); ok) {
		setTemperature(num);
		return true;
	}
	return false;
}

void RGBLightDetail::setRGBMode(RGBMode mode) {
	if (this->mode == mode) return;
	this->mode = mode;
	auto metaEnum = QMetaEnum::fromType<RGBLightDetail::RGBMode>();
	emit commandExec(QString("mode %1").arg(QString(metaEnum.valueToKey(mode)).toLower()));
}

void RGBLightDetail::syncFromLight() {
	emit commandExec("color");
	emit commandExec("brightness");
	emit commandExec("temperature");
	emit commandExec("mode");
}

void RGBLightDetail::onPickColorButtonClicked() {
    QColor color = QColorDialog::getColor(Qt::white, this, QStringLiteral("请选择颜色"));
	setColor(color);
}
