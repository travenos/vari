/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VMATERIALS_H
#define _VMATERIALS_H

#include <memory>
#include <QColor>

struct VCloth
{
    typedef std::shared_ptr<VCloth> ptr;
    typedef std::shared_ptr<const VCloth> const_ptr;
    double cavityHeight = 0.001;
    double permeability = 2e-9;
    double porosity = 0.5;
    QColor baseColor = QColor(255, 172, 172);
    QString name = "";
};

struct VResin
{
    double tempcoef = 1;
    double defaultViscosity = 0.1;
    QString name = "";
};

#endif //_VMATERIALS_H
