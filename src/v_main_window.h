#ifndef V_MAIN_WINDOW_H
#define V_MAIN_WINDOW_H

#include <QMainWindow>

namespace Ui {
class VMainWindow;
}

class VMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VMainWindow(QWidget *parent = 0);
    ~VMainWindow();

private:
    Ui::VMainWindow *ui;
};

#endif // V_MAIN_WINDOW_H
