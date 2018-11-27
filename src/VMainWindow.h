#ifndef V_MAIN_WINDOW_H
#define V_MAIN_WINDOW_H

#include <QMainWindow>
#include <memory>

namespace Ui {
class VMainWindow;
}
class VSimulationFacade;

class VMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VMainWindow(QWidget *parent = 0);
    ~VMainWindow();

private:
    Ui::VMainWindow *ui;
    std::unique_ptr<VSimulationFacade> m_facade;


private slots:
    void on_pushButton_clicked();
};

#endif // V_MAIN_WINDOW_H
