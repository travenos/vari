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

public:
    VDatabaseCloth();
    //TODO struct "VMaterial"
    void materialInfo(const QString &name, int &id, float &cavityheight, float &permability, float &porosity) const noexcept(false);
    //TODO struct "VMaterial"
    void saveMaterial(const QString &name, int id, float cavityheight, float permability, float porosity) noexcept(false);
};

#endif // _VDATABASECLOTH_H
