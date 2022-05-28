#include "Device.h"
#include <QDebug>
#include "../../Third-party/SonyPhotoCamera/SonyPhotoCameraClientLib/typedefs.h"
#include "../../Third-party/SonyPhotoCamera/utils/convertUtils.h"
#include <filesystem>
#include "mainwindow.h"

const QString statusCameraPower("CameraPower");
const QString statusCameraSerie("CameraSerie");
const QString statusCameraConnected("CameraConnected");
const QString statusNewModuleStatus("NewModuleStatus");
const QString statusCameraTelemetry("CameraTelemetry");
const QString statusServerSdCard("ExternalStorageStatus");
const QString valueFalse("False");
const QString valueTrue("True");
const QString valueSwitching("Switching");
const QString valueIdle("Idle");
const QString valueReady("Ready");
//const QString RequestPhotosReadyToSendList("RequestPhotosReadyToSendList");
const QString valueNotFound("NotFound");
const QString downloadProgress("DownloadProgress");
const QString availibleIsolist("AvailableISOList");
const QString availibleShutterList("AvailableShutterList");
const QString cameraISO("CameraISO");
const QString cameraShutter("CameraShutter");
const QString cameraLiveModeStatus("CameraLiveview"); 
const QString cameraAutoSendPhotosStatus("AutoSendPhotos");
const QString availibleImageSizelist("AvailableImageSizeList");
const QString cameraImageSize("CameraImageSize");
const QString availibleImageQualityList("AvailableImageQtyList");
const QString cameraImageQuality("CameraImageQuality");


void Device::onDataReceived(DataType _dataType, QString data)
{
	const auto dataType = static_cast<DataType>(_dataType);
	qDebug() << "callback dataUpdated";
	const auto jsonObj = QJsonDocument::fromJson(data.toLocal8Bit()).object();

	// лог
	auto logDocumentObj = jsonObj;
	for (const auto& key : logDocumentObj.keys())
	{
		if (key.contains("DataBase64"))
		{
			logDocumentObj[key] = "...";
		}
	}
	emit log("modul response: " + QString(QJsonDocument(logDocumentObj).toJson()));

	if (!jsonObj.isEmpty())
	{
		switch (dataType)
		{
		case Status:
			if (jsonObj.keys().size() > 0)
			{
				foreach(QString key, jsonObj.keys())
				{
					QString val = jsonObj.value(key).toString();
					QString value = jsonObj.value(key).toInt();
					qDebug() << key << "=" << val;
					
					if (key == statusNewModuleStatus)
					{
						setStatusFromCallback(Device::ProtocolStatus::ModuleStatus, val);
					}
					if (key == statusCameraPower)
					{
						setStatusFromCallback(Device::ProtocolStatus::PowerOn, val);
					}
					if (key == statusCameraTelemetry)
					{
						setStatusFromCallback(Device::ProtocolStatus::TelemetryAvaliable, val == valueTrue);
					}
					if (key == statusServerSdCard)
					{
						setStatusFromCallback(Device::ProtocolStatus::Sd, val != valueNotFound);
					}
					if (key == statusCameraSerie)
					{
						setStatusFromCallback(Device::ProtocolStatus::SeriaMode, val);
					}
					if (key == downloadProgress)
					{
						setStatusFromCallback(Device::ProtocolStatus::saveProgress, val.toInt());
					}
					if (key == availibleIsolist)
					{
						setStatusFromCallback(Device::ProtocolStatus::isoList, val);
					}

					if (key == availibleShutterList)
					{
						setStatusFromCallback(Device::ProtocolStatus::shutterList, val);
					}
					if (key == cameraISO)
					{
						setStatusFromCallback(Device::ProtocolStatus::cameraIso, val);
					}
					if (key == cameraShutter)
					{
						setStatusFromCallback(Device::ProtocolStatus::cameraShutter, val);
					}
					if (key == cameraLiveModeStatus)
					{
						setStatusFromCallback(Device::ProtocolStatus::cameraLiveMode, val);
					}
					if (key == cameraAutoSendPhotosStatus )
					{
						setStatusFromCallback(Device::ProtocolStatus::SendingPhotos, val);
					}
					if (key == statusCameraConnected)
					{
						setStatusFromCallback(Device::ProtocolStatus::CameraConnected, val);
					}
					if (key == availibleImageSizelist)
					{
						setStatusFromCallback(Device::ProtocolStatus::imageSizeList, val);
					}
					if (key == cameraImageSize)
					{
						setStatusFromCallback(Device::ProtocolStatus::cameraImageSize, val);
					}
					if (key == availibleImageQualityList)
					{
						setStatusFromCallback(Device::ProtocolStatus::imageQualityList, val);
					}
					if (key == cameraImageQuality)
					{
						setStatusFromCallback(Device::ProtocolStatus::cameraImageQuality, val);
					}

				}
			}
			break;

		case PhotoThumbnail:
		{
			if (jsonObj.contains("ThumbName") && jsonObj.contains("ThumbDataBase64"))
			{
				auto name = jsonObj["ThumbName"].toString();
				QByteArray temp = jsonObj["ThumbDataBase64"].toString().toUtf8();
				auto _data = QByteArray::fromBase64(temp);

				qDebug() << jsonObj["ThumbName"].toString() << ", data size = " << _data.size();
				auto p = QPixmap();
				p.loadFromData(_data, "JPG");
				emit incomingPreview(name, QPixmap(p));
			}
			break;
		}


		case LiveviewFrame:
		{
			if (jsonObj.contains("LiveviewFrameSize") && jsonObj.contains("LiveviewFrameDataBase64"))
			{
				auto lvFrameSize = jsonObj["LiveviewFrameSize"].toInt();

				QByteArray temp = jsonObj["LiveviewFrameDataBase64"].toString().toUtf8();
				auto _data = QByteArray::fromBase64(temp);
				auto p = QPixmap();
				p.loadFromData(_data, "JPG");

				qDebug() << "incomingLiveStreamFoto";
				emit incomingLiveStreamFoto(QPixmap(p));
				
			}
			break;
		}

		case File:
		{
			if (jsonObj.contains("PhotoName") && jsonObj.contains("PhotoDataBase64"))
			{
				auto fileName = jsonObj["PhotoName"].toString();
				QByteArray temp = jsonObj["PhotoDataBase64"].toString().toUtf8();
				emit incomingFullSizeFoto(fileName, QByteArray::fromBase64(temp));
			}
			break;
		}


		}
	}
}


Device::Device(QObject* parent)
	: QObject(parent)
{
}

Device::~Device()
{
}

void Device::setClientLibInterface(QSharedPointer<IClientLibObject> client)
{
	if (!clientLibObject.isNull())
	{
		auto qobject = dynamic_cast<QObject*>(clientLibObject.data());
		disconnect(qobject, SIGNAL(dataReceived(DataType, QString)),
			this, SLOT(onDataReceived(DataType, QString)));
	}
	clientLibObject = client;
	auto qobject = dynamic_cast<QObject*>(clientLibObject.data());
	connect(qobject, SIGNAL(dataReceived(DataType, QString)),
		this, SLOT(onDataReceived(DataType, QString)));
}

void Device::downloadFullSize(QString name)
{
	sendCommand(PhotoRequest, name.toUtf8());
}
 
QString cmdToString(Command command)
{
	QString commandString;
	switch (command)
	{
	case SwitchCamPower:
		commandString = "SwitchCamPower";
		break;
	case ActZoom: 
		commandString = "ActZoom";
		break;
	case TakePicture:
		commandString = "TakePicture";
		break;
	case SetSerieMode:
		commandString = "SetSerieMode";
		break;
	case SetSerieTimeout:
		commandString = "SetSerieTimeout";
		break;
	case SetImageSize:
		commandString = "SetImageSize";
		break;
	case SetImageQuality:
		commandString = "SetImageQuality";
		break;
	case PhotoRequest:
		commandString = "PhotoRequest";
		break;
	case SetDownloadPhotosFlag:
		commandString = "SetDownloadPhotosFlag";
		break;
	case SetExposureMode:
		commandString = "ChangeExposureMode";
		break;
	case SetISO:
		commandString = "ChangeISO";
		break;
	case SetShutter:
		commandString = "ChangeShutter";
		break;
	case SetAperture:
		commandString = "ChangeAperture";
		break;
	case ClearPhotosReadyToSendList:
		commandString = "ClearPhotosReadyToSendList";
		break;
	case RequestPhotosReadyToSendList:
		commandString = "RequestPhotosReadyToSendList";
		break;
	case StartPhotoExport:
		commandString = "StartPhotoExport";
		break;
	case SetNetwork:
		commandString = "SetNetwork";
		break;
	case SetLiveviewMode:
		commandString = "SetLiveviewMode";
		break;
	default:
		commandString = "Undefined";
		break;
	}
	return commandString;
}

void Device::sendCommand(Command cmd, QByteArray data)
{
	clientLibObject->setCommand(cmd, data);
	
	emit log("command: " + cmdToString(cmd) + ' ' + data +'\n');
}

void Device::on()
{
	sendCommand(SwitchCamPower, QByteArray());
	
}

void Device::off()
{
	sendCommand(SwitchCamPower, QByteArray());
}

void Device::takeShot()
{
	sendCommand(TakePicture, QByteArray());
}

 void Device::setSeriaMode()
{
	QByteArray data;
	if (seriaOn)
		data.append(char(0x00));
	else
		data.append(char(0x01));
	sendCommand(SetSerieMode, data);
}

 void Device::startLiveStream()
 {
	 QByteArray data;
	 data.append(char(0x01));
	 sendCommand(SetLiveviewMode, data);
 }

 void Device::stopLiveStream()
 {
	 QByteArray data;
	 data.append(char(0x00));
	 sendCommand(SetLiveviewMode, data );
 }
 

 

void Device::zoomPlus()
{
	QByteArray data;
	data.append(char(0x01));
	sendCommand(ActZoom, data);
}

void Device::zoomMinus()
{
	QByteArray data;
	data.append(char(0x00));
	sendCommand(ActZoom, data);
}

void Device::requestPhotos()
{
	sendCommand(RequestPhotosReadyToSendList, QByteArray());
}

void Device::autoDownloadFullsizePhoto()
{
	sendCommand(StartPhotoExport, QByteArray());
}


void Device::startAutoSendPhotos()
{
	QByteArray data;
	data.append(char(0x01));
	sendCommand(SetDownloadPhotosFlag, data);
		
}

void Device::stopAutoSendPhotos()
{
	QByteArray data;
	data.append(char(0x00));
	sendCommand(SetDownloadPhotosFlag, data);
}


	        
void Device::clearListSendingPhoto()
{
	sendCommand(ClearPhotosReadyToSendList, QByteArray());
	
}


bool Device::isOn() const
{
	return false;
}

bool Device::isOnSd() const
{
	return sdOn;
}

bool Device::isOnTelemetry() const
{
	return telemetryOn;
}

bool Device::isOnLiveMode() const
{
	return liveMode;

}

bool Device::isOnGps() const
{
	return  false;
}

bool Device::isLinkOnCamera() const
{
	return false;
}

IDevice::MainStatus Device::getMainStatus() const
{
	return status;
}

QList<QString> Device::getIsoValues() const
{
	return	isoValues;
}

QString Device::getCurrentIso() const
{
	return currentCameraiso;
}

void Device::setCurrentIso(QString setIso)
{ 
	auto data = setIso.toUtf8();
	sendCommand(SetISO, data);
}

QList < QString > Device::getApertureValues()const
{
	return{};
}

QString Device::getCurrentAperture() const
{
	return{};
}

void Device::setCurrentAperture(QString)
{}

QList<QString> Device::getShutterValues() const
{
	return shutterValues;
}


QList<QString> Device::getImageSizeValues() const
{
	return  imageSizeListValues;
}

QList<QString> Device::getImageQualityValues() const
{
	return  imageQualityListValues;
}



QString Device::getCurrentImageSize() const
{
	return currentCameraImageSize;
}

QString Device::getCurrentImageQuality() const
{
	return  currentCameraImageQuality;
}



QString Device::getCurrentShutter() const
{
	return currentCameraShutter;
}

void Device::setCurrentShutter(QString setShutter)
{
	auto data = setShutter.toUtf8();
	sendCommand(SetShutter, data);
}

void Device::setCurrentImageSize(QString setImageSize)
{
	auto data = setImageSize.toUtf8();
	sendCommand(SetImageSize, data);
}


void Device::setCurrentImageQuality(QString setImageQuality)
{
	auto data = setImageQuality.toUtf8();
	sendCommand(SetImageQuality, data);
}



QList < QString > Device::getZoomValues() const
{
	return{};
}

QString Device::getCurrentZoom()const
{
	return{};
}

void Device::setCurrentZoom(QString)
{
}

QStringList Device::splitValues(const QString& s)
{
	return  s.split(";", QString::SkipEmptyParts);
}


void Device::setStatusFromCallback(ProtocolStatus _status, QVariant value)
{	
	if (_status == ProtocolStatus::SendingPhotos)
	{
		autoSendPhotosFlag = value.toBool();
		emit updatedAutoSendPhotos(autoSendPhotosFlag);
	}

	if (_status == ProtocolStatus::cameraLiveMode)
	{
		liveMode = value.toBool();
		emit updatedLiveModeStatusOn(liveMode);
	}

	if (_status == ProtocolStatus::TelemetryAvaliable)
	{
		telemetryOn = value.toBool();
		emit updatedTelemetryStatusOn(telemetryOn);
	}
		
	if (_status == ProtocolStatus::Sd)
	{
		sdOn = value.toBool();
		emit updatedSdStatusOn(sdOn);
	}
 
	if (_status == ProtocolStatus::PowerOn)
	{
		const auto powerValue = value.toString();
		if (powerValue == valueFalse)
			powerStatus = PowerStatus::False;
		else if (powerValue == valueTrue)
			powerStatus = PowerStatus::True;
		else powerStatus = PowerStatus::Switching;
	}

	if (_status == ProtocolStatus::ModuleStatus)
	{
		const auto moduleStatusValue = value.toString();
		if (moduleStatusValue == valueIdle)
			moduleState = ModuleState::Idle;
		else if (moduleStatusValue == valueReady)
			moduleState = ModuleState::Ready;
		else moduleState = ModuleState::Steady;
	}
	if (_status == ProtocolStatus::SendingPhotos)
	{
		sendPhoto = value.toBool();
	}

	if (_status == ProtocolStatus::SeriaMode)
	{
		seriaOn = value.toBool();
		emit updatedSeriaStatus(seriaOn);
	}

	if (_status == ProtocolStatus::saveProgress)
	{
		progressSave = value.toInt();
		emit updatedLoadingFullSize(progressSave);
	}
	if (_status == ProtocolStatus::isoList)
	{
		isoValues = value.toString().split(";", QString::SkipEmptyParts);
		emit updatedAvaliableIso(isoValues);
	}

	if (_status == ProtocolStatus::imageSizeList) 
	{
		
		imageSizeListValues = splitValues(value.toString());
		emit updatedAvaliableImageSize(imageSizeListValues);
	}

	if (_status == ProtocolStatus::imageQualityList)
	{
		imageQualityListValues = splitValues(value.toString());
		emit updatedAvaliableImageQuality(imageQualityListValues);
	}
	
	if (_status == ProtocolStatus::shutterList)
	{
		shutterValues = splitValues(value.toString());
		emit updatedAvaliableShutter(shutterValues);
	}

	if (_status == ProtocolStatus::cameraIso)
	{
		auto strings = value.toString().split("-");
		if (strings.size() == 3)
		{
			currentCameraiso = strings.at(1);
			emit updatedIsoValue(currentCameraiso);
		}
	}

	if (_status == ProtocolStatus::cameraImageSize) 
	{
		currentCameraImageSize = value.toString();
		emit updatedImageSizeValue(currentCameraImageSize);
	}

	if (_status == ProtocolStatus::cameraImageQuality)
	{
		currentCameraImageQuality = value.toString();
		emit updatedImageQualityValue(currentCameraImageQuality);
	}

	if (_status == ProtocolStatus::cameraShutter)
	{
		auto strings = value.toString().split("-");
		if (strings.size() == 3)
		{
			currentCameraShutter = strings.at(1);
			emit updatedShutterValue(currentCameraShutter);
		}
	}

	
	 if ((PowerStatus::True == powerStatus) && (moduleState == ModuleState::Ready)&& !seriaOn && !sendPhoto)
		status = MainStatus::Ready;
	else if (powerStatus == PowerStatus::False || (moduleState == ModuleState::Idle))
		status = MainStatus::Offline;
	else if ((PowerStatus::Switching == powerStatus) || (moduleState == ModuleState::Steady)|| seriaOn || sendPhoto)
		status = MainStatus::Busy;
	


	

	emit updatedMainStatus(status);
}
