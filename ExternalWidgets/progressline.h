#ifndef PROGRESSLINE_H
#define PROGRESSLINE_H

#include <QWidget>

namespace Ui {
class ProgressLine;
}

class ProgressLine : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor back_color READ GetBackgroundColor WRITE SetBackgroundColor)
    Q_PROPERTY(QColor chunk_color READ GetChunkColor WRITE SetChunkColor)
    Q_PROPERTY(QColor border_color READ GetBorderColor WRITE SetBorderColor)
    Q_PROPERTY(double max READ GetProgressMax WRITE SetProgressMax)
    Q_PROPERTY(double min READ GetProgressMin WRITE SetProgressMin)
    Q_PROPERTY(double value READ GetProgressValue WRITE SetProgressValue)
public:
    explicit ProgressLine(QWidget *parent = nullptr);
    explicit ProgressLine(QWidget *parent,QColor back_color,QColor chunk_color,QColor border_color,double max,double min,double defalut);
    explicit ProgressLine(QWidget *parent,double max,double min,double defalut);
    ~ProgressLine();
    QColor GetBackgroundColor();
    void SetBackgroundColor(QColor color);
    QColor GetChunkColor();
    void SetChunkColor(QColor color);
    QColor GetBorderColor();
    void SetBorderColor(QColor color);
    double GetProgressMax();
    void SetProgressMax(double val);
    double GetProgressMin();
    void SetProgressMin(double val);
    double GetProgressValue();
    void SetProgressValue(double val);
    void SetValue(float val);
    float GetValue();
    virtual void mouseMoveEvent(QMouseEvent* e) override;
    virtual void mousePressEvent(QMouseEvent* e) override;
    virtual void mouseReleaseEvent(QMouseEvent* e) override;
    virtual void paintEvent(QPaintEvent* p) override;

signals:
    void valueChanged(double val);
private slots:
    void on_lineEdit_editingFinished();

private:
    Ui::ProgressLine *ui;
    QColor back_color_;
    QColor chunk_color_;
    QColor border_color_;
    float max_;
    float min_;
    float value_;
};

#endif // PROGRESSLINE_H
