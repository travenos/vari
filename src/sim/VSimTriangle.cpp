/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VSimTriangle.h"

/**
 * VSimTriangle implementation
 */

/**
 * @param p_material
 * @param p_node0
 * @param p_node1
 * @param p_node2
 */
VSimTriangle::VSimTriangle(const VCloth::const_ptr &p_material,
                           const VSimulationParametres::const_ptr &p_param,
                           const VSimNode::const_ptr &p_node0,
                           const VSimNode::const_ptr &p_node1,
                           const VSimNode::const_ptr &p_node2) :
    VSimElement(p_material, p_param),
    m_pNodes{p_node0.get(), p_node1.get(), p_node2.get()},
    m_color(p_material->baseColor)
{

}

/**
 * @return std::vector<QVector3D>
 */
void VSimTriangle::getVertices(QVector3D vertices[VERTICES_NUMBER]) const noexcept
{
    for (unsigned int i = 0; i < VERTICES_NUMBER; ++i)
        vertices[i] = m_pNodes[i]->getPosition();
}

void VSimTriangle::updateColor() noexcept
{
    double filledPart = getAveragePressure() / (m_pParam->injectionPressure - m_pParam->vacuumPressure);
    // TODO talk about this
    /*
    int colorValue = m_color.value();
    colorValue *= (1 - filledPart);
    m_color.setHsv(m_color.hue(), m_color.saturation(), colorValue);
    */
    const QColor DEFAULT_COLOR = QColor(255, 172, 172); //TODO Make a constant
    int colorValue = DEFAULT_COLOR.value();
    colorValue *= (1 - filledPart);
    m_color.setHsv(DEFAULT_COLOR.hue(), DEFAULT_COLOR.saturation(), colorValue);
}

/**
 * @return QColor&
 */
const QColor &VSimTriangle::getColor() const noexcept
{
    return m_color;
}

double VSimTriangle::getAveragePressure() const noexcept
{
    double pressure = 0;
    for (unsigned int i = 0; i < VERTICES_NUMBER; ++i)
        pressure += m_pNodes[i]->getPressure();
    return pressure / VERTICES_NUMBER;
}

void VSimTriangle::reset() noexcept
{
    m_color = m_pMaterial->baseColor;
}
