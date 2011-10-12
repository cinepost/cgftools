#ifndef __HIP_trimesh_h__
#define __HIP_trimesh_h__

#include <vector>
#include <map>
#include <GEO/GEO_Point.h>

struct HIP_TriMesh {
	physbam_object*					sim_object;	
	std::vector<GEO_Point*, int>	points;								// point_to_index relation
} ;

#endif
