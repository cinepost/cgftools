#ifndef __HPI_Trimesh_h__
#define __HPI_Trimesh_h__

#include <vector>
#include <map>
#include <GEO/GEO_Point.h>
#include <SIM/SIM_Object.h>

class HPI_TriMesh {
	public:
		HPI_TriMesh();
		~HPI_TriMesh();
	
		bool			setFromObject(SIM_Object *object);
		bool			setToObject(SIM_Object *object);
		
	private:
		std::map<GEO_Point*, int>		points;							// point_to_index relation
};

#endif
