/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <QApplication>

#include "VDefinitions.h"
#include "VWindowMain.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(QStringLiteral(ORGANIZATION_NAME));
    QCoreApplication::setOrganizationDomain(QStringLiteral(ORG_WEBSITE));
    QCoreApplication::setApplicationName(QStringLiteral(APPLICATION_NAME));
    QCoreApplication::setApplicationVersion(QStringLiteral(APPLICATION_VERSION));
    //Set russian translation for all elements
    QLocale::setDefault(QLocale::C);
    QApplication a(argc, argv);
    VWindowMain w;
    w.show();

    return a.exec();
}
