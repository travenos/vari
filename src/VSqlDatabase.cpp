/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <QString>
#include "VSqlDatabase.h"

const QString VSqlDatabase::HOSTNAME("127.0.0.1");
const QString VSqlDatabase::DATABASENAME("vari");
const QString VSqlDatabase::USERNAME("vari");
const QString VSqlDatabase::PASSWORD("vari-password");
VSqlDatabase* VSqlDatabase::s_object = nullptr;

VSqlDatabase * VSqlDatabase::getInstance()
{
    if (s_object != nullptr)
    {
        return s_object;
    }
    else
    {
        s_object = new VSqlDatabase();
        return s_object;
    }
}

void VSqlDatabase::deleteInstance()
{
    if (s_object != nullptr)
    {
        delete s_object;
        s_object = nullptr;
    }
}

bool VSqlDatabase::hasInstance()
{
    return (s_object != nullptr);
}

VSqlDatabase::VSqlDatabase()
{
    m_database = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"));
    m_database.setHostName(HOSTNAME);
    m_database.setDatabaseName(DATABASENAME);
    m_database.setUserName(USERNAME);
    m_database.setPassword(PASSWORD);
}

bool VSqlDatabase::open()
{
    return m_database.open();
}
bool VSqlDatabase::isOpen() const
{
    return m_database.isOpen();
}
void VSqlDatabase::close()
{
    m_database.close();
}

void VSqlDatabase::setDatabaseName(const QString &name)
{
    m_database.setDatabaseName(name);
}

void VSqlDatabase::setHostName(const QString &host)
{
    m_database.setHostName(host);
}

void VSqlDatabase::setPassword(const QString &password)
{
    m_database.setPassword(password);
}

void VSqlDatabase::setUserName(const QString &name)
{
    m_database.setPassword(name);
}

