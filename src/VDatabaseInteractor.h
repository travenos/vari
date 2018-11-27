#ifndef DATABASEINTERACTOR_H
#define DATABASEINTERACTOR_H

#include <QSqlDatabase>
#include <exception>
#include <deque>

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

class DatabaseInteractor
{
private:
    mutable QSqlDatabase m_database;

    static const QString OPEN_ERROR_STRING;
    static const QString FILE_ERROR_STRING;
    static const QString HOSTNAME;
    static const QString DATABASENAME;
    static const QString USERNAME;
    static const QString PASSWORD;
    static const QString GET_NAME_QUERY;
    static const QString GET_INFO_QUERY;
    static const QString UPDATE_INFO_QUERY;
    static const QString INSERT_INFO_QUERY;
    static const QString DELETE_BY_ID_QUERY;
    static const QString COPY_FROM_FILE_QUERY;
    static const QString COPY_TO_FILE_QUERY;

    void basicOperation(const QString &queryString) const noexcept(false);

public:
    DatabaseInteractor();
    void getNames(std::deque<QString> &outputDeque, bool sort = true) const noexcept(false);
    //TODO struct "VMaterial"
    void materialInfo(const QString &name, int &id, float &cavityheight, float &permability, float &porosity) const noexcept(false);
    //TODO struct "VMaterial"
    void saveMaterial(const QString &name, int id, float cavityheight, float permability, float porosity) noexcept(false);
    void removeMaterial(int id) noexcept(false);
    void saveToFile(const QString &fileName) const noexcept(false);
    void loadFromFile(const QString &fileName) noexcept(false);

};

#endif // DATABASEINTERACTOR_H
