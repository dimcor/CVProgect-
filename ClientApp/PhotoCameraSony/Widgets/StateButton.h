#pragma once
#include <QPushButton>


class StateButton : public QPushButton
{
	Q_OBJECT

public:
	StateButton(QPixmap on, QPixmap off, QPixmap disable, QWidget* parent = 0);
	virtual ~StateButton();
	
	enum class State
	{
		on,
		off,
		disable
	};
	void setState(State);
	State getState() const;

protected:
	void paintEvent(QPaintEvent *event);

private:
	State state;
	QColor color;
	QPixmap icon_on;
	QPixmap icon_off;
	QPixmap icon_disable;
};

