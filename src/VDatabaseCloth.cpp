/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include "VDatabaseCloth.h"

const QString VDatabaseCloth::TABLE_NAME("cloth");

const QString VDatabaseCloth::GET_INFO_QUERY("SELECT id, cavityheight, permeability, porosity FROM %1 WHERE name='%2';");
const QString VDatabaseCloth::UPDATE_INFO_QUERY("UPDATE %1 SET name='%2', cavityheight=%3, permeability=%4, porosity=%5 WHERE id=%6;");
const QString VDatabaseCloth::INSERT_INFO_QUERY("INSERT INTO %1 (name, cavityheight, permeability, porosity) VALUES ('%2', %3, %4, %5);");

VDatabaseCloth::VDatabaseCloth():
    VDatabaseInteractor(TABLE_NAME)
{
}

void VDatabaseCloth::materialInfo(const QString &name, int &id, float &cavityheight, float &permeability, float &porosity) const 
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
                cavityheight = query.value(1).toFloat();
                permeability = query.value(2).toFloat();
                porosity = query.value(3).toFloat();
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

void VDatabaseCloth::saveMaterial(const QString &name, int id, float cavityheight, float permeability, float porosity) 
{
    QString execString;
    if (id >= 0)
        execString = UPDATE_INFO_QUERY.arg(TABLE_NAME).arg(name).arg(cavityheight).arg(permeability).arg(porosity).arg(id);
    else
        execString = INSERT_INFO_QUERY.arg(TABLE_NAME).arg(name).arg(cavityheight).arg(permeability).arg(porosity);
    basicOperation(execString);
}
