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




DX11RenderEngine::DX11RenderEngine(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setFixedSize(1280, 720);

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
		ui.renderView->AddLight('1');
		});
	connect(ui.bt_addPointL, &QPushButton::clicked, [=]() {
		ui.renderView->AddLight('0');
		});
	connect(ui.bt_addSpotL, &QPushButton::clicked, [=]() {
		ui.renderView->AddLight('2');
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

    //模型变换绑定
    connect(ui.dSB_MTX, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform({ (float)val,(float)ui.dSB_MTY->value(),
            (float)ui.dSB_MTZ->value() }, char(0));
        });
    connect(ui.dSB_MTY, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform({ (float)ui.dSB_MTX->value(),(float)val,
            (float)ui.dSB_MTZ->value() }, char(0));
        });
    connect(ui.dSB_MTZ, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform({ 
            (float)ui.dSB_MTX->value(),(float)ui.dSB_MTY->value(),(float)val }, char(0));
        });
    connect(ui.dSB_MRX, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform({
           (float)val,(float)ui.dSB_MRY->value(),(float)ui.dSB_MRZ->value() }, char(1));
        });
    connect(ui.dSB_MRY, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform({
           (float)ui.dSB_MRX->value(),(float)val,(float)ui.dSB_MRZ->value() }, char(1));
        });
    connect(ui.dSB_MRZ, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform({
           (float)ui.dSB_MRX->value(),(float)ui.dSB_MRY->value(),(float)val }, char(1));
        });
    connect(ui.dSB_MSX, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform(
            { (float)val, (float)ui.dSB_MSY->value(),(float)ui.dSB_MSZ->value() }, char(2));
        });
    connect(ui.dSB_MSY, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform(
            { (float)ui.dSB_MSX->value(),(float)val, (float)ui.dSB_MSZ->value() }, char(2));
        });
    connect(ui.dSB_MSZ, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform(
            { (float)ui.dSB_MSX->value(),(float)ui.dSB_MSY->value(),(float)val, }, char(2));
        });
    //transform
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
    
    //选择背景颜色对话框
    connect(ui.bt_ChooseBC, &QPushButton::clicked, [=]() {
        static float bgc[4] = { 0.f,0.f,0.f,1.f };
        QColor color = QColorDialog::getColor(Qt::black, this, "choose color");
        bgc[0] = color.redF();
        bgc[1] = color.greenF();
        bgc[2] = color.blueF();
        ui.renderView->SetbgColor(bgc);
        });

    //初始化dx渲染视口
    ui.renderView->InitialViewport();
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
	ui.dSB_MTX->blockSignals(true);
	ui.dSB_MTY->blockSignals(true);
	ui.dSB_MTZ->blockSignals(true);
	ui.dSB_MRX->blockSignals(true);
	ui.dSB_MRY->blockSignals(true);
	ui.dSB_MRZ->blockSignals(true);
	ui.dSB_MSX->blockSignals(true);
	ui.dSB_MSY->blockSignals(true);
	ui.dSB_MSZ->blockSignals(true);
    if (row > -1)
    {
        double tranf[9];
        ui.renderView->SetSelectObject(row + 1, tranf);
		ui.dSB_MTX->setValue(tranf[0]);
		ui.dSB_MTY->setValue(tranf[1]);
		ui.dSB_MTZ->setValue(tranf[2]);
		ui.dSB_MRX->setValue(tranf[3]);
		ui.dSB_MRY->setValue(tranf[4]);
		ui.dSB_MRZ->setValue(tranf[5]);
		ui.dSB_MSX->setValue(tranf[6]);
		ui.dSB_MSY->setValue(tranf[7]);
		ui.dSB_MSZ->setValue(tranf[8]);
    }
    else
    {
        ui.renderView->SetSelectObject(-1, nullptr);
		ui.dSB_MTX->setValue(0.f);
		ui.dSB_MTY->setValue(0.f);
		ui.dSB_MTZ->setValue(0.f);
		ui.dSB_MRX->setValue(0.f);
		ui.dSB_MRY->setValue(0.f);
		ui.dSB_MRZ->setValue(0.f);
		ui.dSB_MSX->setValue(1.f);
		ui.dSB_MSY->setValue(1.f);
		ui.dSB_MSZ->setValue(1.f);
    }
	ui.dSB_MTX->blockSignals(false);
	ui.dSB_MTY->blockSignals(false);
	ui.dSB_MTZ->blockSignals(false);
	ui.dSB_MRX->blockSignals(false);
	ui.dSB_MRY->blockSignals(false);
	ui.dSB_MRZ->blockSignals(false);
	ui.dSB_MSX->blockSignals(false);
	ui.dSB_MSY->blockSignals(false);
	ui.dSB_MSZ->blockSignals(false);

}