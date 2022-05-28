#include <QSize>

inline QSize getScaledImageSizeToElem(const QSize& imageSize, const QSize& origSize)
{
	const auto Kh = imageSize.height() / float(origSize.height());
	const auto Kw = imageSize.width() / float(origSize.width());
	const auto k = qMin(Kw, Kh);
	//return QSize(origSize.width() / k, origSize.height() / k);
	return QSize(imageSize.width() / k, imageSize.height() / k);
}
