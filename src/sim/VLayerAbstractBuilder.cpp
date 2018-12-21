/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VLayerAbstractBuilder.h"

/**
 * VLayerAbstractBuilder implementation
 */

const float VLayerAbstractBuilder::MM_IN_M = 1000;

VLayerAbstractBuilder::VLayerAbstractBuilder(const VCloth &material) :
    m_pMaterial(new VCloth(material)),
    m_nodeStartId(0),
    m_triangleStartId(0),
    m_nodeMaxId(0),
    m_triangleMaxId(0)
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
    return m_nodeMaxId;
}

uint VLayerAbstractBuilder::getTriangleMaxId() const
{
    return m_triangleMaxId;
}
