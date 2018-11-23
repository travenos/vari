#include "v_database_interactor.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QVariant>
#include <QDir>
#include <deque>

static const QString OPEN_ERROR_STRING("Unable to open database");
static const QString FILE_ERROR_STRING("Error processing file");

static const QString HOSTNAME("127.0.0.1");
static const QString DATABASENAME("vari");
static const QString USERNAME("vari");
static const QString PASSWORD("vari-password");

static const QString GET_NAME_QUERY("SELECT name FROM materials;");
static const QString GET_INFO_QUERY("SELECT id, cavityheight, permability, porosity FROM materials WHERE name='%1';");
static const QString UPDATE_INFO_QUERY("UPDATE materials SET name='%1', cavityheight=%2, permability=%3, porosity=%4 WHERE id=%5;");
static const QString INSERT_INFO_QUERY("INSERT INTO materials (name, cavityheight, permability, porosity) VALUES ('%1', %2, %3, %4);");
static const QString DELETE_BY_ID_QUERY("DELETE FROM materials WHERE id=%1;");
static const QString COPY_FROM_FILE_QUERY("COPY materials FROM '%1' DELIMITER ',' CSV;");
static const QString COPY_TO_FILE_QUERY("COPY materials TO '%1' DELIMITER ',' CSV;");

DatabaseInteractor::DatabaseInteractor():
    m_database(QSqlDatabase::addDatabase("QPSQL"))
{
    m_database.setHostName(HOSTNAME);
    m_database.setDatabaseName(DATABASENAME);
    m_database.setUserName(USERNAME);
    m_database.setPassword(PASSWORD);
}

void DatabaseInteractor::getNames(std::deque<QString> &outputDeque, bool sort) noexcept(false)
{
    if (!m_database.isOpen() && m_database.open())
    {
        bool hadError;
        QString errorString;
        {
            QSqlQuery query(GET_NAME_QUERY);
            while (query.next())
                outputDeque.push_back(query.value(0).toString());
            if (sort)
                std::sort(outputDeque.begin(), outputDeque.end());
            hadError = query.lastError().isValid();
            if (hadError)
                errorString = query.lastError().text();
        }
        m_database.close();
        if (hadError)
            throw DatabaseException(errorString);
    }
    else
        throw DatabaseException(OPEN_ERROR_STRING);
}


void DatabaseInteractor::materialInfo(const QString &name, int &id, float &cavityheight, float &permability, float &porosity) noexcept(false)
{
    if (!m_database.isOpen() && m_database.open())
    {
        bool hadError;
        QString errorString;
        {
            QSqlQuery query(GET_INFO_QUERY.arg(name));
            while (query.next())
            {
                id = query.value(0).toInt();
                cavityheight = query.value(1).toFloat();
                permability = query.value(2).toFloat();
                porosity = query.value(3).toFloat();
            }
            hadError = query.lastError().isValid();
            if (hadError)
                errorString = query.lastError().text();
        }
        m_database.close();
        if (hadError)
            throw DatabaseException(errorString);
    }
    else
        throw DatabaseException(OPEN_ERROR_STRING);
}

void DatabaseInteractor::basicOperation(const QString &queryString) noexcept(false)
{
    if (!m_database.isOpen() && m_database.open())
    {
        bool hadError;
        QString errorText;
        {
            QSqlQuery query;
            query.exec(queryString);
            hadError = query.lastError().isValid();
            errorText = query.lastError().text();
        }
        m_database.close();
        if (hadError)
            throw DatabaseException(errorText);
    }
    else
        throw DatabaseException(OPEN_ERROR_STRING);
}

void DatabaseInteractor::saveMaterial(const QString &name, int id, float cavityheight, float permability, float porosity) noexcept(false)
{
    QString execString;
    if (id >= 0)
        execString = UPDATE_INFO_QUERY.arg(name).arg(cavityheight).arg(permability).arg(porosity).arg(id);
    else
        execString = INSERT_INFO_QUERY.arg(name).arg(cavityheight).arg(permability).arg(porosity);
    basicOperation(execString);
}

void DatabaseInteractor::removeMaterial(int id) noexcept(false)
{
    basicOperation(DELETE_BY_ID_QUERY.arg(id));
}

void DatabaseInteractor::loadFromFile(const QString &fileName) noexcept(false)
{
    basicOperation(COPY_FROM_FILE_QUERY.arg(fileName));
}

void DatabaseInteractor::saveToFile(const QString &fileName) noexcept(false)
{
    QString baseName = QFileInfo(fileName).fileName();
    QString tempFileName = QDir::cleanPath(QDir::tempPath() + QDir::separator() + baseName);
    basicOperation(COPY_TO_FILE_QUERY.arg(tempFileName));
    QFile tempFile(tempFileName);
    QFile(fileName).remove();
    bool copyOk = tempFile.copy(fileName);
    tempFile.remove();
    if (!copyOk)
        throw DatabaseException(FILE_ERROR_STRING);
}
