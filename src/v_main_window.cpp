#include "v_main_window.h"
#include "ui_v_main_window.h"

VMainWindow::VMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VMainWindow)
{
    ui->setupUi(this);
}

VMainWindow::~VMainWindow()
{
    delete ui;
}
