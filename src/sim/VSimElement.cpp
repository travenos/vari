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
VSimElement::VSimElement(const VCloth::const_ptr &p_material,
                         const VSimulationParametres::const_ptr &p_param):
    m_pMaterial(p_material),
    m_pParam(p_param),
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

/**
 * @return QColor&
 */
const QColor &VSimElement::getColor() const  
{
    return m_pMaterial->baseColor;
}
