#include "RenderViewport.h"
#include <QMouseEvent>
#include <QString>


RenderViewport::RenderViewport(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setFixedSize(800, 600);
	this->setAttribute(Qt::WA_PaintOnScreen);
	setMouseTracking(true);

}

RenderViewport::~RenderViewport()
{
}

void RenderViewport::mouseMoveEvent(QMouseEvent* e)
{
	mouPos = QString(" x = %1, y = %2").arg(QString::number(e->pos().x())).arg(QString::number(e->pos().y()));
	emit(MouseMoved(mouPos));
}

void RenderViewport::mousePressEvent(QMouseEvent* e)
{
	moubtState = "Pressed";
	emit(MousePressed(moubtState));
}

void RenderViewport::mouseReleaseEvent(QMouseEvent* e)
{
	moubtState = "Released";
	emit(MouseReleased(moubtState));
}
