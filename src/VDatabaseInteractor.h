/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VDATABASEINTERACTOR_H
#define _VDATABASEINTERACTOR_H

#include <QObject>

#include <exception>
#include <deque>
#include "VSqlDatabase.h"

class QString;

class DatabaseException: public std::exception
{
    const QString m_msg;
public:
    DatabaseException(const QString &msg):
        m_msg(msg)
    {}
    const QString &what()
    {return m_msg;}
};

class VDatabaseInteractor : public QObject
{
    Q_OBJECT
public:
    VDatabaseInteractor(const QString &tableName);
    virtual ~VDatabaseInteractor();
    void loadNames(bool sort=true);
    const std::deque<QString> &getNames() const;
    void removeMaterial(int id) ;
    virtual void saveToFile(const QString &fileName) const ;
    virtual void loadFromFile(const QString &fileName) ;
    bool findSQLfile(QString *sqlFile = nullptr) const;
    void createDatabase(const QString &postgresPassword = QStringLiteral("")) ;
    void executeQueriesFromFile(const QString &filename, QSqlDatabase &db) ;
protected:
    static const QString OPEN_ERROR_STRING;
    static const QString FILE_ERROR_STRING;
    static const QString NAME_ERROR_STRING;
    static const QString SQLFILE_ERROR_STRING;
    static const QString GET_NAME_QUERY;
    static const QString DELETE_BY_ID_QUERY;
    static const QString COPY_FROM_FILE_QUERY;
    static const QString COPY_TO_FILE_QUERY;

    VSqlDatabase* databaseInstance() const ;
    std::deque<QString> m_namesDeque ;

    void basicOperation(const QString &queryString) const ;

private:
    const QString m_tableName;

signals:
    void needsToLoadDB();
};

#endif // _VDATABASEINTERACTOR_H
