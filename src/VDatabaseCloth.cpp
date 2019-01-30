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
#include "VDatabaseCloth.h"

const QString VDatabaseCloth::TABLE_NAME("cloth");

const QString VDatabaseCloth::GET_INFO_QUERY("SELECT id, cavityheight, permeability, porosity FROM %1 WHERE name='%2';");
const QString VDatabaseCloth::UPDATE_INFO_QUERY("UPDATE %1 SET name='%2', cavityheight=%3, permeability=%4, porosity=%5 WHERE id=%6;");
const QString VDatabaseCloth::INSERT_INFO_QUERY("INSERT INTO %1 (name, cavityheight, permeability, porosity) VALUES ('%2', %3, %4, %5);");

const int VDatabaseCloth::PARAM_NUMBER = 4;

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
    if (!name.contains(','))
    {
        QString execString;
        if (id >= 0)
            execString = UPDATE_INFO_QUERY.arg(TABLE_NAME).arg(name).arg(cavityheight).arg(permeability).arg(porosity).arg(id);
        else
            execString = INSERT_INFO_QUERY.arg(TABLE_NAME).arg(name).arg(cavityheight).arg(permeability).arg(porosity);
        basicOperation(execString);
    }
    else
        throw DatabaseException(NAME_ERROR_STRING);
}

void VDatabaseCloth::loadFromFile(const QString &fileName)
{
    QFile file(fileName);
    bool openResult = file.open(QFile::ReadOnly | QFile::Text);
    if ( openResult )
    {
        QTextStream in(&file);
        while (!in.atEnd())
        {
            QString name;
            float cavityheight=0, permeability=0, porosity=0;
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
                    cavityheight = paramList[2].toFloat(&ok);
                    break;
                case 3:
                    permeability = paramList[3].toFloat(&ok);
                    break;
                case 4:
                    porosity = paramList[4].toFloat(&ok);
                    break;
                }
                if (!ok)
                    throw DatabaseException(FILE_ERROR_STRING);
            }
            try
            {
                saveMaterial(name, -1, cavityheight, permeability, porosity);
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
