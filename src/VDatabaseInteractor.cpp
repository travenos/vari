/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QVariant>
#include <QDir>
#include <QCoreApplication>
#include <QTextCodec>

#include <deque>

#include "VDatabaseInteractor.h"

const QString VDatabaseInteractor::OPEN_ERROR_STRING("Невозможно открыть базу данных");
const QString VDatabaseInteractor::FILE_ERROR_STRING("Ошибка обработки файла");
const QString VDatabaseInteractor::NAME_ERROR_STRING("Некорректное название материала");
const QString VDatabaseInteractor::SQLFILE_ERROR_STRING("База данных не загружена, а файл \"create.sql\" не найден");

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

void VDatabaseInteractor::loadNames(bool sort)
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
    if (!result)
    {
        bool sqlFileFound = findSQLfile();
        if (sqlFileFound)
        {            
            emit needsToLoadDB();
        }
        else
        {
            throw DatabaseException(SQLFILE_ERROR_STRING);
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

bool VDatabaseInteractor::findSQLfile(QString *sqlFile) const
{
    const QString sqlCreateFilename(QStringLiteral("create.sql"));
    QString exeDir;
    exeDir = QFileInfo(QCoreApplication::applicationFilePath()).absoluteDir().absolutePath();
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
            if (sqlFile != nullptr)
                (*sqlFile) = filePath;
            return true;
        }
    }
    return false;
}

void VDatabaseInteractor::createDatabase(const QString &postgresPassword)
{
    const QString CONNECTION_NAME(QStringLiteral("psql_connect"));
    QString sqlFile;
    if (!findSQLfile(&sqlFile))
        throw DatabaseException(OPEN_ERROR_STRING);
    try
    {
        executeQueriesFromFile(sqlFile, databaseInstance()->getDatabase());
        return;
    }
    catch(DatabaseException&)
    {    }
    QSqlDatabase *db = new QSqlDatabase;
    (*db) = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), CONNECTION_NAME);
    db->setHostName(QStringLiteral("127.0.0.1"));
    //db->setDatabaseName(QStringLiteral("postgres")); //TODO
    db->setUserName(QStringLiteral("postgres"));
    db->setPassword(postgresPassword);
    try
    {
        executeQueriesFromFile(sqlFile, *db);
        executeQueriesFromFile(sqlFile, databaseInstance()->getDatabase());
    }
    catch(DatabaseException &e)
    {
        delete db;
        QSqlDatabase::removeDatabase(CONNECTION_NAME);
        throw e;
    }
    delete db;
    QSqlDatabase::removeDatabase(CONNECTION_NAME);
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
                    cleanedLine=readLine.trimmed();
                    // remove comments at end of line
                    QStringList strings = cleanedLine.split(QStringLiteral("--"));
                    cleanedLine=strings.at(0);

                    if(cleanedLine.startsWith(QStringLiteral("\\c")))
                    {
                        QStringList str = cleanedLine.split(' ');
                        if (str.size() > 1)
                        {
                            if (db.databaseName() != str.at(1))
                            {
                                finishedFile = true;
                                break;
                            }
                        }
                    }
                    if(cleanedLine.startsWith(QStringLiteral("\\")))
                    {
                        break;
                    }
                    // remove lines with only comment, and DROP lines
                    if(!cleanedLine.startsWith(QStringLiteral("--"))
                            && !cleanedLine.startsWith(QStringLiteral("DROP"))
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
                    #if defined(Q_OS_WIN) || defined (WIN32) || defined(__WIN32__)
                    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
                    line = codec->toUnicode(line.toLocal8Bit());
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
