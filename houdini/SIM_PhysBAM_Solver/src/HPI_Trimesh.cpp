#include "HPI_Trimesh.h"

HPI_Trimesh::HPI_Trimesh() {
	gplist.clear();
	points.clear();
}

HPI_Trimesh::~HPI_Trimesh() {
}

polygon_mesh*
HPI_Trimesh::getMesh(){
	return &mesh;
}

std::vector<vf3>*
HPI_Trimesh::getPositions(){
	return &positions;
}

bool
HPI_Trimesh::setFromObject(SIM_Object *object){
	SIM_SopGeometry* geometry = SIM_DATA_CAST(object->getNamedSubData("Geometry"), SIM_SopGeometry);

	const SIM_Position *sim_pos = object->getPosition();
	UT_DMatrix4		xform;
	sim_pos->getTransform(xform);
	const UT_Matrix4	c_xform(xform);
	geometry->getTransform(xform);
	const UT_Matrix4	o_xform(xform);
	
	const GU_Detail* const sgdp(geometry->getGeometry().readLock());
	GU_Detail* gdp = new GU_Detail(sgdp);
	gdp->transform(o_xform);
	gdp->transform(c_xform);
		
	LOG("Fetched gdp from object is:"  << gdp);
	const GEO_PointList					*p_list = &gdp->points();
	const GEO_Primitive					*prim;
	GEO_Point							*p;

	std::vector<vf3> 	position;	
	int pb_vertex_index = 0;	

	for(int i = 0; i < p_list->entries(); i++)
	{
		gplist.insert ( std::pair<GEO_Point*, int>((GEO_Point*)p_list->entry(i), i) );
	}

	int i1, i2, i3, i4;
	pair< std::map<GEO_Point*, int>::iterator,bool > ret;
	FOR_ALL_PRIMITIVES(gdp, prim)
	{
		if(prim->getPrimitiveId() == GEOPRIMPOLY){
			switch(prim->getVertexCount()){
				case 3:
					p = prim->getVertex(0).getPt();
					ret = points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i1 = ret.first->second; // this point already existed. use existing index
					}else{
						positions.push_back(vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i1 = pb_vertex_index++;
					}
					
					p = prim->getVertex(1).getPt();
					ret = points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i2 = ret.first->second; // this point already existed. use existing index
					}else{
						positions.push_back(vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i2 = pb_vertex_index++;
					}
					
					p = prim->getVertex(2).getPt();
					ret = points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i3 = ret.first->second; // this point already existed. use existing index
					}else{
						positions.push_back(vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i3 = pb_vertex_index++;
					}
					
					 mesh.insert_polygon(vi3(i3, i2, i1));
					break;
				case 4:
					p = prim->getVertex(0).getPt();
					ret = points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i1 = ret.first->second; // this point already existed. use existing index
					}else{
						positions.push_back(vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i1 = pb_vertex_index++;
					}	
					
					p = prim->getVertex(1).getPt();
					ret = points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i2 = ret.first->second; // this point already existed. use existing index
					}else{
						positions.push_back(vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i2 = pb_vertex_index++;
					}
					
					p = prim->getVertex(2).getPt();
					ret = points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i3 = ret.first->second; // this point already existed. use existing index
					}else{
						positions.push_back(vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i3 = pb_vertex_index++;
					}
						
					p = prim->getVertex(3).getPt();
					ret = points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i4 = ret.first->second; // this point already existed. use existing index
					}else{
						positions.push_back(vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i4 = pb_vertex_index++;
					}
					
					mesh.insert_polygon(vi4(i4, i3, i2, i1));		
					break;
				default:
					std::cout << prim->getVertexCount();
					break;
			}
		}
	}
	delete gdp;
	return true;
}


bool
HPI_Trimesh::setToObject(SIM_Object *object, std::vector<vf3> *simulated_points){
	/// Get the object's last state before this time step
	SIM_GeometryCopy *geometry_copy = SIM_DATA_CREATE(*object, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy, SIM_DATA_RETURN_EXISTING | SIM_DATA_ADOPT_EXISTING_ON_DELETE);	

	if(geometry_copy){
		GU_DetailHandleAutoWriteLock	gdl(geometry_copy->lockGeometry());
		GEO_PointList					*p_list = &gdl.getGdp()->points();
		GEO_Point						*pt;

		std::map<GEO_Point*, int>::iterator iter;
		int pb_index;
		
		for (iter = gplist.begin(); iter != gplist.end(); iter++) {		
			pb_index = points[iter->first];
			
			pt = p_list->entry(iter->second);
			
			pt->setPos(simulated_points->at(pb_index).data[0], simulated_points->at(pb_index).data[1], simulated_points->at(pb_index).data[2], 1);
		}
		
		const SIM_Position *sim_pos = object->getPosition();
		UT_DMatrix4		xform;
		sim_pos->getInverseTransform(xform);
		const UT_Matrix4	c_xform(xform);
		object->getGeometry()->getTransform(xform);
		xform.invert();
		const UT_Matrix4	o_xform(xform);
		
		gdl.getGdp()->transform(c_xform);
		gdl.getGdp()->transform(o_xform);
		geometry_copy->releaseGeometry(); /// Store the integrated simulation state in geometry_copy
		return true;
	}
	std::cout << "Unable to get GeometryCopy for object: " << object->getObjectId() << " to update!" <<std::endl;
	return false;	
}
