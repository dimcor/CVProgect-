#include "ParametrManager.h"
#include <QDebug>

ParametrManager::ParametrManager(ParametrWidget* widget, QObject* parent)
	: QObject(parent), widget(widget)
{
	connect(widget, SIGNAL(needUpdateValue(QString)), this, SLOT(userSetValue(QString)));
	connect(&userSetValueTimer, SIGNAL(timeout()), this, SLOT(onTimerTimeOut()));
	connect(&deviceResponseTimer, SIGNAL(timeout()), this, SLOT(onDeviceAnswer()));
}

void ParametrManager::deviceUpdateValue(QString)
{
	widget->setEnabled(true);
	deviceResponseTimer.stop();

}

void ParametrManager::userSetValue(QString v)
{
	userSetValueTimer.stop();
	userSetValueTimer.start(500);
	widget->setValue(v);
	valueInCurrent = v;
}

void ParametrManager::onTimerTimeOut()
{
	emit needUpdateValue(valueInCurrent);
	userSetValueTimer.stop();
	widget->setEnabled(false);
	deviceResponseTimer.start(5000);
}

void ParametrManager::onDeviceAnswer()
{
	widget->setEnabled(true);
	deviceResponseTimer.stop();
}

