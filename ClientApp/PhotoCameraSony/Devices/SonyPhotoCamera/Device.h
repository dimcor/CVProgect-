#pragma once
#include "../IDevice.h"
#include "../../Third-party/SonyPhotoCamera/SonyPhotoCameraClientLib/iRpiSonyCameraClient.h"
#include "IClientLibObject.h"

class Device : public QObject, public IDevice
{
	Q_OBJECT
	Q_INTERFACES(IDevice)

public:
	Device(QObject* parent = nullptr);
	void setClientLibInterface(QSharedPointer<IClientLibObject>);

	void requestPhotos();
	virtual ~Device();

	void downloadFullSize(QString name) override;

	void on() override;
	void off() override;
	bool isOn() const override;
	bool isOnSd() const override;
	bool isOnGps() const override;
	bool isOnTelemetry() const override;
	bool isLinkOnCamera() const override;
	MainStatus getMainStatus() const override;
	void takeShot() override;
	void setSeriaMode() override;
	//void setLiveStream() override;
	void zoomMinus() override;
	void zoomPlus() override;

	void startLiveStream() override;
	void stopLiveStream() override;
	bool isOnLiveMode() const override;

	void startAutoSendPhotos() override;
	void stopAutoSendPhotos() override;
	void clearListSendingPhoto() override;
	void autoDownloadFullsizePhoto() override;

	QList<QString> getIsoValues() const override;
	QString getCurrentIso() const override;
	void setCurrentIso(QString) override;

	QList < QString > getApertureValues()const override;
	QString getCurrentAperture() const override;
	void setCurrentAperture(QString) override;

	QList<QString> getShutterValues() const override;
	QString getCurrentShutter() const override;
	void setCurrentShutter(QString) override;

	QList < QString > getZoomValues() const override;
	QString getCurrentZoom()const override;
	void setCurrentZoom(QString) override;

	QList<QString> getImageSizeValues() const override;
	QString getCurrentImageSize() const override;
	void setCurrentImageSize(QString) override;

	QList<QString> getImageQualityValues() const override;
	QString getCurrentImageQuality() const override;
	void setCurrentImageQuality(QString) override;
	
private:
	QStringList splitValues(const QString& s);

	enum class ProtocolStatus
	{
		PowerOn,
		ClientConnected,
		SeriaMode,
		CameraConnected,
		ModuleStatus,
		TelemetryAvaliable,
		SendingPhotos,
		Sd,
		saveProgress,
		isoList,
		shutterList,
		cameraIso,
		cameraShutter,
		cameraLiveMode,
		imageSizeList,
		cameraImageSize,
		imageQualityList,
		cameraImageQuality
	};
	void setStatusFromCallback(ProtocolStatus status, QVariant value);

signals:
	void incomingPreview(QString, QPixmap);
	void incomingFullSizeFoto(QString, QByteArray);
	void updatedLoadingFullSize(int);

	void incomingLiveStreamFoto(QPixmap);
	void updatedLiveModeStatusOn(bool);

	void updatedAutoSendPhotos(bool);


	void updatedStatusOn(bool);
	void updatedSdStatusOn(bool);

	void updatedGpsStatusOn(bool);
	void updatedTelemetryStatusOn(bool);

	void updatedIsoValue(QString);
	void updatedImageSizeValue(QString);
	void updatedImageQualityValue(QString);
	void updatedApertureValue(QString);
	void updatedShutterValue(QString);
	void updatedZoomValue(QString);
	void updatedMainStatus(IDevice::MainStatus) override;
	void updatedAvaliableIso(QList<QString>) override;
	void updatedAvaliableImageSize(QList<QString>) override;
	void updatedAvaliableImageQuality(QList<QString>) override;
	void updatedAvaliableShutter(QList<QString>) override;
	void updatedSeriaStatus(bool) override;
	void log(QString) override;

private slots:
	void onDataReceived(DataType, QString);

private:
	void sendCommand(Command cmd, QByteArray data);

	MainStatus status{ MainStatus::Offline };

	enum PowerStatus
	{
		False,
		True,
		Switching
	};
	enum ModuleState
	{
		Idle,
		Ready,
		Steady
	};

	PowerStatus powerStatus{ False };
	ModuleState moduleState{ Idle };
	bool telemetryOn{ false };
	bool sdOn{ false };
	bool seriaOn{ false };
	bool sendPhoto{ false };
	bool liveMode{ false };
	bool autoSendPhotosFlag{ false };
	int progressSave{0};
	QList<QString> isoValues{};
	QList<QString> imageSizeListValues{};
	QList<QString> imageQualityListValues{};
	QList<QString> shutterValues{};
	QString currentCameraiso{};
	QString currentCameraImageSize{};
	QString currentCameraImageQuality{};
	QString currentCameraShutter{};

	QSharedPointer<IClientLibObject> clientLibObject;
};







