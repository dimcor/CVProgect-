#include "DeviceFactory.h"
#include "Devices/IDevice.h"
#include "Devices/SonyPhotoCamera/Device.h"

#include "SonyPhotoCamera/ClientLibObject.h"
#include "SonyPhotoCamera/SimulatorClientLibObject.h"

DeviceFactory::DeviceFactory()
{
	qRegisterMetaType<DataType>("DataType");
}

DeviceFactory::~DeviceFactory()
{
	
}

static void emptyDeleter(IClientLibObject* obj) {}

QSharedPointer<IDevice> DeviceFactory::create(bool isSony)
{
	QSharedPointer<IClientLibObject> clientLib;
	if (isSony)
	{
		clientLib = QSharedPointer<IClientLibObject>(ClientLibObject::getInstance(), emptyDeleter);
	}
	else
	{
		clientLib = QSharedPointer<IClientLibObject>(new SimulatorClientLibObject);
	}
	auto sonyDevice = new Device;
	sonyDevice->setClientLibInterface(clientLib);
	return QSharedPointer<IDevice>(sonyDevice);
}
