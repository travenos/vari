/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VCLOTH_H
#define _VCLOTH_H

#include <memory>
#include <QColor>

struct VCloth
{
    typedef std::shared_ptr<VCloth> ptr;
    typedef std::shared_ptr<const VCloth> const_ptr;
    QString name;
    double cavityHeight;
    double permeability;
    double porosity;
    QColor baseColor;
};

#endif //_VCLOTH_H
