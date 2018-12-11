/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VLayerAbstractBuilder.h"

/**
 * VLayerAbstractBuilder implementation
 */

const float VLayerAbstractBuilder::MM_IN_M = 1000;

VLayerAbstractBuilder::VLayerAbstractBuilder(const VCloth &material,
                      const VSimulationParametres::const_ptr &p_simParam) :
    m_pMaterial(new VCloth(material)),
    m_pParam(p_simParam)
{

}

VLayerAbstractBuilder::~VLayerAbstractBuilder()
{}
