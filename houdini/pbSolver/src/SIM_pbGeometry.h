#ifndef __SIM_pbGeometry_h__
#define __SIM_pbGeometry_h__

#include "HPI_trimesh.h"

#include <iostream>

#include <GU/GU_Detail.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>

#include "SIM_pbWorldData.h"

class SIM_pbGeometry : public SIM_Geometry
{
public:
	explicit		SIM_pbGeometry(const SIM_DataFactory *factory);
    virtual			~SIM_pbGeometry();
    
	// This members not part of the SIM_Geometry interface; It's only accessed by your own solver
	HPI_TriMesh*	getMesh();
	void			setMesh(HPI_TriMesh* inmesh);

protected:
	// This members not part of the SIM_Geometry interface; It's only accessed by your own solver
	virtual	bool	addGeometryToSimulation(physbam_simulation *simulation)=0;

protected:
	// Members that need to be implemented for your own geometry
	virtual void 	initializeSubclass();
	virtual void 	makeEqualSubclass(const SIM_Data *source);
	virtual void 	handleModificationSubclass(int code);
	
	virtual int64	getMemorySizeSubclass() const;

	// Allows access to the GU_Detail
	virtual 		GU_ConstDetailHandle getGeometrySubclass() const;
											
	mutable GU_DetailHandle myDetailHandle; // Cached GU_Detail
	
	HPI_TriMesh		*mesh;
};

#endif
