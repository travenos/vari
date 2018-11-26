#include <QDebug> //TODO temporary
#include "v_main_window.h"
#include "ui_v_main_window.h"
#include "sim/VSimulationFacade.h"

VMainWindow::VMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VMainWindow)
{
    ui->setupUi(this);
    m_facade.reset(new VSimulationFacade(ui->viewerWidget));
}

VMainWindow::~VMainWindow()
{
    delete ui;
}

void VMainWindow::on_pushButton_clicked()
{

}
