#ifndef __SIM_pbGeometry_h__
#define __SIM_pbGeometry_h__

#include "HPI_trimesh.h"

#include <iostream>

#include <GU/GU_Detail.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_OptionsUser.h>

#include "SIM_pbWorldData.h"

#define NAME_MASS	"mass"

class SIM_pbGeometry : 	public SIM_Geometry
						//,public SIM_OptionsUser
{
public:
	explicit		SIM_pbGeometry(const SIM_DataFactory *factory);
    virtual			~SIM_pbGeometry();
    
	/// Controls the object's resistance.
	GETSET_DATA_FUNCS_F(NAME_MASS, Mass);    
    
	// This members not part of the SIM_Geometry interface; It's only accessed by our own solver
	HPI_TriMesh*	getMesh();
	void			setMesh(HPI_TriMesh* inmesh);

protected:
	// Members that need to be implemented for your own geometry
	virtual void 	initializeSubclass();
	virtual void 	makeEqualSubclass(const SIM_Data *source);
	virtual void 	handleModificationSubclass(int code);
	virtual bool 	getIsAlternateRepresentationSubclass () const;
	virtual void	initAlternateRepresentationSubclass (const SIM_Data &);
	virtual int64	getMemorySizeSubclass() const;

	// Allows access to the GU_Detail
	virtual 		GU_ConstDetailHandle getGeometrySubclass() const;
											
	mutable GU_DetailHandle	myDetailHandle; // Cached GU_Detail
	
	HPI_TriMesh		*mesh;
	
private:
    static const SIM_DopDescription     *getDopDescription();
    DECLARE_STANDARD_GETCASTTOTYPE();
	DECLARE_DATAFACTORY(SIM_pbGeometry, SIM_Geometry, "PhysBAM_Geometry", getDopDescription());	
	
};

#endif
