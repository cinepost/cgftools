#ifndef __SIM_pbDefVisualize_h__
#define __SIM_pbDefVisualize_h__

#include <iostream>

#include <UT/UT_HashTable.h>
#include <UT/UT_Hash.h>
#include <UT/UT_IStream.h>
#include <UT/UT_VoxelArray.h>
#include <GU/GU_DetailHandle.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_SingleSolver.h>
#include <SIM/SIM_Geometry.h>

class SIM_pbDefVisualize : public SIM_Data,
			public SIM_OptionsUser
{
public:
	// This member not part of the SIM_Geometry interface;
	// it's only accessed by your own solver
	//MyOwnRepresentation* getMyOwnRepresentation();
	GET_GUIDE_FUNC_B(SIM_NAME_SHOWGUIDE, ShowGuide, true);
    GET_GUIDE_FUNC_V3(SIM_NAME_COLOR, Color, (1, 0, 0));	

protected:
	explicit		SIM_pbDefVisualize(const SIM_DataFactory *factory);
    virtual			~SIM_pbDefVisualize();
    
	// Members that need to be implemented for your own geometry
	virtual void		initializeSubclass();
	
    virtual bool		getIsAlternateRepresentationSubclass() const;
    virtual void		initAlternateRepresentationSubclass(const SIM_Data &);	
	virtual SIM_Guide	*createGuideObjectSubclass() const;
	virtual void		buildGuideGeometrySubclass(const SIM_RootData &root,
							const SIM_Options &options,
							const GU_DetailHandle &gdh,
							UT_DMatrix4 *xform,
							const SIM_Time &t) const;
						
    static void			createBoundingBoxGuide(GU_Detail *gdp,
							const UT_BoundingBox &bbox,
							const UT_Vector3 &color);
											
private:
	//MyOwnRepresentation* myOwnRepresentation;
	static const SIM_DopDescription *getDopDescription();
	mutable GU_DetailHandle myDetailHandle; // Cached GU_Detail

	DECLARE_STANDARD_GETCASTTOTYPE();
	DECLARE_DATAFACTORY(SIM_pbDefVisualize, SIM_Geometry, "PhysBAM_Deformable_Geometry", getDopDescription());
};

#endif
