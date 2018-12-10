/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSELEMENT_H
#define _VGRAPHICSELEMENT_H

#include <Inventor/nodes/SoSeparator.h>
#include "VSimElement.h"

class SoMaterial;
class SoDrawStyle;

class VGraphicsElement : public SoSeparator {
public: 
    
/**
 * @param simElement
 */
VGraphicsElement(const VSimElement::const_ptr &simElement);
VGraphicsElement(const VGraphicsElement& ) = delete;
VGraphicsElement& operator= (const VGraphicsElement& ) = delete;
virtual ~VGraphicsElement();
virtual void updateAll() ;
virtual void updateColor() ;
virtual void updateVisibility();
virtual void updatePosition() = 0;

private: 
    VSimElement::const_ptr m_pSimElement;
protected:
    SoDrawStyle* m_pDrawStyle;
    SoMaterial* m_pGraphicsMaterial;
};

#endif //_VGRAPHICSELEMENT_H
