#include "GalleryWidgetItemDelegate.h"
#include <QDebug>
#include "Common.h"

//const QSize pixmapSize = QSize(160, 120);
const int offset{5};
const int doubleOffset{10};

GalleryWidgetItemDelegate::GalleryWidgetItemDelegate(QStandardItemModel* model, QObject* parent) :
	QStyledItemDelegate(parent),
	m_model(model)
{

}

void GalleryWidgetItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QRect optionRect = option.rect;

	auto item = m_model->item(index.row(), index.column());
	QPixmap pix(item->data().value<QPixmap>()); 
	//const auto scaledSize = getScaledImageSizeToElem(pixmapSize, pix.size());
	
	

	const auto scaledSize = QSize(160, 120);
	const auto x = optionRect.x();
	const auto y = optionRect.y();
	const auto h = scaledSize.height();
	const auto w = scaledSize.width();

	optionRect.setHeight(h + doubleOffset);
	optionRect.setWidth(w + doubleOffset);
	QColor itemColor = Qt::white;
	if (option.state & QStyle::State_Selected) 
	{
		auto selectedElemColor = QColor(option.palette.color(QPalette::Active, QPalette::Dark));
		painter->setBrush(QBrush(selectedElemColor));
		painter->setPen(Qt::NoPen);
		painter->drawRect(optionRect);
		itemColor = selectedElemColor;
	}

	painter->drawPixmap(x + offset, y + offset, scaledSize.width(), h, pix);
	painter->setPen(Qt::black);
	const auto textY = optionRect.y() - offset + h;
	const QPoint textPos(optionRect.x() + doubleOffset, textY);
	QFont font("times", 8);
	painter->setFont(font);
	QString text = item->text();
	auto textid = text.lastIndexOf(".");
	text.truncate(textid);
	QFontMetrics fm (font);
	int pixelsWide = fm.width(text);
	int pixelsHigh = fm.height();
	QRect textRect = option.rect;
	painter->setBrush(itemColor);
	painter->setPen(itemColor);
	painter->drawRect(optionRect.x() + doubleOffset - 2, textY - doubleOffset -2 , pixelsWide + 2, pixelsHigh);
	painter->setPen(Qt::black);

	painter->drawText(textPos, text);
	painter->setBrush(QBrush(Qt::NoBrush));
}

QSize GalleryWidgetItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{

	auto item = m_model->item(index.row(), index.column());

	QPixmap pix(item->data().value<QPixmap>());
	//const auto scaledSize = getScaledImageSizeToElem(pixmapSize, pix.size());
	const auto scaledSize = pix.size();
	QSize result(scaledSize.width() + doubleOffset, scaledSize.height() + doubleOffset);

	return result;
}
