#include "v_main_window.h"
#include <QApplication>
#include <Inventor/Qt/SoQt.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VMainWindow w;
    w.show();

    return a.exec();
}
