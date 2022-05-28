#pragma once
#include <QDropEvent>
#include <QListView>
#include <QGridLayout>
#include <QStandardItemModel>

class GalleryWidget : public QWidget
{
	Q_OBJECT

public:
	GalleryWidget(QWidget* parent = 0);
	virtual ~GalleryWidget();

	void addImage(QString name, QPixmap pixmap);
	void deleteImage(QString name);
	QStringList getSelectedItems() const;
	void setAutoscroll(bool on);
	bool isAutoscrollOn() const;
	bool autoscrollOn{false};
	void clearList();
	
private:
	QListView*          listView;
	QGridLayout*        gallery_gridLayout;
	QStandardItemModel* model;
	
};

