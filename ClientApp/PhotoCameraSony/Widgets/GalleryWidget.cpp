#include "GalleryWidget.h"
#include <QStandardItem>
#include "GalleryWidgetItemDelegate.h"
#include <QAbstractItemView>
#include <QDebug>

GalleryWidget::GalleryWidget(QWidget* parent):QWidget(parent)
{
	listView = new QListView(this);
	
	listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	listView->setSpacing(5);
	listView->setResizeMode(QListView::Adjust);
	listView->setFlow(QListView::LeftToRight);
	//listView->setGridSize(QSize(140, 140));
	listView->setWrapping(true);
	listView->setMinimumWidth(270);
	listView->setMinimumHeight(240);
	model = new QStandardItemModel(listView);
	listView->setModel(model);
	setLayout(new QHBoxLayout);
	layout()->addWidget(listView);
	listView->setItemDelegate(new GalleryWidgetItemDelegate(model, listView));
}

void GalleryWidget::addImage(QString name, QPixmap pixmap)
{
	auto item = new QStandardItem(QIcon(), name);
	item->setData(pixmap);
	item->setCheckable(true);
	item->setCheckState(Qt::Checked);
	qDebug() << "GalleryWidget::addImage " << pixmap.size();// 255/170
	model->appendRow(item);
	qDebug() << "model size = " << model->rowCount();
}

void GalleryWidget::clearList()
{
	model->clear();
}


void GalleryWidget::deleteImage(QString name)
{
	auto items = model->findItems(name);
	for (auto i : items)
	{
		auto modelIndex = model->indexFromItem(i);
		model->removeRow(modelIndex.row());
	}
}


QStringList GalleryWidget::getSelectedItems() const
{
	QStringList list;
	for (const QModelIndex& index : listView->selectionModel()->selectedIndexes())
	{
		list.append(model->itemFromIndex(index)->text());
	}
	qDebug() << "gallery selected items: " << list.join(",");
	return list;
}
void GalleryWidget::setAutoscroll(bool on) 
{
	autoscrollOn = on;

	if (on)
	{
		connect(listView->model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), listView, SLOT(scrollToBottom()));
	}
	else 
	{
		disconnect(listView->model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), listView, SLOT(scrollToBottom()));
	}
}

bool GalleryWidget:: isAutoscrollOn( ) const
{
	return autoscrollOn;
}



GalleryWidget::~GalleryWidget()
{
}
