#include "RenderViewport.h"
#include <QMouseEvent>
#include "Graphics.h"
#include <QDebug>

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
	if (graphicsIns != nullptr)
	{
		delete graphicsIns;
		graphicsIns = nullptr;
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
  graphicsIns = new Graphics((HWND)winId());
}
void RenderViewport::UpdateViewport()
{
	graphicsIns->EndFrame();
}

void RenderViewport::SetbgColor(float color[4])
{
	graphicsIns->SetVPBackColor(color);
}

void RenderViewport::SetSelectedObjectTransform(const CusMath::vector3d& pos, const char& flag)
{
	switch (flag)
	{
	case 0:
	{
		graphicsIns->SetSelectedObjectTranslate(pos);
	}
		break;
	case 1:
	{
		graphicsIns->SetSelectedObjectRotation(pos);
	}
	break;
	case 2:
	{
		graphicsIns->SetSelectedObjectScale(pos);
	}
		break;
	};
}

void RenderViewport::OnCameraSlideBarChanged(const float& x, const float& y, const float& z)
{
	graphicsIns->SetCameraTransformation(x, y, z);
}
