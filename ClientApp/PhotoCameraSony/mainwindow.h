#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "Widgets/StateButton.h"
#include <QProgressBar>
#include "Widgets/GalleryWidget.h"
#include "Widgets/ParametrWidget.h"
#include "Devices/IDevice.h"
#include "Devices/SonyPhotoCamera/Device.h"
#include <QCheckBox>
#include <QTime>
#include <QFile>
#include "ParametrManager.h"
#include <QTimer>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	void loadSettings();
	void saveSettings();
	void initGallery();
	void initStatusLabels();
	void initLayout();
	void initLabels();
	void connectDevice();
	MainWindow(const QString& dataPath, const QString& configPath, QWidget *parent = 0);
	void updateParametersWidgets();
	void initButton();
	void initProgressBar();
	void initParametrWidgets();
	void updateLiveStreamStatusLabel();
	~MainWindow();
	bool statusSimulator{ false };

private:
	QProgressBar proggresBarLoadImage;
	GalleryWidget* gallery;
	StateButton* mButton;
	StateButton* sdButton;
	StateButton* cameraButton;
	StateButton* getGalleryButton;
	StateButton* telemetryButton;
	StateButton* communicationsButton;
	StateButton* downloadFullSizeButton;
	StateButton* openDirectiryButton;
	StateButton* takePhotoButton;
	StateButton* zoomPlusButton;
	StateButton* zoomMinusButton;
	StateButton* liveModeButton;
	StateButton* clearListSendingPhotoButton;
	StateButton* autoDownloadFullsizePhotoButton;

	QCheckBox* checkBox;
	QCheckBox* checkBoxScrool;
	QCheckBox* checkBoxAutoSendPhotos;

	StateButton* startSeriaButton;
	QFile logFile;

	QPixmap microSdStatusOn;
	QPixmap microSdStatusOff;
	QPixmap telemetryStatusOn;
	QPixmap telemetryStatusOff;
	QPixmap communicationsStatusOn;
	QPixmap communicationsStatusStatusOff;
	QPixmap fotocameraStatusOn;
	QPixmap fotocameraStatusOff;
	QPixmap fotocameraStatusBusy;
	QPixmap fotocameraButtonBusy;
	QPixmap Download;
	QPixmap Folder;
	QPixmap Seria;
	QPixmap TakePhoto;
	QPixmap Gallery;
	QPixmap Red;
	QPixmap Green;
	QPixmap Yellow;
	QPixmap clean;
	QPixmap zoomIn;
	QPixmap zoomOut;
	QPixmap powerOn;
	QPixmap powerOff;
	QPixmap SeriaOn;

	QLabel* labelSd;
	QLabel* labelCamera;
	QLabel* labelStartStop;
	QLabel* labelTelemety;
	QLabel* labelCommunicatons;
	QLabel* labelTimer;
	QLabel* labelISO;
	QLabel* labelAperture;
	QLabel* labelShutter;
	QLabel* labelImageSize;
	QLabel* labelImageQuality;
	QLabel* labelZoom;
	QLabel* labelLive;
	
	ParametrWidget* iso;
	ParametrWidget* aperture;
	ParametrWidget* shutter;
	ParametrWidget* zoom;
	ParametrWidget* imageSize;
	ParametrWidget* imageQuality;

	QCheckBox checkbox;
	ParametrManager* isoParametrManager;
	ParametrManager* shutterParametrManager;
	ParametrManager* imageSizeParametrManager;
	ParametrManager* imageQualityParametrManager;
	QVBoxLayout *layoutButtonVerticalGroup2;
	QTimer* liveStreamRestartTimer;

	QTime time;
	QDate date;
	QSharedPointer<IDevice> device;
	QString lastPath;
	QString dataPath;
	QString configPath;
	

	

#ifdef WIN32
	bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

	unsigned int externPhotoSettingsMessage = 0;
	int bortId = 0;
	bool bortIdAdjusted{ false };
#endif

private slots:
	void onClicked();
	void onClickedSdStatus();
	void onClicked_Camera();
	void onClicked_Start();
	void onClicked_Telemetry();
	void onClicked_Communications();
	void onClickDownloadFullSize();
	void disconnectDevice();
	void onLoadingProgress(int);
	void onClicked_open_dir_button();
	void onPreview(QString, QPixmap);
	void onFullSize(QString, QByteArray);
	void onUpdatedSdStatusOn(bool);
	void onUpdatedGpsStatusOn(bool);
	void onUpdatedTelemetryStatusOn(bool);
	void onUpdatedIsoValue(QString);
	void onUpdatedImageSizeValue(QString);
	void onUpdatedImageQualityValue(QString);
	void onUpdatedApertureValue(QString);
	void onUpdatedShutterValue(QString);
	void onUpdatedZoomValue(QString);
	void onCheckBoxChecked();
	void onCheckBoxScroolChecked();
	void onCheckBoxAutoSendPhotosChecked();
	void onUpdatedAutoSendPhotos(bool);
	void onUserUpdateIso(QString);
	void onUserUpdateAperture(QString);
	void onUserUpdateShutter(QString);
	void onUserUpdateImageSize(QString);
	void onUserUpdateImageQuality(QString);
	void onUserUpdateZoom(QString);
	void onMainStatus(IDevice::MainStatus);
	void visibleSimulatorControlls(bool visible);
	void onClickedTakeShot();
	void onClickedSeria();
	void onClickedZoomPlus();
	void onClickedZoomMinus();
	void onClickedLiveModeButton();
	void seriaStatus(bool);
	void liveModeStatus(bool);
	void onUpdatedAvaliableIso(QList<QString>);
	void onUpdatedAvaliableShutter(QList<QString>);
	void onUpdatedAvaliableImageSize(QList<QString>);
	void onUpdatedAvaliableImageQuality(QList<QString>);
	void onLog(QString);
	void showLiveviewFrame(QPixmap);
	void onLiveStreamRestartTimer();
	void onClickedClearListSendingPhotoButton();
	void onClickedAutoDownloadFullsizePhotoButton();
	
};

#endif // MAINWINDOW_H
