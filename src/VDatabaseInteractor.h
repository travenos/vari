#ifndef _VDATABASEINTERACTOR_H
#define _VDATABASEINTERACTOR_H

#include <exception>
#include <deque>
#include "VSqlDatabase.h"

class QString;

class DatabaseException: public std::exception {
    const QString m_msg;
public:
    DatabaseException(const QString &msg):
        m_msg(msg)
    {}
    const QString &what()
    {return m_msg;}
};

class VDatabaseInteractor
{
private:
    const QString m_tableName;

protected:
    static const QString OPEN_ERROR_STRING;
    static const QString FILE_ERROR_STRING;
    static const QString GET_NAME_QUERY;
    static const QString DELETE_BY_ID_QUERY;
    static const QString COPY_FROM_FILE_QUERY;
    static const QString COPY_TO_FILE_QUERY;

    VSqlDatabase* databaseInstance() const ;

    void basicOperation(const QString &queryString) const ;

public:
    VDatabaseInteractor(const QString &tableName);
    void getNames(std::deque<QString> &outputDeque, bool sort = true) const ;
    void removeMaterial(int id) ;
    void saveToFile(const QString &fileName) const ;
    void loadFromFile(const QString &fileName) ;

};

#endif // _VDATABASEINTERACTOR_H
