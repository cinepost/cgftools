#include "HPI_Trimesh.h"

HPI_Trimesh::HPI_Trimesh() {
	gplist.clear();
	points.clear();
}

HPI_Trimesh::~HPI_Trimesh() {
}

bool
HPI_Trimesh::setFromObject(SIM_Object *object, data_exchange::deformable_body *db){
	SIM_SopGeometry* geometry = SIM_DATA_CAST(object->getNamedSubData("Geometry"), SIM_SopGeometry);
	
	const GU_Detail* const gdp(geometry->getGeometry().readLock());
	
	LOG("Fetched gdp from object is:"  << gdp);
	const GEO_PointList					*p_list = &gdp->points();
	const GEO_Primitive					*prim;
	GEO_Point							*p;
	
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
						db->position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i1 = pb_vertex_index++;
					}
					
					p = prim->getVertex(1).getPt();
					ret = points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i2 = ret.first->second; // this point already existed. use existing index
					}else{
						db->position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i2 = pb_vertex_index++;
					}
					
					p = prim->getVertex(2).getPt();
					ret = points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i3 = ret.first->second; // this point already existed. use existing index
					}else{
						db->position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i3 = pb_vertex_index++;
					}
					
					db->mesh.insert_polygon(data_exchange::vi3(i3, i2, i1));
					break;
				case 4:
					p = prim->getVertex(0).getPt();
					ret = points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i1 = ret.first->second; // this point already existed. use existing index
					}else{
						db->position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i1 = pb_vertex_index++;
					}	
					
					p = prim->getVertex(1).getPt();
					ret = points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i2 = ret.first->second; // this point already existed. use existing index
					}else{
						db->position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i2 = pb_vertex_index++;
					}
					
					p = prim->getVertex(2).getPt();
					ret = points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i3 = ret.first->second; // this point already existed. use existing index
					}else{
						db->position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i3 = pb_vertex_index++;
					}
						
					p = prim->getVertex(3).getPt();
					ret = points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i4 = ret.first->second; // this point already existed. use existing index
					}else{
						db->position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i4 = pb_vertex_index++;
					}
					
					db->mesh.insert_polygon(data_exchange::vi4(i4, i3, i2, i1));		
					break;
				default:
					std::cout << prim->getVertexCount();
					break;
			}
		}
	}
	return true;
}


bool
HPI_Trimesh::setToObject(SIM_Object *object, std::vector<data_exchange::vf3> *simulated_points){
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
		geometry_copy->releaseGeometry(); /// Store the integrated simulation state in geometry_copy
		return true;
	}
	std::cout << "Unable to get GeometryCopy for object: " << object->getObjectId() << " to update!" <<std::endl;
	return false;	
}
