/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VSQLDATABASE_H
#define _VSQLDATABASE_H

#include <QSqlDatabase>

class QString;

class VSqlDatabase
{
private:
    explicit VSqlDatabase();
    ~VSqlDatabase();
    VSqlDatabase(const VSqlDatabase &other) = delete;
    VSqlDatabase & operator=(const VSqlDatabase &other) = delete;

    QSqlDatabase m_database;
    QString m_databaseLocation;
    static VSqlDatabase * s_object;
public:
    static const QString HOSTNAME;
    static const QString DATABASENAME;
    static const QString USERNAME;
    static const QString PASSWORD;
    static const QString CONNECTION_NAME;

    static VSqlDatabase * getInstance();
    static void deleteInstance();
    static bool hasInstance();

    bool open();
    bool isOpen() const;
    void close();

    void setDatabaseName(const QString &name);
    void setHostName(const QString &host);
    void setPassword(const QString &password);
    void setUserName(const QString &name);
    void resetParameters();
    QSqlDatabase & getDatabase();

    bool createDirIfNecessary();
};

#endif // _VSQLDATABASE_H
