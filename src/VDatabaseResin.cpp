/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include "VDatabaseResin.h"

const QString VDatabaseResin::TABLE_NAME("resin");

const QString VDatabaseResin::GET_INFO_QUERY("SELECT id, viscosity, tempcoef FROM %1 WHERE name='%2';");
const QString VDatabaseResin::UPDATE_INFO_QUERY("UPDATE %1 SET name='%2', viscosity=%3, tempcoef=%4 WHERE id=%5;");
const QString VDatabaseResin::INSERT_INFO_QUERY("INSERT INTO %1 (name, viscosity, tempcoef) VALUES ('%2', %3, %4);");

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
    QString execString;
    if (id >= 0)
        execString = UPDATE_INFO_QUERY.arg(TABLE_NAME).arg(name).arg(viscosity).arg(tempcoef).arg(id);
    else
        execString = INSERT_INFO_QUERY.arg(TABLE_NAME).arg(name).arg(viscosity).arg(tempcoef);
    basicOperation(execString);
}

