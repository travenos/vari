/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATIONINFO_H
#define _VSIMULATIONINFO_H

struct VSimulationInfo
{
    double simTime = 0;
    double realTime = 0;
    double realtimeFactor = 0;
    double filledPercent = 0;
    double averagePressure = 0;
    int iteration = 0;
};

#endif //_VSIMULATIONINFO_H
