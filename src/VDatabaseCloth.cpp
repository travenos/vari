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

const QString VDatabaseCloth::TABLE_NAME("cloth_a");

const QString VDatabaseCloth::GET_INFO_QUERY("SELECT id, cavityheight, xpermeability, ypermeability, porosity FROM %1 WHERE name='%2';");
const QString VDatabaseCloth::UPDATE_INFO_QUERY("UPDATE %1 SET name='%2', cavityheight=%3, xpermeability=%4, xpermeability=%4, ypermeability=%5, porosity=%6 WHERE id=%7;");
const QString VDatabaseCloth::INSERT_INFO_QUERY("INSERT INTO %1 (name, cavityheight, xpermeability, ypermeability, porosity) VALUES ('%2', %3, %4, %5, %6);");

const int VDatabaseCloth::PARAM_NUMBER = 5;

VDatabaseCloth::VDatabaseCloth():
    VDatabaseInteractor(TABLE_NAME)
{
}

void VDatabaseCloth::materialInfo(const QString &name, int &id, float &cavityheight, float &xpermeability, float &ypermeability, float &porosity) const
{
    if (!databaseInstance()->isOpen() && databaseInstance()->open())
    {
        bool hadError;
        QString errorString;
        {
            QSqlQuery query(GET_INFO_QUERY.arg(TABLE_NAME).arg(name), databaseInstance()->getDatabase());
            while (query.next())
            {
                id = query.value(0).toInt();
                cavityheight = query.value(1).toFloat();
                xpermeability = query.value(2).toFloat();
                ypermeability = query.value(3).toFloat();
                porosity = query.value(4).toFloat();
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

void VDatabaseCloth::saveMaterial(const QString &name, int id, float cavityheight, float xpermeability, float ypermeability, float porosity)
{
    if (!name.contains(','))
    {
        QString execString;
        if (id >= 0)
            execString = UPDATE_INFO_QUERY.arg(TABLE_NAME).arg(name).arg(cavityheight).arg(xpermeability).arg(porosity).arg(id);
        else
            execString = INSERT_INFO_QUERY.arg(TABLE_NAME).arg(name).arg(cavityheight).arg(xpermeability).arg(porosity);
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
            float cavityheight=0, xpermeability=0, ypermeability=0, porosity=0;
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
                    xpermeability = paramList[3].toFloat(&ok);
                    break;
                case 4:
                    ypermeability = paramList[4].toFloat(&ok);
                    break;
                case 5:
                    porosity = paramList[5].toFloat(&ok);
                    break;
                }
                if (!ok)
                    throw DatabaseException(FILE_ERROR_STRING);
            }
            try
            {
                saveMaterial(name, -1, cavityheight, xpermeability, ypermeability, porosity);
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
