/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSELEMENT_H
#define _VGRAPHICSELEMENT_H

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include "VSimElement.h"

class VGraphicsElement : public SoSeparator {
public: 
    
/**
 * @param simElement
 */
VGraphicsElement(VSimElement::const_ptr simElement);
void updateAll() noexcept;
void updateColor() noexcept;
virtual void updatePosition() = 0;

protected: 
    SoMaterial* m_pGraphicsMaterial;
private: 
    VSimElement::ptr m_pSimElement;
};

#endif //_VGRAPHICSELEMENT_H
