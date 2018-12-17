/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMELEMENT_H
#define _VSIMELEMENT_H

#include <QColor>
#include <vector>
#include "VCloth.h"
#include "VSimulationParametres.h"

class VSimElement
{
public: 
    static const int RGB_MAX;
/**
 * @param material
 */
VSimElement(const VCloth::const_ptr &p_material, const VSimulationParametres::const_ptr &p_param);
    typedef std::shared_ptr<VSimElement> ptr;
    typedef std::shared_ptr<const VSimElement> const_ptr;
    typedef std::shared_ptr<std::vector<VSimElement::ptr> > vector_ptr;
    typedef std::shared_ptr<const std::vector<VSimElement::ptr> > const_vector_ptr;
virtual ~VSimElement();
VSimElement(const VSimElement& ) = delete;
VSimElement& operator= (const VSimElement& ) = delete;
/**
 * @param visible
 */
void setVisible(bool visible) ;
bool isVisible() const ;
virtual const QColor& getColor() const ;
virtual void reset()  = 0;
virtual bool isInjection() const = 0 ;
virtual bool isVacuum() const = 0 ;
virtual bool isNormal() const = 0 ;
virtual double getPressure() const = 0;
virtual double getFilledPart() const = 0;

protected:
    const VCloth::const_ptr m_pMaterial;
    const VSimulationParametres::const_ptr m_pParam;
private: 
    bool m_visible;
};

#endif //_VSIMELEMENT_H
