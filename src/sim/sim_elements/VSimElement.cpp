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

void VSimElement::setVisible(bool visible) 
{
    m_visible = visible;
}

bool VSimElement::isVisible() const  
{
    return m_visible;
}


uint VSimElement::getId() const
{
    return m_id;
}

const QColor &VSimElement::getColor() const  
{
    return m_pMaterial->getBaseColor();
}
