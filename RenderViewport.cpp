#include "RenderViewport.h"
#include <QMouseEvent>
#include "Graphics.h"
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
	if (renderframe != nullptr)
	{
		delete renderframe;
		renderframe = nullptr;
	}
		
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
void RenderViewport::InitialViewport()
{
	//初始化renderViewport，传入hWnd初始化dx
  renderframe = new Graphics((HWND)winId());
}
void RenderViewport::UpdateViewport()
{
	renderframe->EndFrame();
}