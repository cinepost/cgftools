#ifndef __SIM_pbSolver_h__
#define __SIM_pbSolver_h__

#include "HPI_trimesh.h"

class HPI_Object{
public:
	HPI_Object();
	~HPI_Object();
	
private:
	HPI_TriMesh			trimesh;
	physbam_object		object;
	
private:	
	HPI_TriMesh*		getTriMesh();
	physbam
	void				setTriMesh(HPI_TriMesh* inmesh);
};

#endif
