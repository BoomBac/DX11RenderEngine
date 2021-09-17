#include "RenderViewport.h"
#include <QMouseEvent>
#include "Graphics.h"

RenderViewport::RenderViewport(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setFixedSize(800, 600);
	this->setAttribute(Qt::WA_PaintOnScreen);
	setFocusPolicy(Qt::FocusPolicy::ClickFocus);
	//setMouseTracking(true);
}

RenderViewport::~RenderViewport()
{
	if (graphicsIns != nullptr)
	{
		delete graphicsIns;
		graphicsIns = nullptr;
	}
}

void RenderViewport::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_W:
	{
		graphicsIns->SetCameraTranslation(0.f, 0.f, -1.f);
	}
	break;
	case Qt::Key_S:
	{
		graphicsIns->SetCameraTranslation(0.f, 0.f, 1.f);
	}
	break;
	case Qt::Key_A:
	{
		graphicsIns->SetCameraTranslation(1.f, 0.f, 0.f);
	}
	break;
	case Qt::Key_D:
	{
		graphicsIns->SetCameraTranslation(-1.f, 0.f, 0.f);
	}
	break;
	case Qt::Key_Q:
	{
		graphicsIns->SetCameraTranslation(0.f, -1.f, 0.f);
	}
	break;
	case Qt::Key_E:
	{
		graphicsIns->SetCameraTranslation(0.f, 1.f, 0.f);
	}
	break;
	default:
		break;
	}
}

void RenderViewport::mouseMoveEvent(QMouseEvent* e)
{
	//传入增量
	float detlaX = (float)(e->pos().x() - posX);
	float detlaY = (float)(e->pos().y() - posY);
	//y 0 x
	graphicsIns->SetCameraTransformation(detlaY,0.f,0.f);
	graphicsIns->SetCameraTransformationW(0.f,detlaX,0.f);
	mouPos = QString(" x = %1, y = %2").arg(QString::number(e->pos().x())).arg(QString::number(e->pos().y()));
	posX = e->pos().x();
	posY = e->pos().y();
	emit(MouseMoved(mouPos));
}

void RenderViewport::mousePressEvent(QMouseEvent* e)
{
	moubtState = "Pressed";
	posX = e->pos().x();
	posY = e->pos().y();
	emit(MousePressed(moubtState));
}

void RenderViewport::mouseReleaseEvent(QMouseEvent* e)
{
	moubtState = "Released";
	//posX = e->pos().x();
	//posY = e->pos().y();
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
