#include <QDebug> //TODO temporary
#include "VWindowMain.h"
#include "ui_VWindowMain.h"
#include "sim/VSimulationFacade.h"

VWindowMain::VWindowMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VWindowMain)
{
    ui->setupUi(this);
    m_facade.reset(new VSimulationFacade(ui->viewerWidget));
}

VWindowMain::~VWindowMain()
{
    delete ui;
}

void VWindowMain::on_pushButton_clicked()
{

}
