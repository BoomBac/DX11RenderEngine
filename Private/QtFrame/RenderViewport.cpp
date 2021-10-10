#include <QMouseEvent>
#include "Public\Render\Graphics.h"
#include "Public\QtFrame\RenderViewport.h"
#include "Public\Render\Drawable\Drawable.h"
#include "Public\Render\GeometryFactory.h"

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
		graphicsIns->UpdateCameraState(ECameraMovementState::kForward);
	}
	break;
	case Qt::Key_S:
	{
		graphicsIns->UpdateCameraState(ECameraMovementState::kBack);
	}
	break;
	case Qt::Key_A:
	{
		graphicsIns->UpdateCameraState(ECameraMovementState::kLeft);
	}
	break;
	case Qt::Key_D:
	{
		graphicsIns->UpdateCameraState(ECameraMovementState::kRight);
	}
	break;
	case Qt::Key_Q:
	{
		graphicsIns->UpdateCameraState(ECameraMovementState::kUp);
	}
	break;
	case Qt::Key_E:
	{
		graphicsIns->UpdateCameraState(ECameraMovementState::kDown);
	}
	break;
	default:
		break;
	}
}

void RenderViewport::keyReleaseEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_W:
	{
		graphicsIns->UpdateCameraState(ECameraMovementState::kStop);
	}
	break;
	case Qt::Key_S:
	{
		graphicsIns->UpdateCameraState(ECameraMovementState::kStop);
	}
	break;
	case Qt::Key_A:
	{
		graphicsIns->UpdateCameraState(ECameraMovementState::kStop);
	}
	break;
	case Qt::Key_D:
	{
		graphicsIns->UpdateCameraState(ECameraMovementState::kStop);
	}
	break;
	case Qt::Key_Q:
	{
		graphicsIns->UpdateCameraState(ECameraMovementState::kStop);
	}
	break;
	case Qt::Key_E:
	{
		graphicsIns->UpdateCameraState(ECameraMovementState::kStop);
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
	graphicsIns->camera_.AddRotation(detlaY * 0.01f, 0.f, 0.f);
	graphicsIns->camera_.AddRotation(0.f,detlaX * 0.01f, 0.f);
	//状态栏文字
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
	graphicsIns->outline_notify_->AddObserver(this);
}
void RenderViewport::UpdateViewport()
{
	graphicsIns->EndFrame();
}

void RenderViewport::SetbgColor(float color[4])
{
	graphicsIns->SetVPBackColor(color);
}

void RenderViewport::SetCoordinateType(bool is_world)
{
	graphicsIns->SetCoordinateType(is_world);
}

void RenderViewport::AddSceneObject(char type)
{
	if (type=='0')
	{
		GeometryFactory::GenerateGeometry(EGeometryType::kBox);
	}
	else if(type=='1')
		GeometryFactory::GenerateGeometry(EGeometryType::kPlane);

}

void RenderViewport::SetSelectObject(int index, double tranf_info[9])
{
	graphicsIns->SetSelectObject(index);
	if (index != -1)
	{
		auto tranf = graphicsIns->p_selected_object_->GetWorldLocation();
		tranf_info[0] = tranf.x;
		tranf_info[1] = tranf.y;
		tranf_info[2] = tranf.z;
		tranf = graphicsIns->p_selected_object_->GetWorldRotation();
		tranf_info[3] = tranf.x;
		tranf_info[4] = tranf.y;
		tranf_info[5] = tranf.z;
		tranf = graphicsIns->p_selected_object_->GetWorldScale();
		tranf_info[6] = tranf.x;
		tranf_info[7] = tranf.y;
		tranf_info[8] = tranf.z;
	}
}

void RenderViewport::DeleteSceneObject(const int& index)
{
	graphicsIns->DeleteSceneObject(index);
}

int RenderViewport::InitOutline(std::string* item_name)
{
	return graphicsIns->InitOutline(item_name);
}

void RenderViewport::AddLight(const char& light_type)
{
	graphicsIns->AddLight(light_type);
}

void RenderViewport::OnOutlineChanged(bool is_add)
{
	if (is_add)
	{
		emit(SceneObjectAdd(graphicsIns->last_add_object_name_.c_str()));
	}
}

void RenderViewport::SetSelectedObjectTransform(const CusMath::vector3d& pos, const char& flag)
{
	switch (flag)
	{
	case 0:
	{
		graphicsIns->SetSelectedObjectTranslate(pos);
		emit(ActorTransformChange(Graphics::p_selected_object_->GetWorldLocation(), '0'));
	}
		break;
	case 1:
	{
		graphicsIns->SetSelectedObjectRotation(pos);
		if (graphicsIns->GetCoordinateType())
		{
			object_rotation_changed_ = true;
		}
		//CusMath::vector3d rotation = Graphics::p_selected_object_->GetWorldRotation();
		//if (Graphics::p_selected_object_->forward_direction().x == 0.f && Graphics::p_selected_object_->forward_direction().y == 0.f && Graphics::p_selected_object_->forward_direction().z == 1.f)
		//	rotation.z += Graphics::p_selected_object_->GetActorRotation().z;
		//if (Graphics::p_selected_object_->right_direction().x == 1.f && Graphics::p_selected_object_->right_direction().y == 0.f && Graphics::p_selected_object_->right_direction().z == 0.f)
		//	rotation.x += Graphics::p_selected_object_->GetActorRotation().x;
		//if (Graphics::p_selected_object_->up_direction().x == 0.f && Graphics::p_selected_object_->up_direction().y == 1.f && Graphics::p_selected_object_->up_direction().z == 0.f)
		//	rotation.y += Graphics::p_selected_object_->GetActorRotation().y;
		emit(ActorTransformChange(Graphics::p_selected_object_->GetWorldRotation(), '2'));
	}
	break;
	case 2:
	{
		graphicsIns->SetSelectedObjectScale(pos);
	}
		break;
	};
}
