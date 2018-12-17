#include "VWindowMain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //TODO set application information: name, version, vendor, website
    //Set russion translation for all elements
    QLocale::setDefault(QLocale::C);
    QApplication a(argc, argv);
    VWindowMain w;
    w.show();

    return a.exec();
}
