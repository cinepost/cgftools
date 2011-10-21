#ifndef __SIM_pbDefGeometry_h__
#define __SIM_pbDefGeometry_h__

#include "HPI_trimesh.h"

#include <iostream>

#include <GU/GU_Detail.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>

#define SNOW_NAME_DIVISIONS	"div"
#define SNOW_NAME_CENTER	"t"
#define SNOW_NAME_SIZE		"size"

class SIM_pbDefGeometry : public SIM_Geometry
{
public:
	// This member not part of the SIM_Geometry interface;
	// it's only accessed by your own solver
	//MyOwnRepresentation* getMyOwnRepresentation();
	HPI_TriMesh*	getMesh();
	void			setMesh(HPI_TriMesh* inmesh);

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

	// Allows access to the GU_Detail
	virtual GU_ConstDetailHandle getGeometrySubclass() const;
											
private:
	//MyOwnRepresentation* myOwnRepresentation;
	static const SIM_DopDescription *getDopDescription();
	mutable GU_DetailHandle myDetailHandle; // Cached GU_Detail

	DECLARE_STANDARD_GETCASTTOTYPE();
	DECLARE_DATAFACTORY(SIM_pbDefGeometry, SIM_Geometry, "PhysBAM_Deformable_Geometry", getDopDescription());
	
	HPI_TriMesh		*mesh;
};

#endif
