#ifndef UTILITY_H
#define UTILITY_H
#include "point.h"
using namespace Entities;
class Utility {
private:
	Utility();

public:
	struct ArcInfo {
		double radius;
		Point center;
		double startAngle;
		double endAngle;
		bool bUse;
		std::vector<PointF> arc;
		ArcInfo() : radius(0.0), startAngle(0.0), endAngle(0.0), bUse(false) {}
	};

public:
	static double  CalAngle(const Point &center, const Point &vertex0);
	static ArcInfo CalArcInfo(const Point &vertex0, const Point &vertex1);
	static double  Distance(const Point &vertex0, const Point &vertex1);
};

#endif // UTILITY_H
