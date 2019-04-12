/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VINJECTIONVACUUM_H
#define _VINJECTIONVACUUM_H

#include <QVector2D>

struct VInjectionVacuum
{
    float injectionDiameter{0.04f};
    float vacuumDiameter{0.04f};
    QVector2D injectionCoords{-0.96f, 0.46f};
    QVector2D vacuumCoords{0.96f, -0.46f};
};

#endif //_VINJECTIONVACUUM_H
