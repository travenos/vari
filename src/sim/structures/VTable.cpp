/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include "VTable.h"

/**
 * VTable implementation
 */

const QVector2D& VTable::getSize() const
{
    return m_size;
}

const QVector2D& VTable::getInjectionCoords() const
{
    return m_injectionVacuum.injectionCoords;
}

const QVector2D& VTable::getVacuumCoords() const
{
    return m_injectionVacuum.vacuumCoords;
}

void VTable::setSize(const QVector2D& tableSize)
{
    m_size = tableSize;
}

void VTable::setSize(float width, float height)
{
    m_size.setX(width);
    m_size.setY(height);
}

void VTable::setInjectionCoords(const QVector2D& coords)
{
    m_injectionVacuum.injectionCoords = coords;
}

void VTable::setInjectionCoords(float x, float y)
{
    m_injectionVacuum.injectionCoords.setX(x);
    m_injectionVacuum.injectionCoords.setY(y);
}

void VTable::setVacuumCoords(const QVector2D& coords)
{
    m_injectionVacuum.vacuumCoords = coords;
}

void VTable::setVacuumCoords(float x, float y)
{
    m_injectionVacuum.vacuumCoords.setX(x);
    m_injectionVacuum.vacuumCoords.setY(y);
}

float VTable::getInjectionDiameter() const
{
    return m_injectionVacuum.injectionDiameter;
}

void VTable::setInjectionDiameter(float diameter)
{
    m_injectionVacuum.injectionDiameter = std::max(diameter, 0.0f);
}

float VTable::getVacuumDiameter() const
{
    return m_injectionVacuum.vacuumDiameter;
}
void VTable::setVacuumDiameter(float diameter)
{
    m_injectionVacuum.vacuumDiameter = std::max(diameter, 0.0f);
}

const VInjectionVacuum& VTable::getInjectionVacuum() const
{
    return m_injectionVacuum;
}

void VTable::setInjectionVacuum(const VInjectionVacuum &injectionVacuum)
{
    setInjectionDiameter(injectionVacuum.injectionDiameter);
    setVacuumDiameter(injectionVacuum.vacuumDiameter);
    setInjectionCoords(injectionVacuum.injectionCoords);
    setVacuumCoords(injectionVacuum.vacuumCoords);
}
