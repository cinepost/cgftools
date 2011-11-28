#ifndef __HPI_trimesh_h__
#define __HPI_trimesh_h__

#include "wrapper_header.h"
#include <vector>
#include <map>
#include <GEO/GEO_Point.h>

struct HPI_TriMesh {
	std::map<GEO_Point*, int>		points;								// point_to_index relation
};

#endif
