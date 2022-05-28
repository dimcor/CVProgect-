#include "ClientLibObject.h"

#include <QDebug>
#include "../../Third-party/SonyPhotoCamera/SonyPhotoCameraClientLib/rpisonycameraclientlib_global.h"
#pragma comment (lib, "../../Third-party/SonyPhotoCamera/SonyPhotoCameraClientLib/SonyPhotoCameraClientLib")

static IRpiSonyCameraClient* client = nullptr;

void __stdcall dataUpdated(DataType dataType, const char* data, int len)
{
	qDebug() << "callback dataUpdated";
	auto inst = ClientLibObject::getInstance();
	auto rawData = QByteArray::fromRawData(data, len);
	emit inst->dataReceived((dataType), QString(rawData));
}

ClientLibObject::ClientLibObject(QObject* parent)
	: QObject(parent)
{
	if (!client)
		createClient(&client);
	client->SetDataNotification(dataUpdated);
}

void ClientLibObject::setCommand(Command command, const QByteArray& data)
{
	if (nullptr == client)
		return;

	client->SetCommand(command, QByteArray(data).data(), data.size());
}
