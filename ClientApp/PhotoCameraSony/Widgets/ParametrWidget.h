#pragma once

#include <QPushButton>
#include <QLabel>

class ParametrWidget : public QWidget
{
	Q_OBJECT

public:
	ParametrWidget(QWidget* parent =0);
	virtual ~ParametrWidget();
	void setValues(QList<QString> val);
	void setValue(QString);

public slots:

signals:
	void needUpdateValue(QString);
	
private slots :
	void onNext_click();
	void onPrev_click();

private:
	void updateValues();
	QString getValueByIndex(int index) const;

	QPushButton* button_next;
	QPushButton* button_prev;
	QLabel* label_current;
	QList<QString> values;
	int current_value_index{0};
};

