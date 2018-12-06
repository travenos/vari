/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include "VGraphicsElement.h"

/**
 * VGraphicsElement implementation
 */


/**
 * @param simElement
 */
VGraphicsElement::VGraphicsElement(const VSimElement::const_ptr &simElement):
    m_pSimElement(simElement),
    m_pGraphicsMaterial(new SoMaterial)
{
    const QColor& color = m_pSimElement->getColor();
    m_pGraphicsMaterial->diffuseColor.setHSVValue(color.hueF(),
                                                  color.saturationF(),
                                                  color.valueF());
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
