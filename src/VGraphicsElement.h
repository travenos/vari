/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSELEMENT_H
#define _VGRAPHICSELEMENT_H

class VGraphicsElement {
public: 
    
/**
 * @param simElement
 */
void VGraphicsElement(VSimElemPtr simElement);
    
void updateAll();
    
void updateColor();
    
virtual void updatePosition() = 0;
protected: 
    SoMaterial* m_pGraphicsMaterial;
private: 
    VSimElemPtr m_pSimElement;
};

#endif //_VGRAPHICSELEMENT_H