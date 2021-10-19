#include "progressline.h"
#include "ui_progressline.h"
#include "QMouseEvent"
#include "QCursor"
#include "QDebug"

namespace  {
    int pos_x = 0;
    int pos_y = 0;
    bool left_pressed = false;
    int g_x = 0;
}
ProgressLine::ProgressLine(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgressLine)
{
    ui->setupUi(this);
    setMouseTracking(true);
    ui->progressBar->setTextVisible(true);
    ui->lineEdit->setVisible(false);
    border_color_ = QColor(255,0,0);
    back_color_ = QColor(255,255,255);
    chunk_color_ = QColor(180,180,180);
    ui->progressBar->setStyleSheet(
    "QProgressBar\
    {\
    border:1px solid #000000;\
    border-left:8px solid "+QString("rgb(%1, %2, %3)").arg(border_color_.red()).arg(border_color_.green()).arg(border_color_.blue()) +";"\
    "border-radius:10px;\
    background-color:"+QString("rgb(%1, %2, %3)").arg(back_color_.red()).arg(back_color_.green()).arg(back_color_.blue()) +
    "}\
    QProgressBar::chunk\
    {\
    background-color:"+QString("rgb(%1, %2, %3)").arg(chunk_color_.red()).arg(chunk_color_.green()).arg(chunk_color_.blue())+ ";\
    border-radius:8px;\
    }");
    value_ = 0.f;
    min_ = 0.f;
    max_ = 360.f;
    ui->progressBar->setFormat(QString("%1").arg(QString::number(value_,'f',2)));
    ui->progressBar->setValue(value_);
    ui->lineEdit->setText(QString("%1").arg(QString::number(value_,'f',2)));
    ui->progressBar->setMaximum(max_);
    ui->progressBar->setMinimum(min_);
}

ProgressLine::ProgressLine(QWidget *parent, QColor back_color, QColor chunk_color, QColor border_color, double max, double min, double defalut)
: QWidget(parent),
    ui(new Ui::ProgressLine)
{
    ui->setupUi(this);
	setMouseTracking(true);
	ui->progressBar->setTextVisible(true);
	ui->lineEdit->setVisible(false);
    back_color_ = back_color;
	chunk_color_ = chunk_color;
	border_color_ = border_color;
	max_ = max;
	min_ = min;
	value_ = defalut;
	ui->progressBar->setStyleSheet(
		"QProgressBar\
    {\
    border:1px solid #000000;\
    border-left:8px solid " + QString("rgb(%1, %2, %3)").arg(border_color_.red()).arg(border_color_.green()).arg(border_color_.blue()) + ";"\
		"border-radius:10px;\
    background-color:" + QString("rgb(%1, %2, %3)").arg(back_color_.red()).arg(back_color_.green()).arg(back_color_.blue()) +
		"}\
    QProgressBar::chunk\
    {\
    background-color:" + QString("rgb(%1, %2, %3)").arg(chunk_color_.red()).arg(chunk_color_.green()).arg(chunk_color_.blue()) + ";\
    border-radius:8px;\
    }");
    ui->progressBar->setFormat(QString("%1").arg(QString::number(value_,'f',2)));
    ui->progressBar->setValue(value_);
    ui->lineEdit->setText(QString("%1").arg(QString::number(value_,'f',2)));
    ui->progressBar->setMaximum(max_);
    ui->progressBar->setMinimum(min_);
}

ProgressLine::ProgressLine(QWidget* parent, double max, double min, double defalut) : ProgressLine(parent,QColor(255,255,255),QColor(180,180,180),QColor(255,0,0),max,min,defalut)
{
}

ProgressLine::~ProgressLine()
{
    delete ui;
}

QColor ProgressLine::GetBackgroundColor()
{
    return back_color_;
}

void ProgressLine::SetBackgroundColor(QColor color)
{
    back_color_ = color;
    ui->progressBar->setStyleSheet("QProgressBar\
                                  {\
                                      border:1px solid #000000;\
                                      border-left:8px solid "+QString("rgb(%1, %2, %3)").arg(border_color_.red()).arg(border_color_.green()).arg(border_color_.blue()) +";"\
                                      "border-radius:10px;\
                                      background-color:"+QString("rgb(%1, %2, %3)").arg(back_color_.red()).arg(back_color_.green()).arg(back_color_.blue()) +
                                  "}\
                                  QProgressBar::chunk\
                                  {\
                                      background-color:"+QString("rgb(%1, %2, %3)").arg(chunk_color_.red()).arg(chunk_color_.green()).arg(chunk_color_.blue())+ ";\
                                      border-radius:8px;\
                                  }");
}

QColor ProgressLine::GetChunkColor()
{
    return chunk_color_;
}

void ProgressLine::SetChunkColor(QColor color)
{
    chunk_color_ = color;
   ui->progressBar->setStyleSheet("QProgressBar\
                                  {\
                                      border:1px solid #000000;\
                                      border-left:8px solid "+QString("rgb(%1, %2, %3)").arg(border_color_.red()).arg(border_color_.green()).arg(border_color_.blue()) +";"\
                                      "border-radius:10px;\
                                      background-color:"+QString("rgb(%1, %2, %3)").arg(back_color_.red()).arg(back_color_.green()).arg(back_color_.blue()) +
                                  "}\
                                  QProgressBar::chunk\
                                  {\
                                      background-color:"+QString("rgb(%1, %2, %3)").arg(chunk_color_.red()).arg(chunk_color_.green()).arg(chunk_color_.blue())+ ";\
                                      border-radius:8px;\
                                  }");
}

QColor ProgressLine::GetBorderColor()
{
    return border_color_;
};

void ProgressLine::SetBorderColor(QColor color)
{
    border_color_ = color;
   ui->progressBar->setStyleSheet("QProgressBar\
                                  {\
                                      border:1px solid #000000;\
                                      border-left:8px solid "+QString("rgb(%1, %2, %3)").arg(border_color_.red()).arg(border_color_.green()).arg(border_color_.blue()) +";"\
                                      "border-radius:10px;\
                                      background-color:"+QString("rgb(%1, %2, %3)").arg(back_color_.red()).arg(back_color_.green()).arg(back_color_.blue()) +
                                  "}\
                                  QProgressBar::chunk\
                                  {\
                                      background-color:"+QString("rgb(%1, %2, %3)").arg(chunk_color_.red()).arg(chunk_color_.green()).arg(chunk_color_.blue())+ ";\
                                      border-radius:8px;\
                                                   }");}
double ProgressLine::GetProgressMax()
{
    return max_;
}

void ProgressLine::SetProgressMax(double val)
{
    max_ = val;
    ui->progressBar->setMaximum(max_);
}

double ProgressLine::GetProgressMin()
{
    return min_;
}

void ProgressLine::SetProgressMin(double val)
{
    min_ =val;
   ui->progressBar->setMinimum(min_);
}

double ProgressLine::GetProgressValue()
{
   return value_;
}

void ProgressLine::SetProgressValue(double val)
{
    value_ = val;
    ui->progressBar->setFormat(QString("%1").arg(QString::number(value_,'f',2)));
    ui->progressBar->setValue(value_);
    ui->lineEdit->setText(QString("%1").arg(QString::number(value_,'f',2)));
}

void ProgressLine::SetValue(float val)
{
    value_ = val;
}

float ProgressLine::GetValue()
{
    return value_;
}

void ProgressLine::mouseMoveEvent(QMouseEvent *e)
{
   float speed =0.1f + abs(value_)* (max_ - min_) * 0.0000001f;
   if (left_pressed)  //左键按下
   {
       this->setCursor(Qt::BlankCursor);
       auto dis =  mapToGlobal(e->pos()).x() -g_x;
       value_+=dis * speed;
       if( dis> 0)
       {
           value_ = fminf(value_,max_);
           ui->progressBar->setFormat(QString("%1").arg(QString::number(value_,'f',2)));
           ui->progressBar->setValue(value_);
       }
       else if(dis< 0)
       {
           value_ = fmaxf(value_,min_);
           ui->progressBar->setFormat(QString("%1").arg(QString::number(value_,'f',2)));
           ui->progressBar->setValue(value_);
       }
       emit valueChanged(value_);
       g_x = mapToGlobal(e->pos()).x();
       g_x = fmin(1915.f,g_x);
       g_x = fmax(2.f,g_x);
       ui->lineEdit->setText(QString::number(value_));
   }
   else
   {
       this->setCursor(QCursor(Qt::CursorShape::SizeHorCursor));
   }
}

void ProgressLine::mousePressEvent(QMouseEvent *e)
{
    left_pressed = true;
    g_x =  mapToGlobal(e->pos()).x();
    pos_x = e->pos().x();
    pos_y = e->pos().y();
}

void ProgressLine::mouseReleaseEvent(QMouseEvent *e)
{
    left_pressed = false;
    if(e->pos().x()==pos_x && e->pos().y()==pos_y)
    {
        ui->progressBar->setTextVisible(false);
        ui->lineEdit->setVisible(true);
        ui->lineEdit->selectAll();
        ui->lineEdit->setFocus();
    }
    else
   {
      QCursor::setPos(mapToGlobal(QPoint(this->pos().x() - this->width(),this->pos().y()-this->width())));
                                                       }
                                                   }

void ProgressLine::paintEvent(QPaintEvent *p)
{
    ui->progressBar->resize(this->width(),this->height());
   ui->lineEdit->resize(this->width(),this->height());
}

void ProgressLine::on_lineEdit_editingFinished()
{
    value_ = ui->lineEdit->text().toDouble();
    ui->progressBar->setFormat(QString("%1").arg(QString::number(value_,'f',2)));
    ui->progressBar->setValue(value_);
    ui->progressBar->setTextVisible(true);
    ui->lineEdit->setVisible(false);
   emit valueChanged(value_);
}

