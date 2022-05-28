#pragma once

#include <QByteArray>
#include "../../Third-party/SonyPhotoCamera/SonyPhotoCameraClientLib/typedefs.h"

/** @brief Обёртка для работы с библиотекой управления
 * фотоаппаратом через сигналы и слоты */
class IClientLibObject
{
public:
	virtual ~IClientLibObject(){}

	virtual void setCommand(Command command, const QByteArray& data) = 0;

// signals:
	virtual void dataReceived(DataType, QString) = 0;
};
