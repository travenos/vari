/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include "VSimElement.h"

/**
 * VSimElement implementation
 */

const int VSimElement::RGB_MAX = 255;

/**
 * @param material
 */
VSimElement::VSimElement(uint id, const VCloth::const_ptr &p_material):
    m_pMaterial(p_material),
    m_id(id),
    m_visible(true)
{

}

VSimElement::~VSimElement()
{
    #ifdef DEBUG_MODE
    //    qInfo() << "VSimElement destroyed";
    #endif
}

/**
 * @param visible
 */
void VSimElement::setVisible(bool visible) 
{
    m_visible = visible;
}

/**
 * @return bool
 */
bool VSimElement::isVisible() const  
{
    return m_visible;
}


uint VSimElement::getId() const
{
    return m_id;
}

/**
 * @return QColor&
 */
const QColor &VSimElement::getColor() const  
{
    return m_pMaterial->baseColor;
}
