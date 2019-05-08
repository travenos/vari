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
    double cavityHeight{0.001};
    double permeability{2e-9};
    double porosity{0.5};
    QColor baseColor{QColor(255, 172, 172)};
    QString name{QStringLiteral("")};

    float getHeight() const
    {
        return static_cast<float>(cavityHeight);
    }
};

struct VResin
{
    double viscTempcoef{9000};
    double lifetimeTempcoef{6000};
    double defaultViscosity{0.1};
    double defaultLifetime{10800};
    QString name{QStringLiteral("")};
};

#endif //_VMATERIALS_H
