#pragma once
#include <qstring.h>
#include <QMap>

class ExifReader
{
public:
	enum class ExifParam
	{
		OriginalDateTime
	};

	static QMap<ExifParam, QString> getMetadata(QByteArray data);
};

