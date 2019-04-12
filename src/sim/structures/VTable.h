/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VTABLE_H
#define _VTABLE_H

#include "VInjectionVacuum.h"

class VTable
{
public:
    VTable() = default;
    VTable(const VTable& other) = default;
    const QVector2D& getSize() const;
    const QVector2D& getInjectionCoords() const;
    const QVector2D& getVacuumCoords() const;
    void setSize(const QVector2D& size);
    void setSize(float width, float height);
    void setInjectionCoords(const QVector2D &coords);
    void setInjectionCoords(float x, float y);
    void setVacuumCoords(const QVector2D& coords);
    void setVacuumCoords(float x, float y);

    float getInjectionDiameter() const ;
    void setInjectionDiameter(float diameter) ;
    float getVacuumDiameter() const ;
    void setVacuumDiameter(float diameter) ;

    const VInjectionVacuum& getInjectionVacuum() const;
    void setInjectionVacuum(const VInjectionVacuum &injectionVacuum);
private:
    QVector2D m_size{2, 1};
    VInjectionVacuum m_injectionVacuum;
};

#endif //_VTABLE_H
