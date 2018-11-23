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
VSimElement::VSimElement(VMaterial::const_ptr p_material) {

}

/**
 * @param visible
 */
void VSimElement::setVisible(bool visible) {

}

/**
 * @return bool
 */
bool VSimElement::isVisible() const{
    return m_visible;
}

/**
 * @return QColor&
 */
const QColor &VSimElement::getColor() const{
    return NULL;
}
