#ifndef __SIM_pbGeometry_h__
#define __SIM_pbGeometry_h__

#include "HPI_trimesh.h"

#include <iostream>

#include <OP/OP_Context.h>
#include <OP/OP_Director.h>
#include <OBJ/OBJ_Node.h>
#include <SOP/SOP_Node.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_SpareData.h>
#include <GU/GU_Detail.h>
#include <SIM/SIM_Data.h>
#include <SIM/SIM_Engine.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_Names.h>
#include <SIMZ/SIMZ_Utils.h>
#include <UT/UT_XformOrder.h>

#include "SIM_pbWorldData.h"

#define NAME_DISPLAY_GEO	"display_geo"
#define NAME_TRANSLATE		"init_translate"
#define NAME_ROTATE			"init_rotate"
#define NAME_SCALE			"init_scale"

class SIM_pbGeometry : 	public SIM_Geometry
{
public:
	explicit		SIM_pbGeometry(const SIM_DataFactory *factory);
    virtual			~SIM_pbGeometry();
  
	GU_ConstDetailHandle 	getGeometry () const;
    
	/// Controls the object's resistance.
	GETSET_DATA_FUNCS_B(NAME_DISPLAY_GEO, DisplayGeo);
	GETSET_DATA_FUNCS_S(SIM_NAME_SOPPATH, SopPath);
	GETSET_DATA_FUNCS_S(SIM_NAME_PRIMGROUP, PrimGroup); 
    GETSET_DATA_FUNCS_B(SIM_NAME_USETRANSFORM, UseTransform);
    GETSET_DATA_FUNCS_V3(NAME_TRANSLATE, InitTranslate);
	GETSET_DATA_FUNCS_V3(NAME_ROTATE, InitRotate);
	GETSET_DATA_FUNCS_V3(NAME_SCALE, InitScale);
    
	/// This members not part of the SIM_Geometry interface; It's only accessed by our own solver
	HPI_TriMesh*		getMesh();
	void				setMesh(HPI_TriMesh* inmesh);

protected:
	/// Members that need to be implemented for your own geometry
	virtual void 		initializeSubclass();
	virtual void 		makeEqualSubclass(const SIM_Data *source);
	virtual void 		handleModificationSubclass(int code);
	virtual int64		getMemorySizeSubclass() const;
	virtual SIM_Guide	*createGuideObjectSubclass() const;
	virtual void 		buildGuideGeometrySubclass (const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const; 

	/// Allows access to the GU_Detail
	virtual 			GU_ConstDetailHandle getGeometrySubclass() const;
	
	HPI_TriMesh		*mesh;
	
private:
	mutable GU_DetailHandle	myDetailHandle; // Cached GU_Detail

    static const SIM_DopDescription     *getDopDescription();
    DECLARE_STANDARD_GETCASTTOTYPE();
	DECLARE_DATAFACTORY(SIM_pbGeometry, SIM_Data, "PhysBAM_Geometry", getDopDescription());	
	
};

#endif
