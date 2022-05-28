#pragma once
#include <QObject>
#include <QString>
#include <QPixmap>

class IDevice
{
public:
	virtual ~IDevice(){}

	virtual void requestPhotos() = 0 ;

	virtual void downloadFullSize(QString name) = 0;

	virtual void on() = 0;
	virtual void off() = 0;
	virtual bool isOn() const = 0;

	virtual bool isOnSd() const = 0;
	virtual bool isOnGps() const = 0;
	virtual bool isOnTelemetry() const = 0;
	virtual bool isLinkOnCamera() const = 0;
	
	virtual void takeShot() = 0;
	virtual void setSeriaMode() = 0;
	//virtual void setLiveStream() = 0;
	virtual void zoomMinus() = 0;
	virtual void zoomPlus() = 0;

	virtual void startLiveStream() = 0;
	virtual void stopLiveStream() = 0;
	virtual bool isOnLiveMode() const = 0;
	virtual void startAutoSendPhotos() = 0;
	virtual void stopAutoSendPhotos() = 0;

	virtual void clearListSendingPhoto() = 0;
	virtual void autoDownloadFullsizePhoto() = 0;

	virtual QList<QString> getIsoValues() const = 0;
	virtual QString getCurrentIso() const = 0;
	virtual void setCurrentIso(QString) = 0;

	virtual QList <QString > getApertureValues()const = 0;
	virtual QString getCurrentAperture() const = 0;
	virtual void setCurrentAperture(QString) = 0;

	virtual QList<QString> getShutterValues() const = 0;
	virtual QString getCurrentShutter() const = 0;
	virtual void setCurrentShutter(QString) = 0;

	virtual QList<QString> getImageSizeValues()const = 0;
	virtual  QString getCurrentImageSize() const = 0;
	virtual void setCurrentImageSize(QString) = 0;

	virtual QList<QString> getImageQualityValues()const = 0;
	virtual  QString getCurrentImageQuality() const = 0;
	virtual void setCurrentImageQuality(QString) = 0;

	virtual QList <QString> getZoomValues() const = 0;
	virtual QString getCurrentZoom()const = 0;
	virtual void setCurrentZoom(QString) = 0;

	enum class MainStatus
	{
		Offline,
		Busy,
		Ready
	};
	

	virtual MainStatus getMainStatus() const = 0;

signals:
	virtual void incomingPreview(QString, QPixmap) = 0;
	virtual void incomingFullSizeFoto(QString, QByteArray) = 0;
	virtual void updatedLoadingFullSize(int) = 0;

	virtual void incomingLiveStreamFoto(QPixmap) = 0;
	virtual void updatedLiveModeStatusOn(bool) = 0;

	virtual void updatedSdStatusOn(bool) = 0;
	virtual void updatedGpsStatusOn(bool) = 0;
	virtual void updatedTelemetryStatusOn(bool) = 0;
	virtual void updatedIsoValue(QString) = 0;
	virtual void updatedAvaliableIso(QList<QString>) = 0;
	virtual void updatedAvaliableImageSize(QList<QString>) = 0;
	virtual void updatedAvaliableImageQuality(QList<QString>) = 0;
	virtual void updatedAvaliableShutter(QList<QString>) = 0;
	virtual void updatedApertureValue(QString) = 0;
	virtual void updatedShutterValue(QString) = 0;
	virtual void updatedImageSizeValue(QString) = 0;
	virtual void updatedImageQualityValue(QString) = 0;
	virtual void updatedZoomValue(QString) = 0;
	virtual void updatedMainStatus(IDevice::MainStatus) = 0;
	virtual void updatedSeriaStatus(bool) = 0;
	virtual void log(QString) = 0;
};

Q_DECLARE_INTERFACE(IDevice, "IDevice")
Q_DECLARE_METATYPE(IDevice::MainStatus)
Q_DECLARE_METATYPE(QList<QString>)