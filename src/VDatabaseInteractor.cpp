/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QVariant>
#include <QDir>
#include <deque>
#include "VDatabaseInteractor.h"

const QString VDatabaseInteractor::OPEN_ERROR_STRING("Невозможно открыть базу данных");
const QString VDatabaseInteractor::FILE_ERROR_STRING("Ошибка обработки файла");
const QString VDatabaseInteractor::NAME_ERROR_STRING("Некорректное название материала");

const QString VDatabaseInteractor::GET_NAME_QUERY("SELECT name FROM %1;");
const QString VDatabaseInteractor::DELETE_BY_ID_QUERY("DELETE FROM %1 WHERE id=%2;");
const QString VDatabaseInteractor::COPY_FROM_FILE_QUERY("COPY %1 FROM '%2' DELIMITER ',' CSV;");
const QString VDatabaseInteractor::COPY_TO_FILE_QUERY("COPY %1 TO '%2' DELIMITER ',' CSV;");

VDatabaseInteractor::VDatabaseInteractor(const QString &tableName):
    m_tableName(tableName)
{
}

VDatabaseInteractor::~VDatabaseInteractor()
{
    VSqlDatabase::deleteInstance();
}

VSqlDatabase* VDatabaseInteractor::databaseInstance() const 
{
    return VSqlDatabase::getInstance();
}

void VDatabaseInteractor::getNames(std::deque<QString> &outputDeque, bool sort) const 
{
    outputDeque.clear();
    if (!databaseInstance()->isOpen() && databaseInstance()->open())
    {
        bool hadError;
        QString errorString;
        {
            QSqlQuery query(GET_NAME_QUERY.arg(m_tableName));
            while (query.next())
                outputDeque.push_back(query.value(0).toString());
            if (sort)
                std::sort(outputDeque.begin(), outputDeque.end());
            hadError = query.lastError().isValid();
            if (hadError)
                errorString = query.lastError().text();
        }
        databaseInstance()->close();
        if (hadError)
            throw DatabaseException(errorString);
    }
    else
        throw DatabaseException(OPEN_ERROR_STRING);
}

void VDatabaseInteractor::basicOperation(const QString &queryString) const 
{
    if (!databaseInstance()->isOpen() && databaseInstance()->open())
    {
        bool hadError;
        QString errorText;
        {
            QSqlQuery query;
            query.exec(queryString);
            hadError = query.lastError().isValid();
            errorText = query.lastError().text();
        }
        databaseInstance()->close();
        if (hadError)
            throw DatabaseException(errorText);
    }
    else
        throw DatabaseException(OPEN_ERROR_STRING);
}

void VDatabaseInteractor::removeMaterial(int id) 
{
    basicOperation(DELETE_BY_ID_QUERY.arg(m_tableName).arg(id));
}

void VDatabaseInteractor::loadFromFile(const QString &fileName) 
{
    basicOperation(COPY_FROM_FILE_QUERY.arg(m_tableName).arg(fileName));
}

void VDatabaseInteractor::saveToFile(const QString &fileName) const 
{
    QString baseName = QFileInfo(fileName).fileName();
    QString tempFileName = QDir::cleanPath(QDir::tempPath() + QDir::separator() + baseName);
    basicOperation(COPY_TO_FILE_QUERY.arg(m_tableName).arg(tempFileName));
    QFile tempFile(tempFileName);
    QFile(fileName).remove();
    bool copyOk = tempFile.copy(fileName);
    tempFile.remove();
    if (!copyOk)
        throw DatabaseException(FILE_ERROR_STRING);
}
