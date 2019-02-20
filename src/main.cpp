/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <QApplication>
#include <QTranslator>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QProcess>

#include <iostream>
#include <deque>

#include "VDefinitions.h"
#include "VWindowMain.h"
#include "VDatabaseResin.h"
#include "VDatabaseCloth.h"

bool findSQLfile(QString &sqlFile, const char * argv0);
bool createDatabase(const QString &sqlFile);

int main(int argc, char *argv[])
{
    VDatabaseResin resinInteracror;
    VDatabaseResin clothInteracror;
    bool dbCreationNeeded;
    try
    {
        std::deque<QString> names;
        resinInteracror.getNames(names, false);
        bool hasResins = (names.size() != 0);
        clothInteracror.getNames(names, false);
        bool hasCloths = (names.size() != 0);
        dbCreationNeeded = !hasCloths && !hasResins;
    }
    catch(DatabaseException)
    {
            dbCreationNeeded = true;
    }
    if (dbCreationNeeded)
    {
        QString sqlFile;
        const char * argv0 = (argc > 0) ? argv[0] : nullptr;
        bool sqlFileFound = findSQLfile(sqlFile, argv0);
        if (sqlFileFound)
        {
            bool result = createDatabase(sqlFile);
            if (result)
                std::cout << "Successfully updated the database" << std::endl;
            else
                std::cerr << "Error occurred when updating the database" << std::endl;
        }
        else
            std::cerr << "Can not locate 'create.sql' file" << std::endl;
    }
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

bool findSQLfile(QString &sqlFile, const char * argv0)
{
    const QString sqlCreateFilename(QStringLiteral("create.sql"));
    QString exeDir;
    if (argv0 != nullptr)
        exeDir = QFileInfo(argv0).absoluteDir().absolutePath();
    else
        exeDir = QDir::currentPath();
    QStringList directories = {
        QDir::cleanPath(exeDir + QStringLiteral("/sql")),
        QDir::cleanPath(exeDir + QStringLiteral("/../Resources")),
        QDir::cleanPath(exeDir + QStringLiteral("/../share/vari/sql")),
        QStringLiteral("/usr/share/vari/sql")
    };
    foreach(const QString &dirpath, directories)
    {
        QString filePath = QDir::cleanPath(dirpath + QDir::separator() + sqlCreateFilename);
        if (QFile::exists(filePath))
        {
            sqlFile = filePath;
            return true;
        }
    }
    return false;
}

bool createDatabase(const QString &sqlFile)
{
    const QString psql(QStringLiteral("psql"));
    const QStringList psqlArgs =
    {
        QStringLiteral("-h"),
        QStringLiteral("127.0.0.1"),
        QStringLiteral("-U"),
        QStringLiteral("postgres"),
        QStringLiteral("-f"),
        sqlFile
    };
    return (QProcess::execute(psql, psqlArgs) == 0);
}
