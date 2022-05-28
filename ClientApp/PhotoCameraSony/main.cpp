#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QSettings>
#include <QTextCodec>
#include <QtCore/QCommandLineParser>
#include <QDebug>
#include <qstandardpaths.h>
#include <QDir>

void parseDataPathArg(const QCoreApplication& app, QCommandLineParser& parser,QString& dataPath,QString& configPath);


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QCommandLineParser parser;
	QString dataPath, configPath;
	qRegisterMetaType<DataType>("DataType");
	parseDataPathArg(a, parser, dataPath, configPath);
	qDebug() << dataPath;
	qDebug() << configPath;
	MainWindow w(dataPath, configPath);
	w.show();
	return a.exec();

}

void parseDataPathArg(const QCoreApplication& app, QCommandLineParser& parser, QString& dataPath, QString& configPath)
{
	parser.addHelpOption();
	parser.addPositionalArgument("file", QCoreApplication::translate("main", "The file to open."));

	const QString defaultPath = "C:\\PhotoCameraData";
	const auto hint = QString("path to config files, default=\"%1\"").arg(defaultPath);
	QCommandLineOption dataPathOption({ "d", "dataPath" },
		QCoreApplication::translate("main", "Save all data files into <directory>.",hint.toStdString().c_str()),
		QCoreApplication::translate("main", "directory"));
	parser.addOption(dataPathOption);


	 const QString defaultConfigPath = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + QDir::separator() + "PhotoCameraData");
	QCommandLineOption configPathOption({ "c", "configPath" },
		QCoreApplication::translate("main", "Save config and log files into <directory>.",hint.toStdString().c_str()),
		QCoreApplication::translate("main", "directory"));
	parser.addOption(configPathOption);
	

	parser.process(app);
	if (parser.isSet(dataPathOption))
	{
		dataPath = parser.value(dataPathOption);
	}
	else
	{
		dataPath = defaultPath;
	}

	if (parser.isSet(configPathOption))
	{
		configPath = parser.value(configPathOption);
	}
	else
	{
		configPath = defaultConfigPath;
	}

	
	
}