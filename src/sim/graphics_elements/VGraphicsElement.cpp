/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>

#include "VGraphicsElement.h"

/**
 * VGraphicsElement implementation
 */

const QColor VGraphicsElement::INJECTION_COLOR = QColor(0,0,255);
const QColor VGraphicsElement::VACUUM_COLOR = QColor(0,255,255);
const QColor VGraphicsElement::SELECTION_COLOR = QColor(255,0,255);

/**
 * @param simElement
 */
VGraphicsElement::VGraphicsElement(const VSimElement::const_ptr &simElement):
    m_pSimElement(simElement),
    m_pDrawStyle(new SoDrawStyle),
    m_pGraphicsMaterial(new SoMaterial)
{
    updateVisibility();
    addChild(m_pDrawStyle);
    updateColor();
    addChild(m_pGraphicsMaterial);
}

VGraphicsElement::~VGraphicsElement()
{

}

void VGraphicsElement::updateAll() 
{
    updateColor();
    updatePosition();
}

void VGraphicsElement::updateColor() 
{
    const QColor& color = m_pSimElement->getColor();
    m_pGraphicsMaterial->diffuseColor.setValue(color.redF(),
                                                  color.greenF(),
                                                  color.blueF());
}

void VGraphicsElement::updateVisibility()
{
    if (m_pSimElement->isVisible())
        m_pDrawStyle->style.setValue(SoDrawStyle::FILLED);
    else
        m_pDrawStyle->style.setValue(SoDrawStyle::INVISIBLE);
}

void VGraphicsElement::colorIfInjection()
{
    if (m_pSimElement->isNormal())
            updateColor();
    else if (m_pSimElement->isInjection())
    {
        const QColor& color = INJECTION_COLOR;
        m_pGraphicsMaterial->diffuseColor.setValue(color.redF(),
                                                   color.greenF(),
                                                   color.blueF());
    }
}

void VGraphicsElement::colorIfVacuum()
{
    if (m_pSimElement->isNormal())
        updateColor();
    else if (m_pSimElement->isVacuum())
    {
        const QColor& color = VACUUM_COLOR;
        m_pGraphicsMaterial->diffuseColor.setValue(color.redF(),
                                                   color.greenF(),
                                                   color.blueF());
    }
}

void VGraphicsElement::markAsSelected()
{
    const QColor& color = SELECTION_COLOR;
    m_pGraphicsMaterial->diffuseColor.setValue(color.redF(),
                                               color.greenF(),
                                               color.blueF());
}

uint VGraphicsElement::getSimId() const
{
    return m_pSimElement->getId();
}

bool VGraphicsElement::isVisible() const
{
    return (m_pDrawStyle->style.getValue() != SoDrawStyle::INVISIBLE);
}
