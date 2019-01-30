/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include "VDatabaseResin.h"

const QString VDatabaseResin::TABLE_NAME("resin");

const QString VDatabaseResin::GET_INFO_QUERY("SELECT id, viscosity, tempcoef FROM %1 WHERE name='%2';");
const QString VDatabaseResin::UPDATE_INFO_QUERY("UPDATE %1 SET name='%2', viscosity=%3, tempcoef=%4 WHERE id=%5;");
const QString VDatabaseResin::INSERT_INFO_QUERY("INSERT INTO %1 (name, viscosity, tempcoef) VALUES ('%2', %3, %4);");

const int VDatabaseResin::PARAM_NUMBER = 3;

VDatabaseResin::VDatabaseResin():
    VDatabaseInteractor(TABLE_NAME)
{
}

void VDatabaseResin::materialInfo(const QString &name, int &id, float &viscosity, float &tempcoef) const 
{
    if (!databaseInstance()->isOpen() && databaseInstance()->open())
    {
        bool hadError;
        QString errorString;
        {
            QSqlQuery query(GET_INFO_QUERY.arg(TABLE_NAME).arg(name));
            while (query.next())
            {
                id = query.value(0).toInt();
                viscosity = query.value(1).toFloat();
                tempcoef = query.value(2).toFloat();
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

void VDatabaseResin::saveMaterial(const QString &name, int id, float viscosity, float tempcoef) 
{
    if (!name.contains(','))
    {
        QString execString;
        if (id >= 0)
            execString = UPDATE_INFO_QUERY.arg(TABLE_NAME).arg(name).arg(viscosity).arg(tempcoef).arg(id);
        else
            execString = INSERT_INFO_QUERY.arg(TABLE_NAME).arg(name).arg(viscosity).arg(tempcoef);
        basicOperation(execString);
        }
    else
        throw DatabaseException(OPEN_ERROR_STRING);
}

void VDatabaseResin::loadFromFile(const QString &fileName)
{
    QFile file(fileName);
    bool openResult = file.open(QFile::ReadOnly | QFile::Text);
    if ( openResult )
    {
        QTextStream in(&file);
        while (!in.atEnd())
        {
            QString name;
            float viscosity, tempcoef;
            QString line = in.readLine();
            QStringList paramList = line.split(",");
            if (paramList.length() != PARAM_NUMBER + 1)
            {
                file.close();
                throw DatabaseException(FILE_ERROR_STRING);
            }
            for (int i = 0; i < paramList.length(); ++i)
            {
                bool ok = true;
                switch(i)
                {
                case 1:
                    name = paramList[1];
                    break;
                case 2:
                    viscosity = paramList[2].toFloat(&ok);
                    break;
                case 3:
                    tempcoef = paramList[3].toFloat(&ok);
                    break;
                }
                if (!ok)
                    throw DatabaseException(FILE_ERROR_STRING);
            }
            try
            {
                saveMaterial(name, -1, viscosity, tempcoef);
            }
            catch (DatabaseException &)
            {}
        }
        file.close();
    }
    else
    {
        throw DatabaseException(FILE_ERROR_STRING);
    }
}
