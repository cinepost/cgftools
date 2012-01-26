#include <UT/UT_DSOVersion.h>
#include <stdio.h>
#include <iostream>
#include <UT/UT_Assert.h>
#include <GEO/GEO_AttributeHandle.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimVolume.h>
#include <GU/GU_PrimTriStrip.h>
#include <GEO/GEO_IOTranslator.h>
#include <SOP/SOP_Node.h>

namespace HDK_Sample {

class GEO_TetmeshIOTranslator : public GEO_IOTranslator
{
public:
	     GEO_TetmeshIOTranslator() {}
    virtual ~GEO_TetmeshIOTranslator() {}

    virtual const char *formatName() const;

    virtual int		checkExtension(const char *name);

    virtual int		checkMagicNumber(unsigned magic);

    virtual bool	fileLoad(GEO_Detail *gdp, UT_IStream &is, int ate_magic);
    virtual int		fileSave(const GEO_Detail *gdp, ostream &os);
};

}

using namespace HDK_Sample;

const char *
GEO_TetmeshIOTranslator::formatName() const
{
    return "Tetrahedral Mesh Format";
}

int
GEO_TetmeshIOTranslator::checkExtension(const char *name) 
{
    UT_String		sname(name);

    if (sname.fileExtension() && !strcmp(sname.fileExtension(), ".msh"))
	return true;
    return false;
}

int
GEO_TetmeshIOTranslator::checkMagicNumber(unsigned magic)
{
    return 0;
}

bool
GEO_TetmeshIOTranslator::fileLoad(GEO_Detail *gdp, UT_IStream &is, int ate_magic)
{	
	bool			geo_ready = false;
	unsigned int	n_points = 0;
	unsigned int 	n_tetras = 0;
	
    /// Convert our stream to ascii.
    UT_IStreamAutoBinary	forceascii(is, false);
	
	is.read(&n_points);
	is.read(&n_tetras);
	
	if((n_points == 0) || (n_tetras == 0)){
		std::cerr << "Broken .msh file. Unable to determine points and tetras count !!!" << std::endl;
		return false;
	}else{
		std::cout << "Reading .msh file.  Pts: " << n_points << " Tets: " << n_tetras << std::endl;
		while(!is.isEof() && (geo_ready == false)){
			/// read points
			float tmp_pt[3];
			for(int i = 0; i < n_points; i++){
				is.read(tmp_pt, 3);
				gdp->appendPoint()->setPos(tmp_pt[0], tmp_pt[1], tmp_pt[2], 1);
			}
			/// read tetras
			int tmp_tet[4];
			GU_PrimTriStrip     *tristrip;
			GEO_PointList 		pts = gdp->points();
			for(int i = 0; i < n_tetras; i++){
				tristrip =  GU_PrimTriStrip::build((GU_Detail*)gdp, 6, 0);
				is.read(tmp_tet, 4);
				(*tristrip)(0).setPt(pts[tmp_tet[3]]);
				(*tristrip)(1).setPt(pts[tmp_tet[1]]);
				(*tristrip)(2).setPt(pts[tmp_tet[0]]);
				(*tristrip)(3).setPt(pts[tmp_tet[2]]);
				(*tristrip)(4).setPt(pts[tmp_tet[3]]);
				(*tristrip)(5).setPt(pts[tmp_tet[1]]);
			}
			
			geo_ready = true;
		}	
	}
    // All done successfully
    return true;
}

int
GEO_TetmeshIOTranslator::fileSave(const GEO_Detail *gdp, ostream &os)
{
	/*
    // Write our magic token.
    os << "VOXELS" << endl;

    // Now, for each volume in our gdp...
    const GEO_Primitive		*prim;
    GEO_AttributeHandle			 name_gah;
    UT_String				 name;
    UT_WorkBuffer			 buf;

    name_gah = gdp->getPrimAttribute("name");
    FOR_ALL_PRIMITIVES(gdp, prim)
    {
	if (prim->getPrimitiveId() == GEOPRIMVOLUME)
	{
	    // Default name
	    buf.sprintf("volume_%d", prim->getNum());
	    name.harden(buf.buffer());

	    // Which is overridden by any name attribute.
	    if (name_gah.isAttributeValid())
	    {
		name_gah.setElement(prim);
		name_gah.getString(name);
	    }

	    os << "VOLUME " << name << endl;
	    const GEO_PrimVolume	*vol = (GEO_PrimVolume *) prim;

	    int		resx, resy, resz;

	    // Save resolution
	    vol->getRes(resx, resy, resz);
	    os << resx << " " << resy << " " << resz << endl;

	    // Save the center and approximate size.
	    // Calculating the size is complicated as we could be rotated
	    // or sheared.  We lose all these because the .voxel format
	    // only supports aligned arrays.
	    UT_Vector3		p1, p2;

	    os << vol->getVertex().getPt()->getPos().x() << " "
	       << vol->getVertex().getPt()->getPos().y() << " "
	       << vol->getVertex().getPt()->getPos().z() << endl;

	    vol->indexToPos(0, 0, 0, p1);
	    vol->indexToPos(1, 0, 0, p2);
	    os << resx * (p1 - p2).length() << " ";
	    vol->indexToPos(0, 1, 0, p2);
	    os << resy * (p1 - p2).length() << " ";
	    vol->indexToPos(0, 0, 1, p2);
	    os << resz * (p1 - p2).length() << endl;

	    UT_VoxelArrayReadHandleF handle = vol->getVoxelHandle();

	    // Enough of a header, dump the data.
	    os << "{" << endl;
	    for (int z = 0; z < resz; z++)
	    {
		for (int y = 0; y < resy; y++)
		{
		    os << "    ";
		    for (int x = 0; x < resx; x++)
		    {
			os << (*handle)(x, y, z) << " ";
		    }
		    os << endl;
		}
	    }
	    os << "}" << endl;
	    os << endl;
	}
    }
	*/
    return true;
}

void
newGeometryIO(void *)
{
    GU_Detail::registerIOTranslator(new GEO_TetmeshIOTranslator());
}
