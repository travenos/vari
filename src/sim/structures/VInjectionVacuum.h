/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VINJECTIONVACUUM_H
#define _VINJECTIONVACUUM_H

#include <QVector2D>

struct VInjectionVacuum
{
    float injectionDiameter{0.02f};
    float vacuumDiameter{0.02f};
    QVector2D injectionCoords{-0.98f, 0.48f};
    QVector2D vacuumCoords{0.98f, -0.48f};
};

#endif //_VINJECTIONVACUUM_H
