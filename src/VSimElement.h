/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMELEMENT_H
#define _VSIMELEMENT_H

class VSimElement {
public: 
    
/**
 * @param material
 */
void VSimElement(VMaterialPtr material);
    
/**
 * @param visible
 */
void setVisible(bool visible);
    
bool isVisible();
    
QColor& getColor();
    
virtual void reset() = 0;
protected: 
    const VMaterialPtr m_material;
private: 
    bool m_visible;
};

#endif //_VSIMELEMENT_H