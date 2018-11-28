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
VGraphicsElement::VGraphicsElement(VSimElement::const_ptr simElement):
    m_pSimElement(simElement),
    m_pGraphicsMaterial(new SoMaterial)
{
    const QColor& color = m_pSimElement->getColor();
    m_pGraphicsMaterial->diffuseColor.setHSVValue(color.hueF(),
                                                  color.saturationF(),
                                                  color.valueF());
    addChild(m_pGraphicsMaterial);
}

void VGraphicsElement::updateAll() noexcept
{
    updateColor();
    updatePosition();
}

void VGraphicsElement::updateColor() noexcept
{
    const QColor& color = m_pSimElement->getColor();
    m_pGraphicsMaterial->diffuseColor.setHSVValue(color.hueF(),
                                                  color.saturationF(),
                                                  color.valueF());
}
