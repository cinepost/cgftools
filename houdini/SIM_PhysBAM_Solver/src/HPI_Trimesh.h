#ifndef __HPI_Trimesh_h__
#define __HPI_Trimesh_h__

#include "wrapper_header.h"

#include <vector>
#include <map>
#include <GEO/GEO_Point.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIMZ/SIM_SopGeometry.h>

class HPI_TriMesh {
	public:
		HPI_TriMesh();
		~HPI_TriMesh();
	
		bool			setFromObject(SIM_Object *object, data_exchange::deformable_body *db);
		bool			setToObject(SIM_Object *object, std::vector<data_exchange::vf3> *simulated_points);
		
	private:
		std::map<GEO_Point*, int>		points;							// point_to_index relation
};

#endif
