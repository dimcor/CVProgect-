#pragma once
#include <QObject>
#include  "Widgets/ParametrWidget.h"
#include <QTimer>

class  ParametrManager : public QObject
{
	Q_OBJECT

public:
	ParametrManager(ParametrWidget* widget, QObject* parent = 0);
	

	virtual ~ParametrManager(){}

	QString valueInCurrent;

public slots :
	void deviceUpdateValue(QString);

private slots :
	void userSetValue(QString);
	void onTimerTimeOut();
	void onDeviceAnswer();

private:
	ParametrWidget* widget;
	QTimer userSetValueTimer;
	QTimer deviceResponseTimer;

signals:
	void needUpdateValue(QString);
	
};
