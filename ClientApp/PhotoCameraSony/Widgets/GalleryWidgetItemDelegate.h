#ifndef IMAGEDELEGATE_H
#define IMAGEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QStandardItemModel>

class GalleryWidgetItemDelegate : public QStyledItemDelegate
{
public:
	explicit GalleryWidgetItemDelegate(QStandardItemModel *model, QObject *parent = nullptr);

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	QStandardItemModel* m_model;
};


#endif // IMAGEDELEGATE_H
