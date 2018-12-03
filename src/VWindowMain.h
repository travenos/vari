#ifndef _VWINDOWMAIN_H
#define _VWINDOWMAIN_H

#include <QMainWindow>
#include <memory>

namespace Ui {
class VWindowMain;
}
class VSimulationFacade;

class VWindowMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit VWindowMain(QWidget *parent = nullptr);
    ~VWindowMain();

private:
    Ui::VWindowMain *ui;
    std::unique_ptr<VSimulationFacade> m_facade;


private slots:
    void on_pushButton_clicked();
};

#endif // _VWINDOWMAIN_H
