#pragma once
#include "Devices/IDevice.h"
#include <QSharedPointer>

class DeviceFactory
{
public:
	DeviceFactory();
	virtual ~DeviceFactory();
	
	static QSharedPointer<IDevice> create(bool isSony);

};

