#include "PeakMeter.h"
#include <cmath>
#include <QDebug>
#include <QResizeEvent>
#include <QStyleOption>

//const QColor PeakMeter::DEFAULT_BORDER_COLOR = QColor(0, 0, 0);
//const QColor PeakMeter::DEFAULT_BG_COLOR = Qt::gray;

using namespace Gui;

PeakMeter::PeakMeter(QWidget *){
    gradient = QLinearGradient(0, 0, 0, height()-1);
    gradient.setColorAt(0, Qt::red);
    gradient.setColorAt(0.4, QColor(0, 200, 0));
    gradient.setColorAt(0.8, Qt::darkGreen);
    //gradient.setSpread(QGradient::ReflectSpread);

    //borderColor = DEFAULT_BORDER_COLOR;
    //bgColor = DEFAULT_BG_COLOR;
    decayTime = 2500;
    update();
}

void PeakMeter::setPeak(float peak)
{
    if(peak > this->peak){
        this->peak = peak;
    }
    update();
}

void PeakMeter::paintEvent(QPaintEvent *){
    QPainter painter(this);

    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    //background
    //painter.fillRect(0, 0, width()-1, height()-1, bgColor);

    //meter
    if(isEnabled()){
        float top = (1 - peak) * height();
        int h = height() - top - 1;
        painter.fillRect(1, top , width()-2, h-1, gradient);
    }

    //border
    //painter.setPen(borderColor);
    //painter.drawRect(0, 0, width()-1, height()-1);

    long ellapsedTime = QDateTime::currentMSecsSinceEpoch() - lastUpdate;
    peak -= (float)ellapsedTime/decayTime;
    if(peak < 0){
        peak = 0;
    }

    lastUpdate = QDateTime::currentMSecsSinceEpoch();



}

void PeakMeter::resizeEvent ( QResizeEvent * /*ev*/)  {
    gradient.setFinalStop(0, height()-1);
}