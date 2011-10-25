#ifndef __SIM_pbDefGeometry_h__
#define __SIM_pbDefGeometry_h__

#include "HPI_trimesh.h"
#include "SIM_pbGeometry.h"

#include <iostream>

#include <GU/GU_Detail.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>

class SIM_pbDefGeometry : public SIM_pbGeometry
{

protected:
	explicit		SIM_pbDefGeometry(const SIM_DataFactory *factory);
    virtual			~SIM_pbDefGeometry();
    
	// Members that need to be implemented for your own geometry
	virtual void 	initializeSubclass();
	virtual void 	makeEqualSubclass(const SIM_Data *source);
	virtual void 	saveSubclass(ostream &os) const;
	virtual bool 	loadSubclass(UT_IStream &is);
	virtual void 	handleModificationSubclass(int code);
	
	virtual void	optionChangedSubclass(const char *name);
	virtual int64	getMemorySizeSubclass() const;
											
	virtual	bool	addGeometryToSimulation(physbam_simulation *simulation);										
											
private:
	static const SIM_DopDescription *getDopDescription();

	DECLARE_STANDARD_GETCASTTOTYPE();
	DECLARE_DATAFACTORY(SIM_pbDefGeometry, SIM_Geometry, "PhysBAM_Deformable_Geometry", getDopDescription());
};

#endif
