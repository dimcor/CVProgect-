#include "SimulatorClientLibObject.h"
#include <QList>
#include <QWebSocket>
#include <QString>

SimulatorClientLibObject::SimulatorClientLibObject(QObject* parent) : QObject(parent)
{
	socket.reset(new QWebSocket);

	connect(socket.data(), &QWebSocket::connected, this, &SimulatorClientLibObject::onConnected);
	connect(&reconnectTimer, &QTimer::timeout, this, &SimulatorClientLibObject::onReconnectTimer);
	reconnectTimer.setSingleShot(false);
	reconnectTimer.start(500);
}

SimulatorClientLibObject::~SimulatorClientLibObject()
{
	
}

void  SimulatorClientLibObject::onBinaryMessege(const QByteArray& data)
{
	qDebug() << "SimulatorClientLibObject::onBinaryMessege " << data;

	auto dataType = DataType ::Status;
	if (data.contains("LiveviewFrameDataBase64"))
	{
		dataType = DataType::LiveviewFrame;
	}
	if (data.contains("ThumbDataBase64"))
	{
		dataType = DataType::PhotoThumbnail;
	}
	if (data.contains("PhotoDataBase64"))
	{
		dataType = DataType::File;
	}
	emit dataReceived(dataType, QString(data));
}

void SimulatorClientLibObject:: onReconnectTimer()
{
	if (socket->state() != QAbstractSocket::ConnectedState)
	{
		socket->open(QUrl("ws://localhost:7755"));
	}
	else
	{
		reconnectTimer.stop();
	}
//	connect(socket, &QWebSocket::disconnected, this, &SimulatorClientLibObject::close);
}

void SimulatorClientLibObject::onConnected()
{
	connect(socket.data(), &QWebSocket::binaryMessageReceived,
		this, &SimulatorClientLibObject::onBinaryMessege);
}

void SimulatorClientLibObject::setCommand(Command command, const QByteArray& data)
{
	QByteArray cmdData;
	qDebug() << "what is command ? " << command;
	switch (command)
	{
		case SetLiveviewMode:
			{
				cmdData = QByteArray("SetLiveviewMode");
				socket->sendBinaryMessage(cmdData);
				break;
			}
	
		case PhotoRequest:
			{
				cmdData = QByteArray("PhotoRequest ").append(data);
				socket->sendBinaryMessage(cmdData);
				break;
			}

		case RequestPhotosReadyToSendList:
			{
				cmdData = QByteArray("RequestPhotosReadyToSendList");
				socket->sendBinaryMessage(cmdData);
				break;
			}

		case SwitchCamPower :
			{
				cmdData = QByteArray("SwitchCamPower");
				socket->sendBinaryMessage(cmdData);
				break;
			}
		default:
		{
			qDebug() << "SimulatorClientLibObject::setCommand: ERROR undefined command!";
			return;
		}
	socket->sendBinaryMessage(cmdData);
	}
}


