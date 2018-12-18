/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VEXCEPTIONS_H
#define _VEXCEPTIONS_H

#include <exception>

class VException: public std::exception
{
};

class VSimulatorException: public VException
{
};

class VLayersException: public VException
{
};

class VImportException: public VException
{
};

#endif //_VEXCEPTIONS_H
