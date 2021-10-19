#include <QLabel>
#include <QTimer>
#include <QScrollBar>
#include <QPushButton>
#include <QColorDialog>
#include <QFileDialog>
#include <QListWidget>
#include <QTableWidget> 



#include "Public/Global.h"
#include "Public/QtFrame/DX11RenderEngine.h"
#include "vector3D.h"
#include "QComboBox"
#include "Public/QtFrame/RenderViewport.h"
#include "Public/Render/Drawable/Drawable.h"
#include "qevent.h"
#include "Public/Render/ModelResFactory.h"
#include <Public/Render/ResManage/MeshFactory.h>
#include <Public/Render/GeometryFactory.h>


namespace
{
	ELightType g_light_type = ELightType::kDirectionLight;
}


DX11RenderEngine::DX11RenderEngine(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setFixedSize(1280, 720);
	//初始化dx渲染视口
	ui.renderView->InitialViewport();
    //创建状态栏相关
    QLabel* lb_pos = new QLabel(QString("MousePos:"), this);
    ui.statusBar->addWidget(lb_pos);
    QLabel* lb_state = new QLabel(QString("MouseState:"), this);
    ui.statusBar->addWidget(lb_state);
    QLabel* lb_time = new QLabel(QString("CurrentTime:"), this);
    ui.statusBar->addWidget(lb_time);


    connect(ui.renderView, &RenderViewport::MouseMoved, [=](QString pos) {
        lb_pos->setText(QString("MousePos:") + pos);
        });
    connect(ui.renderView, &RenderViewport::MousePressed, [=](QString state) {
        lb_state->setText(QString("MouseState:") + state);
        });
    connect(ui.renderView, &RenderViewport::MouseReleased, [=](QString state) {
        lb_state->setText(QString("MouseState:") + state);
        });
    connect(ui.renderView, &RenderViewport::ActorTransformChange, [=](const CusMath::vector3d& tranf, char flag) {
        if (flag == '0')
        {
            transform_info_[0] = tranf.x;
            transform_info_[1] = tranf.y;
            transform_info_[2] = tranf.z;
        }
        else if (flag == '2')
        {
            transform_info_[3] = tranf.x;
            transform_info_[4] = tranf.y;
            transform_info_[5] = tranf.z;
        }
        });
    //世界大纲
    connect(ui.renderView, &RenderViewport::SceneObjectAdd, [=](const char* name){
        QString s(name);
        ui.L_OutLine->addItem(s);
        });

    //currentRowChanged
	connect(ui.L_OutLine, &QListWidget::currentRowChanged,this,&DX11RenderEngine::OnOutlineItemChanged);
    //添加场景物体按钮
    //AddSceneObject
    connect(ui.bt_addBox, &QPushButton::clicked, [=]() {
        ui.renderView->AddSceneObject('0');
        });
	connect(ui.bt_addPlane, &QPushButton::clicked, [=]() {
		ui.renderView->AddSceneObject('1');
		});
	connect(ui.bt_addDirL, &QPushButton::clicked, [=]() {
		ui.renderView->AddLight(ELightType::kDirectionLight);
		AdjustLightProperty(g_light_type, ELightType::kDirectionLight);
		g_light_type = ELightType::kDirectionLight;
		});
	connect(ui.bt_addPointL, &QPushButton::clicked, [=]() {
		ui.renderView->AddLight(ELightType::kPonintLight);
		AdjustLightProperty(g_light_type, ELightType::kPonintLight);
		g_light_type = ELightType::kPonintLight;
		});
	connect(ui.bt_addSpotL, &QPushButton::clicked, [=]() {
		ui.renderView->AddLight(ELightType::kSpotLight);
		AdjustLightProperty(g_light_type, ELightType::kSpotLight);
		g_light_type = ELightType::kSpotLight;
		});
    //导入模型
    connect(ui.bt_chooseModel, &QPushButton::clicked, [=]() {
		QString curPath = QCoreApplication::applicationDirPath();   //QDir::currentPath();初始目录
		QString dlgTitle = "choose a model file"; //对话框标题
		QString filter = "模型文件(*.obj);;所有文件(*.*)"; //筛选器
		QString aFileName = QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter); //返回文件名
        if (MeshFactory::getInstance().AddMesh(aFileName.toStdString()))
        {
            QFileInfo fi(aFileName);
            ui.L_ModelList->addItem(aFileName);
           // GeometryFactory::GenerateGeometry(fi.fileName().toStdString().c_str());
        }
        });
    //创建导入的模型
    connect(ui.L_ModelList, &QListWidget::itemDoubleClicked, [=](QListWidgetItem* item) {
             QFileInfo fi(item->text());
             qDebug() << fi.fileName();
             GeometryFactory::GenerateGeometry(fi.fileName().toStdString().c_str());
        });
    //更改坐标轴类型
    connect(ui.CB_Coord, &QComboBox::currentIndexChanged,this,&DX11RenderEngine::ChangeCoordinateType);

    //初始化transform属性
	InitTransformDetail();
    //初始化灯光属性
    InitLightDetail();

	InitRenderDetail();
	InitMaterialDetail();
    //选择背景颜色对话框
    connect(ui.bt_ChooseBC, &QPushButton::clicked, [=]() {
        static float bgc[4] = { 0.f,0.f,0.f,1.f };
        QColor color = QColorDialog::getColor(Qt::black, this, "choose color");
        bgc[0] = color.redF();
        bgc[1] = color.greenF();
        bgc[2] = color.blueF();
        ui.renderView->SetbgColor(bgc);
        });


    //arr接收场景构造时创建的物体名称，暂时最多16个
    std::string arr[16];
    auto size = ui.renderView->InitOutline(arr);
    qDebug() <<"object count" << size;
    for (int i = 0; i < size; i++)
    {
		ui.L_OutLine->addItem(QString::fromStdString(arr[i]));
    }
    //设置定时器，此处模拟tick，后续可能更改
    QTimer *timer = new QTimer(this);
    timer->start(16.7);  // fps = num/1000 16.7的话近似于60fps
    connect(timer, &QTimer::timeout, [=]()
    {
        //每次修改界面内容都会调用paintEvent()
       lb_time->setText(QString::number(Global::getInstance()->gTimer.Peek(), 'f', 2));
    }); 
    
}

DX11RenderEngine::~DX11RenderEngine()
{

}

void DX11RenderEngine::paintEvent(QPaintEvent* e)
{
    ui.renderView->UpdateViewport();
}

void DX11RenderEngine::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Delete:
        {
        auto row = ui.L_OutLine->currentRow();
		auto p = ui.L_OutLine->takeItem(row);
        ui.renderView->DeleteSceneObject(row+1);
		delete p;
        }
        break;
    }
}

void DX11RenderEngine::ChangeCoordinateType(int index)
{
    //存储变到世界坐标前，局部坐标的分量
    static double last_local_rotation[3] = { 0.f,0.f,0.f };
    if (index == 0)
    {
		last_local_rotation[0] = p_location_x_->GetValue();
		last_local_rotation[1] = p_location_x_->GetValue();
		last_local_rotation[2] = p_location_x_->GetValue();
		p_location_x_->SetProgressValue(static_cast<double>(transform_info_[0]));
		p_location_y_->SetProgressValue(static_cast<double>(transform_info_[1]));
		p_location_z_->SetProgressValue(static_cast<double>(transform_info_[2]));
		p_rotation_x_->SetProgressValue(static_cast<double>(transform_info_[3]));
		p_rotation_y_->SetProgressValue(static_cast<double>(transform_info_[4]));
		p_rotation_z_->SetProgressValue(static_cast<double>(transform_info_[5]));
		ui.renderView->SetCoordinateType(true);
    }	
    else
    {
		p_location_x_->SetProgressValue(0.f);
		p_location_y_->SetProgressValue(0.f);
		p_location_z_->SetProgressValue(0.f);

		if (ui.renderView->object_rotation_changed_)
		{
			p_rotation_x_->SetProgressValue(static_cast<double>(0.f));
			p_rotation_y_->SetProgressValue(static_cast<double>(0.f));
			p_rotation_z_->SetProgressValue(static_cast<double>(0.f));
		}
		else
		{
			p_rotation_x_->SetProgressValue(last_local_rotation[0]);
			p_rotation_y_->SetProgressValue(last_local_rotation[1]);
			p_rotation_z_->SetProgressValue(last_local_rotation[2]);
		}
        ui.renderView->SetCoordinateType(false);
        ui.renderView->object_rotation_changed_ = false;
    }
}

void DX11RenderEngine::OnOutlineItemChanged(int row)
{
    if (row > -1)
    {
        // 0-8表示变换，9表示当前是否选中了灯光
        double tranf[10];
        ui.renderView->SetSelectObject(row + 1, tranf);
        p_location_x_->SetProgressValue(tranf[0]);
        p_location_y_->SetProgressValue(tranf[1]);
        p_location_z_->SetProgressValue(tranf[2]);
		p_rotation_x_->SetProgressValue(tranf[3]);
		p_rotation_y_->SetProgressValue(tranf[4]);
		p_rotation_z_->SetProgressValue(tranf[5]);
        p_scale_x_->SetProgressValue(tranf[6]);
        p_scale_x_->SetProgressValue(tranf[7]);
        p_scale_x_->SetProgressValue(tranf[8]);
		ELightType next_light_type = ui.renderView->GetLightType();
        if (tranf[9] == 1.f)
        {
			p_light_detail_->setVisible(true);
			AdjustLightProperty(g_light_type, next_light_type);
			g_light_type = next_light_type;
        }
		AdjustRenderProperty();
		AdjustRenderProperty();
    }
    else
    {
        ui.renderView->SetSelectObject(-1, nullptr);
		p_location_x_->SetProgressValue(0.f);
		p_location_y_->SetProgressValue(0.f);
		p_location_z_->SetProgressValue(0.f);
		p_rotation_x_->SetProgressValue(0.f);
		p_rotation_y_->SetProgressValue(0.f);
		p_rotation_z_->SetProgressValue(0.f);
		p_scale_x_->SetProgressValue(0.f);
		p_scale_x_->SetProgressValue(0.f);
		p_scale_x_->SetProgressValue(0.f);
    }
}

void DX11RenderEngine::InitLightDetail()
{
	p_light_detail_ = new QWidget(ui.tbx_detail);
	ui.tbx_detail->addItem(p_light_detail_, "Light");
	p_light_property_ = new QTableWidget(p_light_detail_);
	p_light_property_->setGeometry(10, 10, 333, 175);
	p_light_property_->setRowCount(5);
	p_light_property_->setColumnCount(2);
	QStringList s;
	s << "Property" << "Value";
	p_light_property_->setHorizontalHeaderLabels(s);
	p_light_property_->horizontalHeader()->setVisible(true);
	p_light_property_->verticalHeader()->setVisible(false);
	p_light_property_->horizontalHeader()->setMinimumSectionSize(150);
	p_light_property_->verticalScrollBar()->setVisible(false);
	p_light_property_->verticalHeader()->setMinimumSectionSize(27);
	//p_light_property_->verticalHeader()->setStretchLastSection(true);
	p_light_property_->horizontalHeader()->setStretchLastSection(true);
	p_light_property_->setSelectionBehavior(QAbstractItemView::SelectItems);
	p_light_property_->setSelectionMode(QAbstractItemView::NoSelection);
	p_light_property_->setCellWidget(0, 0, new QLabel("Color", ui.tbx_detail));
	dynamic_cast<QLabel*>(p_light_property_->cellWidget(0, 0))->setAlignment(Qt::AlignCenter);
	p_light_color_ = new QPushButton(ui.tbx_detail);
	p_light_property_->setCellWidget(0, 1, p_light_color_);
	p_light_color_->setStyleSheet("background-color: rgb(255, 255, 255);border-radius:10px;");
	connect(p_light_color_, &QPushButton::clicked, [=]() {
		QColor color = QColorDialog::getColor(Qt::red);
		p_light_color_->setStyleSheet("background-color:" + QString("rgb(%1,%2,%3);").arg(color.red()).arg(color.green()).arg(color.blue()) +
            "border-radius:10px;");
		ui.renderView->SetLightProperty(color.redF(), color.greenF(), color.blueF(),'0');
		});
	p_light_color_->resize(p_light_property_->cellWidget(0, 0)->width(), p_light_property_->cellWidget(0, 0)->height());
	//强度
	p_light_property_->setCellWidget(1, 0, new QLabel("Intensity", ui.tbx_detail));
	dynamic_cast<QLabel*>(p_light_property_->cellWidget(1, 0))->setAlignment(Qt::AlignCenter);
	p_light_intensity_ = new ProgressLine(ui.tbx_detail, QColor(255, 255, 255), QColor(180, 180, 180),QColor(255, 0, 0), 100.f, 0.f, 20.f);
	p_light_property_->setCellWidget(1, 1, p_light_intensity_);
	connect(p_light_intensity_, &ProgressLine::valueChanged, [this](double val) {
		ui.renderView->SetLightProperty(static_cast<float>(val / 100.f), 0.f, 0.f, '1');
		});
	//范围
	p_light_property_->setCellWidget(2, 0, new QLabel("Radius", ui.tbx_detail));
	dynamic_cast<QLabel*>(p_light_property_->cellWidget(2, 0))->setAlignment(Qt::AlignCenter);
	p_light_radius_ = new ProgressLine(ui.tbx_detail, QColor(255, 255, 255), QColor(180, 180, 180),QColor(255, 0, 0), 1000.f, 0.f, 20.f);
	p_light_property_->setCellWidget(2, 1, p_light_radius_);
	connect(p_light_radius_, &ProgressLine::valueChanged, [this](double val) {
		ui.renderView->SetLightProperty(static_cast<float>(val), 0.f, 0.f, '2');
		});
	p_light_property_->hideRow(2);
	//外角
	p_light_property_->setCellWidget(3, 0, new QLabel("OuterAngle", ui.tbx_detail));
	dynamic_cast<QLabel*>(p_light_property_->cellWidget(3, 0))->setAlignment(Qt::AlignCenter);
	p_light_o_angle_ = new ProgressLine(ui.tbx_detail, QColor(255, 255, 255), QColor(180, 180, 180), QColor(255, 0, 0), 90.f, 0.f, 60.f);
	p_light_property_->setCellWidget(3, 1, p_light_o_angle_);
	connect(p_light_o_angle_, &ProgressLine::valueChanged, [this](double val) {
		ui.renderView->SetLightProperty(static_cast<float>(val), 0.f, 0.f, '4');
		});
	p_light_property_->hideRow(3);
	//内角
	p_light_property_->setCellWidget(4, 0, new QLabel("InnearAngle", ui.tbx_detail));
	dynamic_cast<QLabel*>(p_light_property_->cellWidget(4, 0))->setAlignment(Qt::AlignCenter);
	p_light_i_angle_ = new ProgressLine(ui.tbx_detail, QColor(255, 255, 255), QColor(180, 180, 180), QColor(255, 0, 0), 90.f, 0.f, 45.f);
	p_light_property_->setCellWidget(4, 1, p_light_i_angle_);
	connect(p_light_i_angle_, &ProgressLine::valueChanged, [this](double val) {
		ui.renderView->SetLightProperty(static_cast<float>(val), 0.f, 0.f, '3');
		});
	p_light_property_->hideRow(4);
	p_light_property_->resize(p_light_property_->width(), p_light_property_->height() - 30*3);
	p_light_detail_->setVisible(true);
}

void DX11RenderEngine::InitTransformDetail()
{
	p_location_x_ = new ProgressLine(this, QColor(255, 255, 255), QColor(180, 180, 180),
		QColor(255, 0, 0), 10000.f, -10000.f, 0.f);
	p_location_y_ = new ProgressLine(this, QColor(255, 255, 255), QColor(180, 180, 180),
		QColor(0, 255, 0), 10000.f, -10000.f, 0.f);
	p_location_z_ = new ProgressLine(this, QColor(255, 255, 255), QColor(180, 180, 180),
		QColor(0, 0, 255), 10000.f, -10000.f, 0.f);
	p_rotation_x_ = new ProgressLine(this, QColor(255, 255, 255), QColor(180, 180, 180),
		QColor(255, 0, 0), 360.f, 0.f, 0.f);
	p_rotation_y_ = new ProgressLine(this, QColor(255, 255, 255), QColor(180, 180, 180),
		QColor(0, 255, 0), 360.f, 0.f, 0.f);
	p_rotation_z_ = new ProgressLine(this, QColor(255, 255, 255), QColor(180, 180, 180),
		QColor(0, 0, 255), 360.f, 0.f, 0.f);
	p_scale_x_ = new ProgressLine(this, QColor(255, 255, 255), QColor(180, 180, 180),
		QColor(255, 0, 0), 100.f, 0.f, 1.f);
	p_scale_y_ = new ProgressLine(this, QColor(255, 255, 255), QColor(180, 180, 180),
		QColor(0, 255, 0), 100.f, 0.f, 1.f);
	p_scale_z_ = new ProgressLine(this, QColor(255, 255, 255), QColor(180, 180, 180),
		QColor(0, 0, 255), 100.f, 0.f, 1.f);
	ui.tb_transform->setCellWidget(0, 0, p_location_x_);
	ui.tb_transform->setCellWidget(0, 1, p_location_y_);
	ui.tb_transform->setCellWidget(0, 2, p_location_z_);
	ui.tb_transform->setCellWidget(1, 0, p_rotation_x_);
	ui.tb_transform->setCellWidget(1, 1, p_rotation_y_);
	ui.tb_transform->setCellWidget(1, 2, p_rotation_z_);
	ui.tb_transform->setCellWidget(2, 0, p_scale_x_);
	ui.tb_transform->setCellWidget(2, 1, p_scale_y_);
	ui.tb_transform->setCellWidget(2, 2, p_scale_z_);
	ui.tb_transform->verticalHeader()->setVisible(true);

	connect(p_location_x_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetSelectedObjectTransform({ (float)val,p_location_y_->GetValue(),
			p_location_z_->GetValue() }, char(0));
		});
	connect(p_location_y_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetSelectedObjectTransform({ p_location_x_->GetValue(),
			(float)val,
			p_location_z_->GetValue() }, char(0));
		});
	connect(p_location_z_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetSelectedObjectTransform({ p_location_x_->GetValue(),
			p_location_y_->GetValue(),(float)val }, char(0));
		});
	connect(p_rotation_x_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetSelectedObjectTransform({ (float)val,p_rotation_y_->GetValue(),
			p_rotation_z_->GetValue() }, char(1));
		});
	connect(p_rotation_y_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetSelectedObjectTransform({ p_rotation_x_->GetValue(),
			(float)val,
			p_rotation_z_->GetValue() }, char(1));
		});
	connect(p_rotation_z_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetSelectedObjectTransform({ p_rotation_x_->GetValue(),
			p_rotation_y_->GetValue(),(float)val }, char(1));
		});
	//connect(p_rotation_x_, &ProgressLine::valueChanged, [=](const double& val) {
	//	ui.renderView->SetSelectedObjectTransform({ (float)val,0.f,
	//		0.f }, char(1),EAxisType::kXAxis);
	//	});
	//connect(p_rotation_y_, &ProgressLine::valueChanged, [=](const double& val) {
	//	ui.renderView->SetSelectedObjectTransform({ (float)val,0.f,
	//		0.f}, char(1),EAxisType::kYAxis);
	//	});
	//connect(p_rotation_z_, &ProgressLine::valueChanged, [=](const double& val) {
	//	ui.renderView->SetSelectedObjectTransform({ (float)val,0.f,
	//		0.f }, char(1),EAxisType::kZAxis);
	//	});

	connect(p_scale_x_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetSelectedObjectTransform({ (float)val,p_scale_y_->GetValue(),
			p_scale_z_->GetValue() }, char(2));
		});
	connect(p_scale_y_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetSelectedObjectTransform({ p_scale_x_->GetValue(),
			(float)val,
			p_scale_z_->GetValue() }, char(2));
		});
	connect(p_scale_z_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetSelectedObjectTransform({ p_scale_x_->GetValue(),
			p_scale_y_->GetValue(),(float)val }, char(2));
		});
}

void DX11RenderEngine::InitRenderDetail()
{
	p_cb_visiblity_ = new QCheckBox(this);
	p_cb_shadow_ = new QCheckBox(this);
	ui.tb_render->setCellWidget(0, 1, p_cb_visiblity_);
	ui.tb_render->setCellWidget(1, 1, p_cb_shadow_);
	AdjustRenderProperty();
	connect(p_cb_visiblity_, &QCheckBox::stateChanged, [this](int vis) {
		if (vis == 0)
			ui.renderView->SetRenderProperty(false, p_cb_shadow_->checkState());
		else
			ui.renderView->SetRenderProperty(true, p_cb_shadow_->checkState());
		});
	connect(p_cb_shadow_, &QCheckBox::stateChanged, [this](int vis) {
		if (vis == 0)
			ui.renderView->SetRenderProperty(p_cb_visiblity_->checkState(),false);
		else
			ui.renderView->SetRenderProperty(p_cb_visiblity_->checkState(),true);
		});
	p_shadow_bias_ = new ProgressLine(this,1.f, 0.f, 0.002f);
	p_shadow_light_far_ = new ProgressLine(this,10000.f, 0.f, 1000.f);
	p_shadow_light_near_ = new ProgressLine(this,500.f, 0.f, 1.f);
	p_shadow_light_size_ = new ProgressLine(this,100.f, 0.f, 0.4f);
	ui.tb_render->setCellWidget(3, 1, p_shadow_bias_);
	ui.tb_render->setCellWidget(5, 1, p_shadow_light_far_);
	ui.tb_render->setCellWidget(4, 1, p_shadow_light_near_);
	ui.tb_render->setCellWidget(2, 1, p_shadow_light_size_);
	connect(p_shadow_bias_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetShadowProperty(-1,-1,-1,val);
		});
	connect(p_shadow_light_far_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetShadowProperty(val, -1,-1, -1);
		});	
	connect(p_shadow_light_near_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetShadowProperty(-1, val,-1,- 1);
		});
	connect(p_shadow_light_size_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetShadowProperty(-1, -1, val,-1);
		});
}

void DX11RenderEngine::InitMaterialDetail()
{
	p_bt_albedo_ = new QPushButton(ui.tb_material);
	ui.tb_material->setCellWidget(0, 1, p_bt_albedo_);
	p_bt_albedo_->setStyleSheet("background-color: rgb(255, 219, 145);border-radius:10px;");
	connect(p_bt_albedo_, &QPushButton::clicked, [=]() {
		qDebug() << "clicked";
		QColor color = QColorDialog::getColor(Qt::red);
		p_bt_albedo_->setStyleSheet("background-color:" + QString("rgb(%1,%2,%3);").arg(color.red()).arg(color.green()).arg(color.blue()) +
			"border-radius:10px;");
		ui.renderView->SetMaterialProperty(CusMath::vector3d{ color.redF(), color.greenF(), color.blueF()}, -1.f, -1.f);
		});
	p_pl_metallic_ = new ProgressLine(ui.tb_material, 1.f, 0.f, 0.f);
	ui.tb_material->setCellWidget(1, 1, p_pl_metallic_);
	connect(p_pl_metallic_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetMaterialProperty(CusMath::vector3d{-1.f,0.f,0.f},static_cast<float>(val),-1);
		});
	p_pl_roughness_ = new ProgressLine(ui.tb_material, 1.f, 0.f, 0.1f);
	ui.tb_material->setCellWidget(2, 1, p_pl_roughness_);
	connect(p_pl_roughness_, &ProgressLine::valueChanged, [=](const double& val) {
		ui.renderView->SetMaterialProperty(CusMath::vector3d{ -1.f,0.f,0.f },-1, static_cast<float>(val));
		});
}

void DX11RenderEngine::AdjustLightProperty(ELightType pre_type, ELightType next_type)
{
	p_light_color_->setStyleSheet("background-color:" + QString("rgb(%1,%2,%3);").arg(static_cast<int>(ui.renderView->graphicsIns->p_scene_light_->light_color_.x * 255.f)).
		arg(static_cast<int>(ui.renderView->graphicsIns->p_scene_light_->light_color_.y*255.f)).arg(static_cast<int>(ui.renderView->graphicsIns->p_scene_light_->light_color_.z*255.f)) +
		"border-radius:10px;");
	p_light_intensity_->SetProgressValue(100.f* static_cast<double>(ui.renderView->graphicsIns->p_scene_light_->light_intensity_ ));
	switch (pre_type)
	{
	case ELightType::kPonintLight:
	{
		if (next_type == ELightType::kPonintLight)
		{
			p_light_radius_->SetProgressValue(static_cast<double>(ui.renderView->graphicsIns->p_scene_light_->affect_radius_));
			return;
		}
		else if (next_type == ELightType::kDirectionLight)
		{
			p_light_property_->hideRow(2);
			p_light_property_->resize(p_light_property_->width(), p_light_property_->height() - 30);
			return;
		}
		else if (next_type == ELightType::kSpotLight)
		{
			p_light_property_->showRow(3);
			p_light_property_->showRow(4);
			p_light_radius_->SetProgressValue(static_cast<double>(ui.renderView->graphicsIns->p_scene_light_->affect_radius_));
			p_light_i_angle_->SetProgressValue(static_cast<double>(ui.renderView->graphicsIns->p_scene_light_->inner_angle_));
			p_light_o_angle_->SetProgressValue(static_cast<double>(ui.renderView->graphicsIns->p_scene_light_->outer_angle));
			p_light_property_->resize(p_light_property_->width(), p_light_property_->height() + 30*2);
			return;
		}
	}
		break;
	case ELightType::kDirectionLight:
	{
		if (next_type == ELightType::kPonintLight)
		{
			p_light_property_->showRow(2);
			p_light_radius_->SetProgressValue(static_cast<double>(ui.renderView->graphicsIns->p_scene_light_->affect_radius_));
			p_light_property_->resize(p_light_property_->width(), p_light_property_->height() + 30);
			return;
		}
		else if (next_type == ELightType::kDirectionLight)
		{
			return;
		}
		else if (next_type == ELightType::kSpotLight)
		{
			p_light_property_->showRow(2);
			p_light_property_->showRow(3);
			p_light_property_->showRow(4);
			p_light_radius_->SetProgressValue(static_cast<double>(ui.renderView->graphicsIns->p_scene_light_->affect_radius_));
			p_light_i_angle_->SetProgressValue(static_cast<double>(ui.renderView->graphicsIns->p_scene_light_->inner_angle_));
			p_light_o_angle_->SetProgressValue(static_cast<double>(ui.renderView->graphicsIns->p_scene_light_->outer_angle));
			p_light_property_->resize(p_light_property_->width(), p_light_property_->height() + 30 * 3);
			return;
		}
	}
		break;
	case ELightType::kSpotLight:
	{
		if (next_type == ELightType::kPonintLight)
		{
			p_light_property_->hideRow(3);
			p_light_property_->hideRow(4);
			p_light_radius_->SetProgressValue(static_cast<double>(ui.renderView->graphicsIns->p_scene_light_->affect_radius_));
			p_light_property_->resize(p_light_property_->width(), p_light_property_->height() - 30*2);
			return;
		}
		else if (next_type == ELightType::kDirectionLight)
		{
			p_light_property_->hideRow(2);
			p_light_property_->hideRow(3);
			p_light_property_->hideRow(4);
			p_light_property_->resize(p_light_property_->width(), p_light_property_->height() - 30 * 3);
			return;
		}
		else if (next_type == ELightType::kSpotLight)
		{
			p_light_radius_->SetProgressValue(static_cast<double>(ui.renderView->graphicsIns->p_scene_light_->affect_radius_));
			p_light_i_angle_->SetProgressValue(static_cast<double>(ui.renderView->graphicsIns->p_scene_light_->inner_angle_));
			p_light_o_angle_->SetProgressValue(static_cast<double>(ui.renderView->graphicsIns->p_scene_light_->outer_angle));
			return;
		}
	}
		break;
	default:
		break;
	}
}

void DX11RenderEngine::AdjustRenderProperty()
{
	bool vis;
	bool shadow;
	p_cb_visiblity_->blockSignals(true);
	p_cb_shadow_->blockSignals(true);
	ui.renderView->GetRenderProperty(vis, shadow);
	if (vis)
		p_cb_visiblity_->setCheckState(Qt::CheckState::Checked);
	else
		p_cb_visiblity_->setCheckState(Qt::CheckState::Unchecked);
	if (shadow)
		p_cb_shadow_->setCheckState(Qt::CheckState::Checked);
	else
		p_cb_shadow_->setCheckState(Qt::CheckState::Unchecked);
	p_cb_visiblity_->blockSignals(false);
	p_cb_shadow_->blockSignals(false);
}

