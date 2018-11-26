/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VSimElement.h"

/**
 * VSimElement implementation
 */


/**
 * @param material
 */
VSimElement::VSimElement(VCloth::const_ptr p_material) {

}

/**
 * @param visible
 */
void VSimElement::setVisible(bool visible) noexcept {

}

/**
 * @return bool
 */
bool VSimElement::isVisible() const  noexcept{
    return m_visible;
}

/**
 * @return QColor&
 */
const QColor &VSimElement::getColor() const  noexcept{
    return m_pMaterial->baseColor;
}
