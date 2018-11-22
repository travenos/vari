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
