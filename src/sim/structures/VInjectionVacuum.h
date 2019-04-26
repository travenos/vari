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
    QVector2D injectionCoords{0.04f, 0.96f};
    QVector2D vacuumCoords{1.96f, 0.04f};
};

#endif //_VINJECTIONVACUUM_H
