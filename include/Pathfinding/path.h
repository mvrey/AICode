
// project A Star
// path.h
// author: Toni Barella
// Artificial Intelligence - 3VGP
// ESAT 2016/2017

#ifndef __PATH__
#define __PATH__

#include "../../include/config.h"

class CostMap;

class Path
{
public:
	Path();
	Path(const Path& orig);
	~Path();

	bool Add(::MOMOS::Vec2 position);
	int Length();
	::MOMOS::Vec2 NextPoint();

	void Print(CostMap *map); // Prints the path's positions
	
	std::vector<::MOMOS::Vec2> path_;
	int current_point_;
};
#endif // __PATH__