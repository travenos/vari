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

    static const int PARAM_NUMBER;

public:
    VDatabaseResin();
    void materialInfo(const QString &name, int &id, float &viscosity, float &viscTempcoef,
                      float &lifetime, float &lifetimeTempcoef) const ;
    void saveMaterial(const QString &name, int id, float viscosity, float viscTempcoef,
                      float lifetime, float lifetimeTempcoef) ;

    void loadFromFile(const QString &fileName) override;
};

#endif // _VDATABASERESIN_H
