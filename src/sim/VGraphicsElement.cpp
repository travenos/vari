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
    m_pGraphicsMaterial->diffuseColor.setHSVValue(color.hueF(),
                                                  color.saturationF(),
                                                  color.valueF());
}

void VGraphicsElement::updateVisibility()
{
    if (m_pSimElement->isVisible())
        m_pDrawStyle->style.setValue(SoDrawStyle::FILLED);
    else
        m_pDrawStyle->style.setValue(SoDrawStyle::INVISIBLE);
}
