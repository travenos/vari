#include <QColor>
#include <vector>
#include "vtypes.h"

//TODO: move structures to files where they are used
struct VMaterial
{
	double cavityHeight;
	double permability;
	double porosity;
	QColor baseColor;
};

struct VSimulationInfo
{
	double time;
	double filledPercent;
	double medianPressure;
};

struct VLayeredNeighbours
{
	std::vector<VNodePtr> prevLayerNeighbours;
	std::vector<VNodePtr> currentLayerNeighbours;
	std::vector<VNodePtr> nextLayerNeighbours;
};
