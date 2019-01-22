/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMELEMENT_H
#define _VSIMELEMENT_H

#include <QColor>
#include <vector>

#include "../structures/VSimulationParametres.h"

class VSimElement
{
public: 
    static const int RGB_MAX;

    typedef std::shared_ptr<VSimElement> ptr;
    typedef std::shared_ptr<const VSimElement> const_ptr;
    typedef std::vector<ptr> vector_t;
    typedef std::shared_ptr<vector_t> vector_ptr;
    typedef std::shared_ptr<const vector_t> const_vector_ptr;

    VSimElement(uint id, const VCloth::const_ptr &p_material);


    virtual ~VSimElement();
    VSimElement(const VSimElement& ) = delete;
    VSimElement& operator= (const VSimElement& ) = delete;

    void setVisible(bool visible) ;
    bool isVisible() const ;
    uint getId() const;
    virtual const QColor& getColor() const ;
    virtual void reset()  = 0;
    virtual bool isMarkedForRemove() const = 0;
    virtual bool isInjection() const = 0 ;
    virtual bool isVacuum() const = 0 ;
    virtual bool isNormal() const = 0 ;
    virtual double getPressure() const = 0;
    virtual double getFilledPart() const = 0;

protected:
    const VCloth::const_ptr m_pMaterial;
private: 
    uint m_id;
    bool m_visible;
};

#endif //_VSIMELEMENT_H
