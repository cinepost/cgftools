#include "SIM_pbDefVisualize.h"

#include <UT/UT_DSOVersion.h>
#include <UT/UT_Floor.h>
#include <UT/UT_Vector3.h>
#include <UT/UT_WorkBuffer.h>
#include <GEO/GEO_AttributeHandle.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimPart.h>
#include <GU/GU_PrimPoly.h>
#include <GU/GU_RayIntersect.h>
#include <GU/GU_PrimTriStrip.h>
#include <PRM/PRM_Include.h>
#include <SIM/SIM_Engine.h>
#include <SIM/SIM_Options.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ObjectArray.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Random.h>
#include <SIM/SIM_RandomTwister.h>
#include <SIM/SIM_Position.h>
#include <SIM/SIM_PRMShared.h>
#include <SIM/SIM_Guide.h>
#include <SIM/SIM_GuideShared.h>

SIM_pbDefVisualize::SIM_pbDefVisualize(const SIM_DataFactory *factory)
 : BaseClass(factory), SIM_OptionsUser(this)
{

}

SIM_pbDefVisualize::~SIM_pbDefVisualize()
{
	// freeArray();	
}

const SIM_DopDescription *
SIM_pbDefVisualize::getDopDescription()
{
	static PRM_Name	 theGuideBox("usebox", "Bounding Box");

    static PRM_Template	 theTemplates[] = {
	PRM_Template()
    };

	static PRM_Template	 theGuideTemplates[] = {
	PRM_Template(PRM_TOGGLE,	1, &SIMshowguideName, PRMzeroDefaults),
	PRM_Template(PRM_RGB,		3, &SIMcolorName, PRMoneDefaults, 0, &PRMunitRange),	
	PRM_Template()
    };

    static SIM_DopDescription	 theDopDescription(true,
						   "physbam_defvisualize",
						   "PhysBAM Deformable Visualize",
						   "Visualization",
						   //"PBM_DeformableGeometry",
						   classname(),
						   theTemplates);
	theDopDescription.setGuideTemplates(theGuideTemplates);					   

    return &theDopDescription;
}

void
SIM_pbDefVisualize::initializeSubclass()
{
    BaseClass::initializeSubclass();
    //freeArray();
    myDetailHandle.clear();
}

void
SIM_pbDefVisualize::buildGuideGeometrySubclass(const SIM_RootData &root,
				    const SIM_Options &options,
				    const GU_DetailHandle &gdh,
				    UT_DMatrix4 *,
				    const SIM_Time &) const
{
	std::cout << "Build guide geo..." << std::endl;
    // Build our template geometry, if we are so asked.
    initAlternateRepresentation();

	///if (gdh.isNull())
	///return;

    GU_DetailHandleAutoWriteLock	gdl(gdh);
    GU_Detail						*gdp = gdl.getGdp();
    UT_BoundingBox					bbox;
    UT_Vector3						color(1,1,1);
    GEO_AttributeHandle				cd_gah;

    //color = getColor(options);

    // Find our bounding box.
    UT_Vector3				bbmin(-1,-1,-1), bbmax(1,1,1);
    bbox.initBounds(bbmin, bbmax);

    cd_gah = gdp->getPointAttribute("Cd");
    if (!cd_gah.isAttributeValid())
    {
	static float one[3] = { 1, 1, 1 };
	gdp->addPointAttrib("Cd", 3 * sizeof(float), GB_ATTRIB_FLOAT, one);
	cd_gah = gdp->getPointAttribute("Cd");
    }

    //if (getUseBox(options))
	//createBoundingBoxGuide(gdp, bbox, color);
	gdp->polymeshCube (3, 3, 3, -0.5F, 0.5F, -0.5F, 0.5F, -0.5F, 0.5F, GEO_PATCH_TRIANGLE, true);
}

void
SIM_pbDefVisualize::createBoundingBoxGuide(GU_Detail *gdp,
		const UT_BoundingBox &bbox,
		const UT_Vector3 &color)
{
    GEO_Point	*corners[8];
    int		 idx1, idx2;
    UT_Vector3	 pos;
    GU_PrimPoly	*line;
    GEO_AttributeHandle	cd_gah;

    cd_gah = gdp->getPointAttribute("Cd");
    if (!cd_gah.isAttributeValid())
    {
	static float one[3] = { 1, 1, 1 };
	gdp->addPointAttrib("Cd", 3 * sizeof(float), GB_ATTRIB_FLOAT, one);
	cd_gah = gdp->getPointAttribute("Cd");
    }

    GB_PrimitiveGroup	*bboxgrp;

    bboxgrp = gdp->newPrimitiveGroup("bbox");

    // Create the 8 points with the rule that
    // we use max(axis) if idx1 & (1 << axis) is true.
    for (idx1 = 0; idx1 < 8; idx1++)
    {
	corners[idx1] = gdp->appendPoint();

	pos.x() = (idx1 & 1) ? bbox.xmax() : bbox.xmin();
	pos.y() = (idx1 & 2) ? bbox.ymax() : bbox.ymin();
	pos.z() = (idx1 & 4) ? bbox.zmax() : bbox.zmin();
	corners[idx1]->getPos() = pos;

	cd_gah.setElement(corners[idx1]);
	cd_gah.setV3(color);
    }

    // Create each edge.  Edges are in increaing direction of index.
    // Two indicies are connected if they differ in one and only one
    // axis.  Brute force and ignorance saves the day.
    // We run the n^2 possible indices and only build if the bit
    // field of differences matches a desired pattern.
    for (idx1 = 0; idx1 < 8; idx1++)
    {
	for (idx2 = idx1+1; idx2 < 8; idx2++)
	{
	    switch (idx1 ^ idx2)
	    {
		case 1:
		case 2:
		case 4:
		    line = GU_PrimPoly::build(gdp, 2, GU_POLY_OPEN, 0);
		    bboxgrp->add(line);

		    (*line)(0).setPt(corners[idx1]);
		    (*line)(1).setPt(corners[idx2]);

		    break;
	    }
	}
    }
}

SIM_Guide *
SIM_pbDefVisualize::createGuideObjectSubclass() const
{
    // Return a shared guide so that we only have to build our geometry
    // once. But set the displayonce flag to false so that we can set
    // a different transform for each object.
    return new SIM_GuideShared(this, false);
}

bool
SIM_pbDefVisualize::getIsAlternateRepresentationSubclass() const
{
    return false;
}

void
SIM_pbDefVisualize::initAlternateRepresentationSubclass(const SIM_Data &parent)
{
    const SIM_pbDefVisualize	*sf = SIM_DATA_CASTCONST(&parent, SIM_pbDefVisualize);

    //myArray = sf;
}
