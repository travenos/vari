#include <memory>
#include <QColor>

struct VCloth
{
    typedef std::shared_ptr<VCloth> ptr;
    typedef std::shared_ptr<const VCloth> const_ptr;
    double cavityHeight;
    double permeability;
    double porosity;
    QColor baseColor;
};
