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
virtual void updateAll() noexcept;
virtual void updateColor() noexcept;
virtual void updatePosition() = 0;

private: 
    VSimElement::const_ptr m_pSimElement;
protected:
    SoMaterial* m_pGraphicsMaterial;
};

#endif //_VGRAPHICSELEMENT_H
