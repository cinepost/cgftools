#ifndef __HPI_trimesh_h__
#define __HPI_trimesh_h__

#include <vector>
#include <map>
#include <GEO/GEO_Point.h>

struct HPI_TriMesh {
	physbam_object*					sim_object;	
	std::map<GEO_Point*, int>		points;								// point_to_index relation
} ;

#endif
