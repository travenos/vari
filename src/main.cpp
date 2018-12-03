#include "VWindowMain.h"
#include <QApplication>
#include <Inventor/Qt/SoQt.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VWindowMain w;
    w.show();

    return a.exec();
}
