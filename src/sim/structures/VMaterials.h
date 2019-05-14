/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VMATERIALS_H
#define _VMATERIALS_H

#include <memory>
#include <cmath>
#include <algorithm>
#include <QColor>

class VCloth
{
private:
    double m_cavityHeight;
    double m_xPermeability;
    double m_yPermeability;
    double m_diagPermeability;
    double m_porosity;
    QColor m_baseColor;
    QString m_name;
    double m_angleDeg;
    double m_angleRad;
    double m_angleCos;
    double m_angleSin;
    double m_angleTan;
    double m_angleOrthogTan;
    bool m_isTanInf;
    bool m_isOrthogTanInf;

public:
    typedef std::shared_ptr<VCloth> ptr;
    typedef std::shared_ptr<const VCloth> const_ptr;

    VCloth():
        m_cavityHeight{0.001},
        m_xPermeability{2e-9},
        m_yPermeability{2e-9},
        m_porosity{0.5},
        m_baseColor{QColor(255, 172, 172)}
    {   updateDiag();
        setAngleDeg(0);}

    inline double getCavityHeight() const {return m_cavityHeight;}
    inline double getXPermeability() const {return m_xPermeability;}
    inline double getYPermeability() const {return m_yPermeability;}
    inline double getAvgPermeability() const {return m_diagPermeability;}
    inline double getPorosity() const {return m_porosity;}
    inline double getAngleRad() const {return m_angleRad;}
    inline double getAngleDeg() const {return  m_angleDeg;}
    inline double getAngleCos() const {return  m_angleCos;}
    inline double getAngleSin() const {return  m_angleSin;}
    inline double getAngleTan() const {return  m_angleTan;}
    inline double getAngleOrthogTan() const {return  m_angleOrthogTan;}
    inline const QColor & getBaseColor() const {return m_baseColor;}
    inline const QString & getName() const {return m_name;}
    inline bool isTanInf() const {return m_isTanInf;}
    inline bool isOrthogTanInf() const {return m_isOrthogTanInf;}
    inline float getHeightF() const {return static_cast<float>(m_cavityHeight);}

    inline void setCavityHeight(double cavityHeight) {m_cavityHeight = std::max(cavityHeight, 0.0);}
    inline void setXPermeability(double xPermeability) {m_xPermeability = std::max(xPermeability, 0.0);
                                                       updateDiag();}
    inline void setYPermeability(double yPermeability) {m_yPermeability = std::max(yPermeability, 0.0);
                                                        updateDiag();}
    inline void setPorosity(double porosity) {m_porosity = std::min(std::max(porosity, 0.0), 1.0);}

    inline void setBaseColor(const QColor &color) {m_baseColor = color;}
    inline void setName(const QString &name) {m_name = name;}

    void setAngleDeg(double angle)
    {
        angle = fmod(angle, 360.0);
        int sgnA = angle >= 0 ? 1 : -1;
        if (fabs(angle) > 180.0)
            angle -= 360.0 * sgnA;
        if (fabs(angle) > 90.0)
            angle -= 180.0 * sgnA;
        m_angleDeg = angle;
        m_angleRad = angle * M_PI / 180.0;
        m_angleCos = cos(m_angleRad);
        m_angleSin = sin(m_angleRad);
        m_angleTan = tan(m_angleRad);
        m_angleOrthogTan = tan(m_angleRad + M_PI_2);
        m_isTanInf = (fabs(m_angleDeg) == 90.0);
        m_isOrthogTanInf = (m_angleDeg == 0);
    }

private:
    inline void updateDiag() {m_diagPermeability = (m_xPermeability + m_yPermeability) / 2;}
};

struct VResin
{
    double viscTempcoef{9000};
    double lifetimeTempcoef{6000};
    double defaultViscosity{0.1};
    double defaultLifetime{10800};
    QString name{QStringLiteral("")};
};

#endif //_VMATERIALS_H
