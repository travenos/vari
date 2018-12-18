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
    m_pParam(p_simParam),
    m_nodeStartId(0),
    m_triangleStartId(0),
    m_NodeMaxId(0),
    m_TriangleMaxId(0)
{}

VLayerAbstractBuilder::~VLayerAbstractBuilder()
{}

void VLayerAbstractBuilder::setNodeStartId(uint id)
{
    m_nodeStartId = id;
}

void VLayerAbstractBuilder::setTriangleStartId(uint id)
{
    m_triangleStartId = id;
}

uint VLayerAbstractBuilder::getNodeMaxId() const
{
    return m_NodeMaxId;
}

uint VLayerAbstractBuilder::getTriangleMaxId() const
{
    return m_TriangleMaxId;
}
