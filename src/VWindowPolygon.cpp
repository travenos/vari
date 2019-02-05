/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include "VWindowPolygon.h"
#include "ui_VWindowPolygon.h"

VWindowPolygon::VWindowPolygon(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VWindowPolygon)
{
    ui->setupUi(this);
}

VWindowPolygon::~VWindowPolygon()
{
    delete ui;
    #ifdef DEBUG_MODE
        qInfo() << "VWindowPolygon destroyed";
    #endif
}

void VWindowPolygon::accept()
{
    hide();
    emit polygonAvailable(m_polygon);
    reset();
    close();
}

void VWindowPolygon::reset()
{
    m_polygon.clear();
    //TODO : clear widget
}

void VWindowPolygon::reject()
{
    hide();
    emit creationCanceled();
    reset();
    close();
}

void VWindowPolygon::closeEvent(QCloseEvent *)
{
    emit windowClosed();
}

void VWindowPolygon::on_buttonBox_rejected()
{
    reject();
}

void VWindowPolygon::on_buttonBox_accepted()
{
    accept();
}

