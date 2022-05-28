#include "StateButton.h"
#include <QPainter>


StateButton::StateButton(QPixmap on, QPixmap off, QPixmap disable, QWidget* parent )
	: QPushButton(QIcon(disable), "", parent), icon_on(on),icon_off(off),icon_disable(disable)
{
	setState(State::disable);

}


StateButton::~StateButton()
{
}

void StateButton::setState(State s)
{
	state = s;
	if (state == State::on)
	{
		setIcon(QIcon(icon_on));
	}
	else if (state == State::disable)
	{
		setIcon(QIcon(icon_disable));
	}
	else if (state == State::off)
	{
		setIcon(QIcon(icon_off));
	}
	
}



StateButton::State StateButton::getState() const
{
	return  state;
};

void StateButton::paintEvent(QPaintEvent *event)
{
	QPushButton::paintEvent(event);
	QPainter painter(this);
	painter.setPen(color);

}