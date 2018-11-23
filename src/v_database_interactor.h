#ifndef DATABASEINTERACTOR_H
#define DATABASEINTERACTOR_H

#include <exception>
#include <QSqlDatabase>
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
public:
    DatabaseInteractor();
    void getNames(std::deque<QString> &outputDeque, bool sort = true) noexcept(false);
    void materialInfo(const QString &name, int &id, float &cavityheight, float &permability, float &porosity) noexcept(false);
    void saveMaterial(const QString &name, int id, float cavityheight, float permability, float porosity) noexcept(false);
    void removeMaterial(int id) noexcept(false);
    void saveToFile(const QString &fileName) noexcept(false);
    void loadFromFile(const QString &fileName) noexcept(false);

private:
    void basicOperation(const QString &queryString) noexcept(false);
    QSqlDatabase m_database;
};

#endif // DATABASEINTERACTOR_H
