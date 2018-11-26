/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMELEMENT_H
#define _VSIMELEMENT_H

#include <QColor>
#include "VMaterial.h"

class VSimElement {
public: 
    
/**
 * @param material
 */
VSimElement(VCloth::const_ptr p_material);
    typedef std::shared_ptr<VSimElement> ptr;
    typedef std::shared_ptr<const VSimElement> const_ptr;
    typedef std::weak_ptr<VSimElement> weak_ptr;
    typedef std::weak_ptr<const VSimElement> const_weak_ptr;

/**
 * @param visible
 */
void setVisible(bool visible) noexcept;
bool isVisible() const noexcept;
const QColor& getColor() const noexcept;
virtual void reset() = 0;
protected:
    const VCloth::const_ptr m_pMaterial;
private: 
    bool m_visible;
};

#endif //_VSIMELEMENT_H
