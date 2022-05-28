#pragma once

#include <QObject>
#include "IClientLibObject.h"
#include <QSharedPointer>
#include <QTimer>

class QUdpSocket;
class QWebSocket;

class SimulatorClientLibObject : public QObject, public IClientLibObject
{
	Q_OBJECT

public:
	explicit SimulatorClientLibObject(QObject* parent = nullptr);
	~SimulatorClientLibObject();

	void setCommand(Command command, const QByteArray& data) override;

signals:
	void dataReceived(DataType, QString) override;

private:
	QSharedPointer<QWebSocket> socket;

	CallBackFunctionData incomingDataCallback;
	int port { 7756 };
	QTimer reconnectTimer;
	
private slots:
	void onBinaryMessege(const QByteArray &);
	void onConnected();
	void onReconnectTimer();
};

