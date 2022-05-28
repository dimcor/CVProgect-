#include "mainwindow.h"
#include <QHBoxLayout>
#include "Widgets/StateButton.h"
#include <QPainter>
#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include "Widgets/ParametrWidget.h"
#include <QGridLayout>
#include <QSettings>
#include <QDir>
#include <QDesktopServices>
#include "Exif/ExifReader.h"
#include "Widgets/GalleryWidget.h"

#include "Devices/SonyPhotoCamera/Device.h"
#include <QCheckBox>
#include "Devices/DeviceFactory.h"
#include <QDateTime>
#include <complex>
#include <valarray>
#include <QString>
#include "Common.h"
#include <QBuffer>
#include <QFileDialog>

MainWindow::MainWindow(const QString& dataPath, const QString& configPath, QWidget *parent)
	: QMainWindow(parent),
	logFile(configPath + "\\" + "logFile.txt"),
	dataPath(dataPath),
	configPath(configPath)
	
{
	if (dataPath.size() && dataPath.right(1) != "\\" && dataPath.right(1) != "/")
	{
		this->dataPath += '/';
	}
	
	if (configPath.size() && configPath.right(1) != "\\" && configPath.right(1) != "/")
	{
		this->configPath += '/';
	}

	

	liveStreamRestartTimer = new QTimer(this);

	logFile.open(QIODevice::WriteOnly | QIODevice::Text);
	loadSettings(); 

	initGallery();
	initButton();
	initProgressBar();
	initParametrWidgets();
	initStatusLabels();
	initLabels();
	initLayout();

	connect(sdButton, SIGNAL(clicked()), this, SLOT(onClickedSdStatus()));
	connect(cameraButton, SIGNAL(clicked()), this, SLOT(onClicked_Camera()));
	connect(getGalleryButton, SIGNAL(clicked()), this, SLOT(onClicked_Start()));
	connect(telemetryButton, SIGNAL(clicked()), this, SLOT(onClicked_Telemetry()));
	connect(communicationsButton, SIGNAL(clicked()), this, SLOT(onClicked_Communications()));
	connect(downloadFullSizeButton, SIGNAL(clicked()), this, SLOT(onClickDownloadFullSize()));
	connect(openDirectiryButton,SIGNAL(clicked()),this,SLOT(onClicked_open_dir_button()));
	connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxChecked()));
	connect(checkBoxScrool, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxScroolChecked()));

	connect(clearListSendingPhotoButton, SIGNAL(clicked()), this, SLOT(onClickedClearListSendingPhotoButton()));
	connect(checkBoxAutoSendPhotos, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxAutoSendPhotosChecked()));
	connect(autoDownloadFullsizePhotoButton, SIGNAL(clicked()), this, SLOT(onClickedAutoDownloadFullsizePhotoButton()));

	connect(takePhotoButton, SIGNAL(clicked()), this, SLOT (onClickedTakeShot()));
	connect(startSeriaButton, SIGNAL(clicked()), this, SLOT(onClickedSeria()));
	connect(zoomPlusButton, SIGNAL(clicked()),this, SLOT (onClickedZoomPlus()));
	connect(zoomMinusButton, SIGNAL(clicked()), this, SLOT(onClickedZoomMinus()));
	connect(liveModeButton, SIGNAL(clicked()), this, SLOT(onClickedLiveModeButton()));
	connect(liveStreamRestartTimer, SIGNAL(timeout()), this, SLOT(onLiveStreamRestartTimer()));

	device = DeviceFactory::create(!statusSimulator);
	checkBox->setChecked(statusSimulator);
	connectDevice();
	
#ifdef WIN32
	externPhotoSettingsMessage = ::RegisterWindowMessageA("Extern photo settings");
#endif
}

void MainWindow::updateParametersWidgets()
{
	iso->setValues(device->getIsoValues());
	iso->setValue(device->getCurrentIso());
	shutter->setValues(device->getShutterValues());
	shutter->setValue(device->getCurrentShutter());
	aperture->setValues(device->getApertureValues());
	aperture->setValue(device->getCurrentAperture());
	zoom->setValues(device->getZoomValues());
	zoom->setValue(device->getCurrentZoom());
	imageSize->setValues(device->getImageSizeValues());
	imageSize->setValue(device->getCurrentImageSize());
	imageQuality->setValues(device->getImageQualityValues());
	imageQuality->setValue(device->getCurrentImageQuality());
}

void MainWindow::connectDevice()
{
	auto deviceQObject = dynamic_cast<QObject*>(device.data());
	connect(deviceQObject, SIGNAL(updatedLoadingFullSize(int)), this, SLOT(onLoadingProgress(int)));
	connect(deviceQObject, SIGNAL(incomingPreview(QString, QPixmap)), this, SLOT(onPreview(QString, QPixmap)));
	connect(deviceQObject, SIGNAL(incomingFullSizeFoto(QString, QByteArray)), this, SLOT(onFullSize(QString, QByteArray)));
	connect(deviceQObject, SIGNAL(updatedSdStatusOn(bool)), this, SLOT(onUpdatedSdStatusOn(bool)));
	onUpdatedSdStatusOn(device->isOnSd());
	connect(deviceQObject, SIGNAL(updatedGpsStatusOn(bool)), this, SLOT(onUpdatedGpsStatusOn(bool)));
	connect(deviceQObject, SIGNAL(updatedTelemetryStatusOn(bool)), this, SLOT(onUpdatedTelemetryStatusOn(bool)));
	onUpdatedTelemetryStatusOn(device->isOnTelemetry());
	connect(deviceQObject, SIGNAL(updatedIsoValue(QString)), this, SLOT(onUpdatedIsoValue(QString)));
	connect(deviceQObject, SIGNAL(updatedApertureValue(QString)), this, SLOT(onUpdatedApertureValue(QString)));
	connect(deviceQObject, SIGNAL(updatedShutterValue(QString)), this, SLOT(onUpdatedShutterValue(QString)));
	connect(deviceQObject, SIGNAL(updatedImageSizeValue(QString)), this, SLOT(onUpdatedImageSizeValue(QString)));
	connect(deviceQObject, SIGNAL(updatedImageQualityValue(QString)), this, SLOT(onUpdatedImageQualityValue(QString)));
	connect(deviceQObject, SIGNAL(updatedZoomValue(QString)), this, SLOT(onUpdatedZoomValue(QString)));
	qRegisterMetaType<IDevice::MainStatus>();
	qRegisterMetaType<QList<QString>>();
	connect(deviceQObject, SIGNAL(updatedMainStatus(IDevice::MainStatus)), this, SLOT(onMainStatus(IDevice::MainStatus)));
	connect(deviceQObject, SIGNAL(updatedAvaliableIso(QList<QString>)), this, SLOT(onUpdatedAvaliableIso(QList<QString>)));
	connect(deviceQObject, SIGNAL(updatedAvaliableShutter(QList<QString>)), this, SLOT(onUpdatedAvaliableShutter(QList<QString>)));
	connect(deviceQObject, SIGNAL(updatedAvaliableImageSize(QList<QString>)), this, SLOT(onUpdatedAvaliableImageSize(QList<QString>)));
	connect(deviceQObject, SIGNAL(updatedAvaliableImageQuality(QList<QString>)), this, SLOT(onUpdatedAvaliableImageQuality(QList<QString>)));
	connect(deviceQObject, SIGNAL(log(QString)), this, SLOT(onLog(QString)));
	connect(deviceQObject, SIGNAL(updatedIsoValue(QString)), isoParametrManager, SLOT(deviceUpdateValue(QString)));
	connect(deviceQObject, SIGNAL(updatedShutterValue(QString)), shutterParametrManager, SLOT(deviceUpdateValue(QString)));
	connect(deviceQObject, SIGNAL(updatedImageSizeValue(QString)), imageSizeParametrManager, SLOT(deviceUpdateValue(QString)));
	connect(deviceQObject, SIGNAL(updatedImageQualityValue(QString)), imageQualityParametrManager, SLOT(deviceUpdateValue(QString)));
	connect(deviceQObject, SIGNAL(updatedSeriaStatus(bool)), this, SLOT(seriaStatus(bool)));
	connect(deviceQObject, SIGNAL(updatedLiveModeStatusOn(bool)), this, SLOT(liveModeStatus(bool)));
	connect(deviceQObject, SIGNAL(updatedAutoSendPhotos(bool)), this, SLOT(onUpdatedAutoSendPhotos(bool)));
	connect(deviceQObject, SIGNAL(incomingLiveStreamFoto(QPixmap)), this, SLOT(showLiveviewFrame(QPixmap)));

	updateParametersWidgets();
}
void MainWindow::disconnectDevice()
{
	auto deviceQObject = dynamic_cast<QObject*>(device.data());
	disconnect(deviceQObject, SIGNAL(updatedLoadingFullSize(int)), this, SLOT(onLoadingProgress(int)));
	disconnect(deviceQObject, SIGNAL(incomingPreview(QString, QPixmap)), this, SLOT(onPreview(QString, QPixmap)));
	disconnect(deviceQObject, SIGNAL(incomingFullSizeFoto(QString, QByteArray)), this, SLOT(onFullSize(QString, QByteArray)));
	disconnect(deviceQObject, SIGNAL(updatedSdStatusOn(bool)), this, SLOT(onUpdatedSdStatusOn(bool)));
	disconnect(deviceQObject, SIGNAL(updatedGpsStatusOn(bool)), this, SLOT(onUpdatedGpsStatusOn(bool)));
	disconnect(deviceQObject, SIGNAL(updatedTelemetryStatusOn(bool)), this, SLOT(onUpdatedTelemetryStatusOn(bool)));
	disconnect(deviceQObject, SIGNAL(updatedIsoValue(QString)), this, SLOT(onUpdatedIsoValue(QString)));
	disconnect(deviceQObject, SIGNAL(updatedApertureValue(QString)), this, SLOT(onUpdatedApertureValue(QString)));
	disconnect(deviceQObject, SIGNAL(updatedShutterValue(QString)), this, SLOT(onUpdatedShutterValue(QString)));
	disconnect(deviceQObject, SIGNAL(updatedImageSizeValue(QString)), this, SLOT(onUpdatedImageSizeValue(QString)));
	disconnect(deviceQObject, SIGNAL(updatedImageQualityValue(QString)), this, SLOT(onUpdatedImageQualityValue(QString)));
	disconnect(deviceQObject, SIGNAL(updatedZoomValue(QString)), this, SLOT(onUpdatedZoomValue(QString)));
	disconnect(deviceQObject, SIGNAL(updatedMainStatus(IDevice::MainStatus)), this, SLOT(onMainStatus(IDevice::MainStatus)));
	disconnect(deviceQObject, SIGNAL(updatedAvaliableIso(QList<QString>)), this, SLOT(onUpdatedAvaliableIso(QList<QString>)));
	disconnect(deviceQObject, SIGNAL(updatedAvaliableShutter(QList<QString>)), this, SLOT(onUpdatedAvaliableShutter(QList<QString>)));
	disconnect(deviceQObject, SIGNAL(updatedAvaliableImageSize(QList<QString>)), this, SLOT(onUpdatedAvaliableImageSize(QList<QString>)));
	disconnect(deviceQObject, SIGNAL(updatedImageQualityValue(QString)), this, SLOT(onUpdatedImageQualityValue(QString)));
	disconnect(deviceQObject, SIGNAL(log(QString)), this, SLOT(onLog(QString)));
	disconnect(deviceQObject, SIGNAL(updatedIsoValue(QString)), isoParametrManager, SLOT(deviceUpdateValue(QString)));
	disconnect(deviceQObject, SIGNAL(updatedShutterValue(QString)), shutterParametrManager, SLOT(deviceUpdateValue(QString)));
	disconnect(deviceQObject, SIGNAL(updatedImageSizeValue(QString)), imageSizeParametrManager, SLOT(deviceUpdateValue(QString)));
	disconnect(deviceQObject, SIGNAL(updatedImageQualityValue(QString)), imageQualityParametrManager, SLOT(deviceUpdateValue(QString)));
	disconnect(deviceQObject, SIGNAL(updatedSeriaStatus(bool)), this, SLOT(seriaStatus(bool)));
	disconnect(deviceQObject, SIGNAL(updatedLiveModeStatusOn(bool)), this, SLOT(liveModeStatus(bool)));
	connect(deviceQObject, SIGNAL(updatedAutoSendPhotos(bool)), this, SLOT(onUpdatedAutoSendPhotos(bool)));
	disconnect(deviceQObject, SIGNAL(incomingLiveStreamFoto(QPixmap)), this, SLOT(showLiveviewFrame(QPixmap)));///
}

void MainWindow::initStatusLabels()
{
	labelSd = new QLabel("microSd_l");
	labelSd->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	labelSd->setPixmap(microSdStatusOff);
	labelSd->setMaximumHeight(32);
	labelTelemety = new QLabel("melemetry_status");
	labelTelemety->setPixmap(telemetryStatusOff);
	labelTelemety->setMaximumHeight(32);
	labelCommunicatons = new QLabel("communications_status");
	labelCommunicatons->setPixmap(communicationsStatusStatusOff);
	labelCommunicatons->setMaximumHeight(32);
	labelCamera = new QLabel("fotocamera_status");
	labelCamera->setPixmap(fotocameraStatusOff);
	labelCamera->setMaximumHeight(32);
	
	
}

void MainWindow::initLayout()
{
	QVBoxLayout *layoutMain = new QVBoxLayout;
	QHBoxLayout *layoutStatus = new QHBoxLayout;
	QVBoxLayout *layoutButtonVerticalGroup1 = new QVBoxLayout;
	QVBoxLayout *layoutButtonVerticalGroup2 = new QVBoxLayout;
	QHBoxLayout *layoutButtonGroup = new QHBoxLayout;
	QHBoxLayout *layoutCheclBox = new QHBoxLayout;
	QHBoxLayout *layoutLiveSteram = new QHBoxLayout;
	QVBoxLayout *layoutWidget = new QVBoxLayout;
	QHBoxLayout *layoutIso = new QHBoxLayout;
	QHBoxLayout *layoutShutter = new QHBoxLayout;
	QHBoxLayout *layoutImageSize = new QHBoxLayout;
	QHBoxLayout *layoutImageQuality = new QHBoxLayout;
	QHBoxLayout *layoutZoom = new QHBoxLayout;
	QHBoxLayout *layoutButton = new QHBoxLayout;
	
	layoutMain->addLayout(layoutButton);
	layoutMain->addLayout(layoutWidget);
	layoutMain->addLayout(layoutButtonGroup);
	layoutMain->addLayout(layoutLiveSteram);
	layoutMain->addLayout(layoutCheclBox);
	layoutMain->addWidget(gallery);
	layoutMain->addLayout(layoutStatus);

	layoutButtonGroup->addLayout(layoutButtonVerticalGroup1);
	layoutButtonGroup->addLayout( layoutButtonVerticalGroup2);
	layoutCheclBox->addWidget(checkBoxScrool);
	layoutCheclBox->addWidget(checkBoxAutoSendPhotos);
	layoutCheclBox->addWidget(downloadFullSizeButton);
	layoutCheclBox->addWidget(openDirectiryButton);
	

	layoutStatus->addWidget(labelCamera);
	layoutStatus->addWidget(labelSd);
	layoutStatus->addWidget(labelTelemety);
	layoutStatus->addWidget(labelCommunicatons);
	layoutStatus->setSizeConstraint(QLayout::SetMinimumSize);
	layoutStatus->addWidget(&proggresBarLoadImage);
	layoutStatus->addStretch();

	layoutIso->addWidget(labelISO);
	layoutIso->addWidget(iso);
	layoutIso->addSpacing(30);

	layoutShutter->addWidget(labelShutter);
	layoutShutter->addWidget(shutter);
	layoutShutter->addSpacing(30);

	layoutImageSize->addWidget(labelImageSize);
	layoutImageSize->addWidget(imageSize);
	layoutImageSize->addSpacing(30);

	layoutImageQuality->addWidget(labelImageQuality);
	layoutImageQuality->addWidget(imageQuality);
	layoutImageQuality->addSpacing(30);

	layoutZoom->addWidget(labelZoom);
	
	layoutZoom->addWidget(zoomMinusButton, Qt::AlignCenter);
	layoutZoom->addWidget(zoomPlusButton, Qt::AlignCenter);
	//layoutZoom->addSpacing(50);
	layoutZoom->addStretch();
	layoutZoom->addWidget(checkBox);
	
	layoutWidget->addLayout(layoutIso);
	layoutWidget->addLayout(layoutShutter);
	layoutWidget->addLayout(layoutImageSize);
	layoutWidget->addLayout(layoutImageQuality);
	layoutWidget->addLayout(layoutZoom);

	layoutLiveSteram->addWidget(labelLive);
	
	layoutButton->addWidget(cameraButton);
	
	//layoutButton->addStretch();
	
	//layoutButtonVerticalGroup1->addWidget(clearListSendingPhotoButton);//
	layoutButtonVerticalGroup2->addWidget(sdButton);
	layoutButton->addWidget(getGalleryButton);
	layoutButton->addWidget(takePhotoButton);
	layoutButton->addWidget(startSeriaButton);
	layoutButton->addWidget(clearListSendingPhotoButton);
	//layoutButton->addStretch();
	//layoutButtonVerticalGroup2->addWidget(autoDownloadFullsizePhotoButton);//
	layoutButtonVerticalGroup2->addWidget(communicationsButton);
	layoutButtonVerticalGroup2->addWidget(telemetryButton);

	QWidget *window = new QWidget();
	window->setLayout(layoutMain);
	setCentralWidget(window);
}

void MainWindow::initLabels()
{ 
	labelISO = new QLabel(this);
	labelISO->setText("ISO");
	labelISO->setFixedWidth(60);
	labelAperture = new QLabel(this);
	labelAperture->setText(QString::fromLocal8Bit(""));
	labelAperture->setVisible(false);
	labelShutter = new QLabel(this);
	labelShutter->setText(QString::fromLocal8Bit("Затвор "));
	labelShutter->setFixedWidth(60);
	labelImageSize = new QLabel(this);
	labelImageSize->setFixedWidth(60);
	labelImageSize->setText(QString::fromLocal8Bit("Разрешение "));

	labelImageQuality = new QLabel(this);
	labelImageQuality->setFixedWidth(60);
	labelImageQuality->setText(QString::fromLocal8Bit("Качество "));

	labelZoom = new QLabel(this);
	labelZoom->setText(QString::fromLocal8Bit("Увеличение "));
	labelZoom->setFixedWidth(60);
	//labelZoom->setVisible(false);
	labelLive = new QLabel(this);
	labelLive->setFixedSize(256, 180);
	labelLive->setStyleSheet("QLabel {"
		"border-style: solid;"
		"border-width: 2px;"
		"border-color: gray; "
		"}");
	
}

void MainWindow::initParametrWidgets()
{
	iso = new ParametrWidget();

	isoParametrManager = new ParametrManager(iso);
	connect(isoParametrManager, SIGNAL(needUpdateValue(QString)), this, SLOT(onUserUpdateIso(QString)));

	aperture = new ParametrWidget();
	
	connect(aperture, SIGNAL(needUpdateValue(QString)), this, SLOT(onUserUpdateAperture(QString)));
	aperture->setVisible(false);

	shutter = new ParametrWidget();
	
	shutterParametrManager = new ParametrManager(shutter);
	connect(shutterParametrManager, SIGNAL(needUpdateValue(QString)), this, SLOT(onUserUpdateShutter(QString)));

	zoom = new ParametrWidget();
	
	connect(zoom, SIGNAL(needUpdateValue(QString)), this, SLOT(onUserUpdateZoom(QString)));
	zoom->setVisible(false);

	imageSize = new ParametrWidget();
	imageSizeParametrManager = new ParametrManager(imageSize);
	connect(imageSizeParametrManager, SIGNAL(needUpdateValue(QString)), this, SLOT(onUserUpdateImageSize(QString)));

	imageQuality = new ParametrWidget();
	imageQualityParametrManager = new ParametrManager(imageQuality);
	connect(imageQualityParametrManager, SIGNAL(needUpdateValue(QString)), this, SLOT(onUserUpdateImageQuality(QString)));
}

void MainWindow::initProgressBar()
{
	proggresBarLoadImage.setMaximum(100);
	proggresBarLoadImage.setMinimum(0);
	proggresBarLoadImage.setVisible(false);
}

void MainWindow::initButton()
{
	microSdStatusOn.load(":/main/icon/microSdStatusOn.png");
	microSdStatusOff.load( ":/main/icon/sdCardOff.png" );
	telemetryStatusOn.load( ":/main/icon/telemetryOn.png" );
	telemetryStatusOff.load(":/main/icon/telemetryOff.png");
	communicationsStatusOn.load( ":/main/icon/communicationsOn.png" );
	communicationsStatusStatusOff.load( ":/main/icon/communicationsOff.png" );
	fotocameraStatusOn.load( ":/main/icon/photocameraOn.png" );
	fotocameraStatusOff.load( ":/main/icon/photocameraOff.png");
	fotocameraStatusBusy.load( ":/main/icon/busy.png" );
	fotocameraButtonBusy.load( ":/main/icon/cameraBusy" );
	Download.load( ":/main/icon/download.png" );
	Folder.load( ":/main/icon/folder.png" );
	Seria.load( ":/main/icon/seria.png" );
	TakePhoto.load( ":/main/icon/takePhoto.png" );
	Gallery.load( ":/main/icon/gallery.png" );
	Red.load( ":/main/icon/red.png" );
	Green.load( ":/main/icon/green.png" );
	Yellow.load( ":/main/icon/yellow.png" );
	clean.load(":/main/icon/clean.png");
	zoomIn.load(":/main/icon/zoom-in.png");
	zoomOut.load(":/main/icon/zoom-out.png");
	powerOn.load(":/main/icon/power-on.png");
	powerOff.load(":/main/icon/power-off.png");
	SeriaOn.load(":/main/icon/seria_on.png");

	sdButton = new StateButton(microSdStatusOn, microSdStatusOff, QPixmap(), this); //
	sdButton->setMinimumHeight(30);
	sdButton->setText(QString::fromLocal8Bit("Сменить статус Sd"));
	sdButton->setState(StateButton::State::off);
	sdButton->setVisible(statusSimulator);
	cameraButton = new StateButton(powerOn, powerOff, powerOn, this);
	cameraButton->setFixedHeight(30);
	cameraButton->setMaximumWidth(100);
	cameraButton->setToolTip(QString::fromLocal8Bit("Камера вкл/выкл"));
	cameraButton->setState(StateButton::State::off);
	takePhotoButton = new StateButton(TakePhoto, TakePhoto, TakePhoto, this);
	
	takePhotoButton->setFixedHeight(30);
	takePhotoButton->setMaximumWidth(100);
	takePhotoButton->setToolTip(QString::fromLocal8Bit("Сделать фото"));
	// zoom button control 
	zoomPlusButton = new StateButton(zoomIn, zoomIn, zoomIn, this);
	
	zoomPlusButton->setFixedHeight(30);
	zoomPlusButton->setMaximumWidth(100);
	zoomPlusButton->setToolTip(QString::fromLocal8Bit("Приблизить")); 
	zoomMinusButton = new StateButton(zoomOut, zoomOut, zoomOut, this);
	
	zoomMinusButton->setFixedHeight(30);
	zoomMinusButton->setMaximumWidth(100);
	zoomMinusButton->setToolTip(QString::fromLocal8Bit("Отдалить"));

	liveModeButton = new StateButton(QPixmap(), QPixmap(), QPixmap(), this);//
	liveModeButton-> setFixedSize(50, 100);
	liveModeButton->setText(QString::fromLocal8Bit("Live"));
	liveModeButton->setVisible(false);

	getGalleryButton = new StateButton(Gallery, Gallery, Gallery, this);
	getGalleryButton->setFixedHeight(30);
	getGalleryButton->setMaximumWidth(100);
	getGalleryButton->setToolTip(QString::fromLocal8Bit("Галерея"));
	startSeriaButton = new StateButton(Seria, SeriaOn, Seria, this);
	startSeriaButton->setFixedHeight(30);
	startSeriaButton->setMaximumWidth(100);
	startSeriaButton->setToolTip(QString::fromLocal8Bit("Серийная съемка"));
	telemetryButton = new StateButton(telemetryStatusOn, telemetryStatusOff, QPixmap(), this); //
	telemetryButton->setMinimumHeight(30);
	telemetryButton->setText(QString::fromLocal8Bit("Телеметрия вкл/выкл"));
	telemetryButton->setState(StateButton::State::off);
	telemetryButton->setVisible(statusSimulator);
	communicationsButton = new StateButton(communicationsStatusOn, communicationsStatusStatusOff, QPixmap(), this);//
	communicationsButton->setMinimumHeight(30);
	communicationsButton->setText(QString::fromLocal8Bit("Связь вкл/выкл"));//
	communicationsButton->setState(StateButton::State::off);
	communicationsButton->setVisible(statusSimulator);
	downloadFullSizeButton = new StateButton(Download, Download, Download, this);
	downloadFullSizeButton->setFixedHeight(30);
	downloadFullSizeButton->setMaximumWidth(100);
	downloadFullSizeButton->setToolTip(QString::fromLocal8Bit("Скачать полноразмерное фото"));
	downloadFullSizeButton->setState(StateButton::State::off);
	openDirectiryButton = new StateButton(Folder, Folder, Folder, this);
	openDirectiryButton->setFixedHeight(30);
	openDirectiryButton->setMaximumWidth(100);
	openDirectiryButton->setToolTip(QString::fromLocal8Bit("Сохраненые фото"));

	clearListSendingPhotoButton = new StateButton(clean, clean, clean, this);
	clearListSendingPhotoButton->setToolTip(QString::fromLocal8Bit("Очистить"));
	clearListSendingPhotoButton->setFixedHeight(30);
	clearListSendingPhotoButton->setMaximumWidth(100);
	
	autoDownloadFullsizePhotoButton = new StateButton(QPixmap(), QPixmap(), QPixmap(), this);
	autoDownloadFullsizePhotoButton->setText(QString::fromLocal8Bit("Скачать все"));/// для загрузки на внещний диск 
	autoDownloadFullsizePhotoButton->setVisible(false);

	checkBox = new QCheckBox();
	checkBox->setText(QString::fromLocal8Bit("Режим симулятора"));
	checkBox->setEnabled(true);
	checkBox->setChecked(false);
	checkBoxScrool = new QCheckBox();
	checkBoxScrool->setText(QString::fromLocal8Bit("Автопрокрутка"));
	checkBoxScrool->setEnabled(true);
	checkBoxScrool->setChecked(false);

	checkBoxAutoSendPhotos = new QCheckBox();
	checkBoxAutoSendPhotos->setText(QString::fromLocal8Bit("Автозагрузка"));
	checkBoxAutoSendPhotos->setEnabled(true);
	checkBoxAutoSendPhotos->setChecked(false);

}

void MainWindow::saveSettings()
{
	QSettings settings(configPath + "settings.conf", QSettings::IniFormat);
	settings.beginGroup("windows");
	settings.setValue("x", x());
	settings.setValue("y", y());
	settings.setValue("width", width());
	settings.setValue("height", height());
	settings.setValue("lastPath", lastPath);
	settings.setValue("simulator", statusSimulator);
	settings.setValue("configPath", configPath);
	settings.endGroup();
}

MainWindow::~MainWindow()
{
	saveSettings();
	logFile.close();
}

void MainWindow::onClicked()
{
	int valueState = static_cast<int>(mButton->getState());
	if (++valueState > 2)
		valueState = 0;
	mButton->setState(static_cast<StateButton::State>(valueState));

	// temp
	for (auto selectedName : gallery->getSelectedItems())
	{
		device->downloadFullSize(selectedName);
	}
}

void MainWindow::onClickedSdStatus()
{
	
}

void MainWindow::onClicked_Camera()
{
	if (device->isOn())
	{
		device->off();
	}
	else
	{
		device->on();
	}
}

void MainWindow::onClickedTakeShot()
{
	device->takeShot();
}

void MainWindow::onClickedSeria()
{
	device->setSeriaMode();
}


void MainWindow::onClickedZoomPlus()
{
	device->zoomPlus();
}

void MainWindow::onClickedZoomMinus()
{
	device->zoomMinus();
}

void MainWindow::onClickedLiveModeButton()	
{
	/*if (device->isOnLiveMode())
	{
		device->stopLiveStream();
		labelLive->setVisible(false);
		labelLive->clear();
		
	}
	else
	{	
		labelLive->setVisible(true);
		device->startLiveStream();
		
	}*/
}


void MainWindow::showLiveviewFrame(QPixmap p)
{
	labelLive->clear();
	const auto scaledSize = getScaledImageSizeToElem(p.size(), QSize(200, 200));
	
	auto scaledImage = p.scaled(scaledSize.width(), scaledSize.height());
	auto w = scaledImage.width();
	auto h = scaledImage.height();

	labelLive->setPixmap(scaledImage);
	//QDir dir("C:/Development/Photo/PhotoSimulator/dataSamples/livestream/");
	//static int i = 0;

	//QFile file(QString::number(i)+".jpeg");
	//i++;
	//file.open(QIODevice::WriteOnly);
	//scaledImage.save(&file,"JPEG");



}

void MainWindow::seriaStatus(bool valueSeria)
{ 
	if (valueSeria)
	{
		startSeriaButton->setState(StateButton::State::off);
	}
	else
	{
		startSeriaButton->setState(StateButton::State::on);
	}
}


void MainWindow::liveModeStatus(bool valueLiveMode)
{
	updateLiveStreamStatusLabel();
}



void MainWindow::onClicked_Start()
{
	gallery->clearList();
	device->requestPhotos();
}

void MainWindow::onClicked_Telemetry()
{
	int valueState = static_cast<int>(telemetryButton->getState());
	if (++valueState > 1)
		valueState = 0;
	telemetryButton->setState(static_cast<StateButton::State>(valueState));

	if (telemetryButton->getState() == StateButton::State::on)
		labelTelemety->setPixmap(telemetryStatusOn);
	else
		labelTelemety->setPixmap(telemetryStatusOff);
}

void MainWindow::onClicked_Communications()
{
	int valueState = static_cast<int>(communicationsButton->getState());
	if (++valueState > 1)
		valueState = 0;
	communicationsButton->setState(static_cast<StateButton::State>(valueState));

	if (communicationsButton->getState() == StateButton::State::on)
		labelCommunicatons->setPixmap(communicationsStatusOn);
	else
		labelCommunicatons->setPixmap(communicationsStatusStatusOff);
}

void MainWindow::onClicked_open_dir_button()
{
	QString savePath = { "../incomingData" };
	QDir dir(lastPath);
	QDesktopServices::openUrl(QUrl::fromLocalFile(dir.absolutePath()));

}

void MainWindow::onClickedClearListSendingPhotoButton()
{
	device->clearListSendingPhoto();
	gallery->clearList();
}


void MainWindow::onClickedAutoDownloadFullsizePhotoButton()
{
	device->autoDownloadFullsizePhoto();
}

void MainWindow::onClickDownloadFullSize()
{
	for (auto item : gallery->getSelectedItems())
		device->downloadFullSize(item);
	proggresBarLoadImage.setVisible(true);
}

void MainWindow::onCheckBoxChecked()
{
	disconnectDevice();
	if (checkBox->isChecked())
	{
		statusSimulator = true;
	}
	else
	{
		statusSimulator = false;
	}
	const auto isSony = !checkBox->isChecked();
	visibleSimulatorControlls(!isSony);
	device = DeviceFactory::create(isSony);
	connectDevice();

	
	
}

void MainWindow::onCheckBoxScroolChecked()
{
	gallery->setAutoscroll(!gallery->isAutoscrollOn());
}


void MainWindow::onCheckBoxAutoSendPhotosChecked()
{
	if (checkBoxAutoSendPhotos->isChecked())
	{
		device->startAutoSendPhotos();
	}
	else
	{
		device->stopAutoSendPhotos();
	}
	
}


void MainWindow::onUpdatedAvaliableIso(QList<QString> val)
{
	iso->setValues(val);
}

void MainWindow::onUpdatedAvaliableShutter(QList<QString> val)
{
	shutter->setValues(val);
}

void MainWindow::onUpdatedAvaliableImageSize(QList<QString> val)
{
	imageSize->setValues(val);
}

void MainWindow::onUpdatedAvaliableImageQuality(QList<QString> val)
{
	imageQuality->setValues(val);
}


void MainWindow::onLoadingProgress(int value)
{
	proggresBarLoadImage.setValue(value);
	if (proggresBarLoadImage.maximum()== value)
		proggresBarLoadImage.setVisible(false);
}

void MainWindow::loadSettings()
{
	auto te = statusSimulator;
	QSettings settings(configPath + "settings.conf", QSettings::IniFormat);
	settings.beginGroup("windows");
	lastPath = settings.value("lastPath", lastPath).toString();
	statusSimulator = settings.value("simulator").toBool();
	configPath = settings.value("configPath", configPath).toString();
	int x = settings.value("x", -1).toInt();
	int y = settings.value("y", -1).toInt();
	int width = settings.value("width", -1).toInt();
	int height = settings.value("height", -1).toInt(); settings.endGroup();
	if (x > 0 && y > 0 && width > 0 && height > 0)
	{
		setGeometry(x, y, width, height);
		
	}
}



void MainWindow::initGallery()
{
	gallery = new GalleryWidget(this);
}

void MainWindow::onPreview(QString name, QPixmap preview)
{
	qDebug() << "incoming preview " << name ;;
	gallery->addImage(name, preview);
	
}

void MainWindow::onFullSize(QString name, QByteArray data)
{
	qDebug() << "incoming fullsize " << name;
	gallery->deleteImage(name);
	
	auto meta = ExifReader::getMetadata(data);
	qDebug() << meta.values();

	auto datatime = meta[ExifReader::ExifParam::OriginalDateTime];
	datatime.resize(10);
	datatime.replace(":", "_");

	QString extraDir, fileName;
	extraDir = datatime;
	fileName = name;

#ifdef WIN32
	static int fileNum = 10000000;
	if (bortIdAdjusted)
	{ // payload dir structure
		
		fileName = QString::number(fileNum) + ".jpg";
		//fileName = name.right(10); fileNum = fileName.left(fileName.size() - 4).toInt();
		extraDir = QString::number(bortId);
	}
#endif

	QDir dir(dataPath);

	if (!dir.exists(dataPath))
	{
		dir.mkdir(dataPath);
	}
	dir.cd(dataPath);
	if (!dir.exists(extraDir))
	{
		dir.mkdir(extraDir);
	}

	QFile f(dataPath + extraDir + '/' + fileName);
	if (f.exists())
	{
		f.remove();
	}
	f.open(QIODevice::WriteOnly);
	f.write(data);
	f.close();

#ifdef WIN32
	static HWND hWndDest = 0;
	
	if (!hWndDest || !::IsWindow(hWndDest))
	{
		hWndDest = ::FindWindowA(0, "Fly - Payload");
	}

	if (hWndDest)
	{
		static unsigned int message = ::RegisterWindowMessageA("Take jpeg");
		::PostMessageA(hWndDest, message, bortId, fileNum);
		//++fileNum;
	}
	else
	{
		qDebug() << "can`t send message";
	}
#endif

	lastPath = dataPath + extraDir +'/';
	qDebug() << lastPath << "LASTPATH";

#ifdef WIN32
	 ++fileNum;
#endif
}

void MainWindow::onUpdatedSdStatusOn(bool value)
{
	if (value)
	{
		labelSd->setPixmap(microSdStatusOn);
	}
	else
	{
		labelSd->setPixmap(microSdStatusOff);	
	}
}

void MainWindow::onUpdatedAutoSendPhotos(bool value)
{
	if (value)
	{
		
	}
}


void MainWindow::onUpdatedGpsStatusOn(bool on)
{
	StateButton::State state;
	if (on)
	{
		state = StateButton::State::on;
		labelCommunicatons->setPixmap(communicationsStatusOn);
	}
	else
	{
		state = StateButton::State::off;
		labelCommunicatons->setPixmap((communicationsStatusStatusOff));
	}
	communicationsButton->setState(state);
}

void MainWindow::onUpdatedTelemetryStatusOn(bool value)
{
	if (value )
	{	
		labelTelemety->setPixmap(telemetryStatusOn);
	}
	else
	{
		labelTelemety->setPixmap((telemetryStatusOff));
	}
	
}

void MainWindow::onUpdatedIsoValue(QString value)
{
	iso->setValue(value);
}

void MainWindow::onUserUpdateIso(QString iso)
{
	device->setCurrentIso(iso);
}

void MainWindow::onUpdatedApertureValue( QString value)
{
	aperture->setValue(value);
}

void MainWindow::onUserUpdateAperture(QString aperture)
{
	device->setCurrentAperture(aperture);
}

void MainWindow::onUpdatedShutterValue(QString value)
{
	shutter->setValue(value);
}

void MainWindow::onUserUpdateShutter(QString shutter)
{
	device->setCurrentShutter(shutter);
}

void MainWindow::onUpdatedImageSizeValue(QString value)
{
	imageSize->setValue(value);
}

void MainWindow::onUpdatedImageQualityValue(QString value)
{
	imageQuality->setValue(value);
}


void MainWindow::onUserUpdateImageSize(QString imageSize)
{
	device->setCurrentImageSize(imageSize);
}

void MainWindow::onUserUpdateImageQuality(QString imageQuality)
{
	device->setCurrentImageQuality(imageQuality);
}



void MainWindow::onUpdatedZoomValue(QString value)
{
	zoom->setValue(value);
}

void MainWindow::onUserUpdateZoom(QString zoom)
{
	device->setCurrentZoom(zoom);
}

void MainWindow::onMainStatus(IDevice::MainStatus value)
{
	if (value == IDevice::MainStatus::Ready)
	{
		labelCamera->setPixmap(fotocameraStatusOn);
		cameraButton->setState(StateButton::State::on);
	}
		
	if (value == IDevice::MainStatus::Busy)
	{
		labelCamera->setPixmap(fotocameraStatusBusy);
		cameraButton->setState(StateButton::State::disable);
	}

	if (value == IDevice::MainStatus::Offline)
	{
		labelCamera->setPixmap(fotocameraStatusOff);
		cameraButton->setState(StateButton::State::off);
		
		
	}

	if (!liveStreamRestartTimer->isActive() && (value == IDevice::MainStatus::Ready) && !device->isOnLiveMode())
	{
		liveStreamRestartTimer->start(500);

	}

	checkBox->setEnabled(value != IDevice::MainStatus::Busy);
}


void MainWindow::onLiveStreamRestartTimer()
{
	if (!device->isOnLiveMode())
	{
		device->startLiveStream();
	}
	else
	{
		liveStreamRestartTimer->stop();
	}
}

void MainWindow::updateLiveStreamStatusLabel()
{
	QString labelColor = "gray";
	//if (device->isOnLiveMode() && (device->getMainStatus()==IDevice::MainStatus::Ready))
	if (device->isOnLiveMode())

	{
		labelColor = "green";
	}
	labelLive->setStyleSheet(QString("QLabel {"
		"border-style: solid;"
		"border-width: 2px;"
		"border-color: %1; "
		"}").arg(labelColor));
}

void MainWindow::visibleSimulatorControlls(bool visible)
{	
	sdButton->setVisible(visible);
	telemetryButton->setVisible(visible);
	communicationsButton->setVisible(visible);
	//getGalleryButton->setVisible(visible);
	takePhotoButton->setVisible(!visible);
	startSeriaButton->setVisible(!visible);

}


QDataStream& operator<<(const QTextStream& out, const QDate& rhs);

void MainWindow::onLog(QString text)
{
	time = QTime::currentTime();
	date = QDate::currentDate();
	
	if (logFile.isOpen())
	{
		auto textStr = QString(date.toString() + ' ' + time.toString() + ' ' + text + '\n');
		logFile.write(textStr.toUtf8());
		logFile.flush();
	}
	
}

#ifdef WIN32
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	MSG* msg = reinterpret_cast<MSG*>(message);
	if (msg->message == WM_DESTROY || msg->message == WM_NCDESTROY)
	{
		close();
		return true;
	}
	else if (msg->message == WM_KEYDOWN && (msg->wParam == VK_F11 || msg->wParam == VK_ESCAPE))
	{
		HWND hWnd = reinterpret_cast<HWND>(winId());
		if (hWnd)
		{
			HWND hWndParent = ::GetAncestor(hWnd, GA_PARENT);
			if (hWndParent)
			{
				::PostMessageA(hWndParent, msg->message, msg->wParam, msg->lParam);
				return true;
			}
		}
	}
	else if (externPhotoSettingsMessage && externPhotoSettingsMessage == msg->message)
	{
		enum class ExternMessageType {SetBortId = 1};
		switch ((ExternMessageType)msg->wParam)
		{
		case ExternMessageType::SetBortId :
			bortId = (int)msg->lParam;
			bortIdAdjusted = true;
			break;

		default:
			break;
		}
		return true;
	}
	//qDebug() << "msg " << msg->message << " wParam " << msg->wParam << " lParam " << msg->lParam;
	return false;
}
#endif
