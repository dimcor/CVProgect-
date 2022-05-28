#pragma once
#include "IClientLibObject.h"
#include <QObject>

class ClientLibObject : public QObject, public IClientLibObject
{
	Q_OBJECT

protected:
	ClientLibObject(QObject* parent = nullptr);

public:
	~ClientLibObject() {}

	static ClientLibObject* getInstance()
	{
		static ClientLibObject device;
		return &device;
	}

	void setCommand(Command command, const QByteArray& data) override;

signals:
	void dataReceived(DataType, QString) override;
};

