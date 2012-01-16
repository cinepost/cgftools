#ifndef __HPI_Trimesh_h__
#define __HPI_Trimesh_h__

#include "wrapper_header.h"

#include <vector>
#include <map>
#include <GEO/GEO_Point.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIMZ/SIM_SopGeometry.h>
#include <SIM/SIM_Position.h>

#include "logtools.h"

typedef std::vector<const GEO_Point*> hpi_trimesh_face;						// Array of GEO_Point*

class HPI_Trimesh {
	public:
		HPI_Trimesh();
		~HPI_Trimesh();
	
		bool				setFromObject(SIM_Object *object);	
		bool				setToObject(SIM_Object *object, std::vector<vf3> *simulated_points);
		
		polygon_mesh		*getMesh();
		std::vector<vf3>	*getPositions();
		
	private:
		std::vector<hpi_trimesh_face>		faces;					// Trimesh faces
		std::map<GEO_Point*, int>			gplist;					// GEO_Point* to GEO_PointList index relation
		std::map<GEO_Point*, int>			points;					// GEO_Point* to PhysBAM mesh index relation

		polygon_mesh 		mesh;
		std::vector<vf3> 	positions;
};

#endif
