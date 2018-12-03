/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VLayerAbstractBuilder.h"

/**
 * VLayerAbstractBuilder implementation
 */

VLayerAbstractBuilder::VLayerAbstractBuilder(const VCloth &material,
                      const VSimulationParametres::const_ptr &p_simParam) :
    m_pMaterial(new VCloth(material)),
    m_pParam(p_simParam)
{

}

VLayerAbstractBuilder::~VLayerAbstractBuilder()
{}
