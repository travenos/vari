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
VGraphicsElement(VSimElem::ptr simElement);
void updateAll() noexcept;
void updateColor() noexcept;
virtual void updatePosition() = 0;

protected: 
    SoMaterial* m_pGraphicsMaterial;
private: 
    VSimElem::ptr m_pSimElement;
};

#endif //_VGRAPHICSELEMENT_H
