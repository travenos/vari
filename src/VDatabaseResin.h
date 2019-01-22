/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VDATABASERESIN_H
#define _VDATABASERESIN_H

#include "VDatabaseInteractor.h"

class VDatabaseResin : public VDatabaseInteractor
{
private:
    static const QString TABLE_NAME;

    static const QString GET_INFO_QUERY;
    static const QString UPDATE_INFO_QUERY;
    static const QString INSERT_INFO_QUERY;

public:
    VDatabaseResin();
    void materialInfo(const QString &name, int &id, float &viscosity, float &tempcoef) const ;
    void saveMaterial(const QString &name, int id, float viscosity, float tempcoef) ;
};

#endif // _VDATABASERESIN_H
