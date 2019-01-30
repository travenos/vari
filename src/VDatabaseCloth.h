/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VDATABASECLOTH_H
#define _VDATABASECLOTH_H

#include "VDatabaseInteractor.h"

class VDatabaseCloth : public VDatabaseInteractor
{
private:
    static const QString TABLE_NAME;

    static const QString GET_INFO_QUERY;
    static const QString UPDATE_INFO_QUERY;
    static const QString INSERT_INFO_QUERY;

    static const int PARAM_NUMBER;

public:
    VDatabaseCloth();
    void materialInfo(const QString &name, int &id, float &cavityheight, float &permability, float &porosity) const ;
    void saveMaterial(const QString &name, int id, float cavityheight, float permability, float porosity) ;

    void loadFromFile(const QString &fileName) override;
};

#endif // _VDATABASECLOTH_H
