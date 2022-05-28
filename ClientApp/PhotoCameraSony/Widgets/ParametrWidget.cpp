#include "ParametrWidget.h"
#include <QLayout>
#include <QPushButton>
#include <QDebug>

ParametrWidget::ParametrWidget(QWidget* parent) : QWidget(parent)
{
	QHBoxLayout* layout = new QHBoxLayout;
	layout->setMargin(0);
	button_next = new QPushButton;
	button_prev = new QPushButton;
	label_current = new QLabel;
	setLayout(layout);
	button_prev->setMinimumWidth(60);
	button_next->setMinimumWidth( 60);
	button_prev->setText("<");
	button_next->setText(">");
	layout->addWidget(button_prev, Qt::AlignHCenter);
	layout->addStretch(10);
	label_current->setText("-");
	label_current->setMinimumWidth( 60);
	label_current->setAlignment(Qt::AlignCenter);
	layout->addWidget(label_current, Qt::AlignHCenter);
	layout->addStretch(10);
	layout->addWidget(button_next, Qt::AlignHCenter);

	connect(button_next,SIGNAL(clicked()),this,SLOT(onNext_click()));
	connect(button_prev,SIGNAL(clicked()),this,SLOT(onPrev_click()));

	setStyleSheet("QLabel { background-color: #D1D1C1; margin: 1px 1px 1px 1px; };");//  задаем цвет  
}


ParametrWidget::~ParametrWidget()
{

}

 void ParametrWidget::setValues(QList<QString> values)
{
	this->values = values;
	auto val = values.size();

	// remove this
	if (val >= 1)
	{
		current_value_index = 1;
		setValue(values.at(current_value_index));
	}
}

 void ParametrWidget::onNext_click( )
 {
	 if(current_value_index+1 >=values.size())
		 return;

	 current_value_index++;
	 updateValues();
}

 void ParametrWidget::onPrev_click()
 {
	 if (current_value_index - 1 < 0)
		 return;

	 current_value_index--;
	 updateValues();
}

 void ParametrWidget::updateValues()
{
	if (values.isEmpty())
		return;

	 if (current_value_index >= 0 && current_value_index <= values.size())
	 {
		 auto value = values.at(current_value_index);
		 emit needUpdateValue(value);
	 }

}

 QString ParametrWidget::getValueByIndex(int index) const
{
	if (index >= 0 && index < values.size())
	{
		return values.at(index);
	}
	return "-";
}


void ParametrWidget::setValue(QString v)
{
	auto indexOf = values.indexOf(v);
	if (indexOf == -1)
		return;

	current_value_index = indexOf;
	
	if (current_value_index + 1 == values.size())
	{
		button_next->setEnabled(false);// кнопка next_button  не активна
		button_prev->setEnabled(true);
	}
	else 
	{
		button_prev->setEnabled(true);// кнопка prev_button  активна
		button_next->setEnabled(true);// кнопка prev_button  активна
	}
	if (current_value_index  == 0)
	{
		button_prev->setEnabled(false); // кнопка prev_button    не активна
		button_next->setEnabled(true); // кнопка next_button   активна
	}

	const auto value = getValueByIndex(current_value_index);
	label_current->setText(value);

	const auto prevValue = getValueByIndex(current_value_index - 1);
	button_prev->setText(prevValue);

	const auto nextValue = getValueByIndex(current_value_index + 1);
	button_next->setText(nextValue);
}
