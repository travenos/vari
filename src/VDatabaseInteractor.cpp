/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include <QTextStream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QVariant>
#include <QDir>
#include <QCoreApplication>
#include <QSqlRecord>

#include <deque>

#include "VDatabaseInteractor.h"

const QString VDatabaseInteractor::OPEN_ERROR_STRING("Невозможно открыть базу данных");
const QString VDatabaseInteractor::FILE_ERROR_STRING("Ошибка обработки файла");
const QString VDatabaseInteractor::NAME_ERROR_STRING("Некорректное название материала");
const QString VDatabaseInteractor::SQLFILE_ERROR_STRING("База данных не загружена, а файл \"create.sql\" не найден");

const QString VDatabaseInteractor::GET_NAME_QUERY("SELECT name FROM %1;");
const QString VDatabaseInteractor::GET_ALL_QUERY("SELECT * FROM %1;");
const QString VDatabaseInteractor::DELETE_BY_ID_QUERY("DELETE FROM %1 WHERE id=%2;");

const QString VDatabaseInteractor::COMMANDS_FILE("create.sql");

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

void VDatabaseInteractor::loadNames(bool sort)
{
    for (int i{0}; i < 2; ++i)
    {
        m_namesDeque.clear();
        bool result = true;
        bool hadError = false;
        if (!databaseInstance()->isOpen() && databaseInstance()->open())
        {
            {
                QSqlQuery query(GET_NAME_QUERY.arg(m_tableName), databaseInstance()->getDatabase());
                while (query.next())
                    m_namesDeque.push_back(query.value(0).toString());
                if (sort)
                    std::sort(m_namesDeque.begin(), m_namesDeque.end());
                hadError = query.lastError().isValid();
            }
            databaseInstance()->close();
            result = (m_namesDeque.size() > 0 && !hadError);
        }
        else
            result = false;
        if (result)
        {
            break;
        }
        else
        {
            createDatabase();
        }
    }
}

const std::deque<QString> & VDatabaseInteractor::getNames() const
{
    return m_namesDeque;
}

void VDatabaseInteractor::basicOperation(const QString &queryString) const 
{
    if (!databaseInstance()->isOpen() && databaseInstance()->open())
    {
        bool hadError;
        QString errorText;
        {
            QSqlQuery query(databaseInstance()->getDatabase());
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

void VDatabaseInteractor::saveToFile(const QString &fileName) const
{
    if (!databaseInstance()->isOpen() && databaseInstance()->open())
    {
        bool hadError;
        QString errorString;
        {
            QFile csvFile (fileName);
            if (!csvFile.open(QFile::WriteOnly | QFile::Text))
                throw DatabaseException(FILE_ERROR_STRING);
            QTextStream outStream(&csvFile);
            QSqlQuery query(GET_ALL_QUERY.arg(m_tableName), databaseInstance()->getDatabase());
            while (query.next())
            {
                const QSqlRecord record = query.record();
                for (int i{0}, recCount{record.count()} ; i < recCount ; ++i)
                {
                    if (i > 0)
                        outStream << ',';
                    outStream << record.value(i).toString();
                }
                outStream << '\n';
            }
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

bool VDatabaseInteractor::findSQLfile(const QString &name, QString *foundPath) const
{
    QString exeDir;
    exeDir = QFileInfo(QCoreApplication::applicationFilePath()).absoluteDir().absolutePath();
    QStringList directories = {
        QDir::cleanPath(exeDir + QStringLiteral("/sql")),
        QDir::cleanPath(exeDir + QStringLiteral("/../Resources")),
        QDir::cleanPath(QStringLiteral("/usr/share/vari/sql")),
        QDir::cleanPath(exeDir),
        QDir::cleanPath(exeDir + QStringLiteral("/../share/vari/sql")),
    };
    foreach(const QString &dirpath, directories)
    {
        QString filePath = QDir::cleanPath(dirpath + QDir::separator() + name);
        if (QFile::exists(filePath))
        {
            if (foundPath != nullptr)
                (*foundPath) = filePath;
            return true;
        }
    }
    return false;
}

void VDatabaseInteractor::createDatabase()
{
    QString sqlFile;
    if (!databaseInstance()->createDirIfNecessary())
        throw DatabaseException(OPEN_ERROR_STRING);
    if (!findSQLfile(COMMANDS_FILE, &sqlFile))
        throw DatabaseException(SQLFILE_ERROR_STRING);
    executeQueriesFromFile(sqlFile, databaseInstance()->getDatabase());
}

void VDatabaseInteractor::executeQueriesFromFile(const QString &filename, QSqlDatabase &db)
{
    QFile file(filename);
    if (!db.isOpen() && db.open())
    {
        {
            file.open(QIODevice::ReadOnly);
            QSqlQuery query(db);
            bool finishedFile = false;
            while (!finishedFile && !file.atEnd())
            {
                QByteArray readLine;
                QString cleanedLine;
                QString line;
                bool finishedLine = false;
                while(!finishedLine && !file.atEnd())
                {
                    readLine = file.readLine();
                    cleanedLine = readLine.trimmed();
                    // remove comments at end of line
                    QStringList strings = cleanedLine.split(QStringLiteral("--"));
                    cleanedLine=strings.at(0);
                    if(cleanedLine.startsWith(QStringLiteral("\\")))
                    {
                        break;
                    }
                    // remove lines with only comment
                    if(!cleanedLine.startsWith(QStringLiteral("--"))
                            && !cleanedLine.isEmpty())
                    {
                        line += cleanedLine;
                    }
                    if(cleanedLine.endsWith(';'))
                    {
                        break;
                    }
                    if(cleanedLine.startsWith(QStringLiteral("COMMIT")))
                    {
                        finishedLine = true;
                    }
                }

                if(!line.isEmpty())
                {
                    #ifdef DEBUG_MODE
                    qDebug() << line;
                    #endif
                    query.exec(line);
                }
            }
        }
        db.close();
    }
    else
        throw DatabaseException(OPEN_ERROR_STRING);
}
