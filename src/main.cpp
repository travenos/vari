/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <QApplication>
#include <QTranslator>

#include <iostream>
#include <deque>

#include "VDefinitions.h"
#include "VWindowMain.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(QStringLiteral(ORGANIZATION_NAME));
    QCoreApplication::setOrganizationDomain(QStringLiteral(ORG_WEBSITE));
    QCoreApplication::setApplicationName(QStringLiteral(APPLICATION_NAME));
    QCoreApplication::setApplicationVersion(QStringLiteral(APPLICATION_VERSION));

    QLocale::setDefault(QLocale::C);
    QApplication a(argc, argv);
    QTranslator translator;
    translator.load("qtbase_ru", QStringLiteral(":/"));
    a.installTranslator(&translator);
    VWindowMain w;
    w.show();

    return a.exec();
}
