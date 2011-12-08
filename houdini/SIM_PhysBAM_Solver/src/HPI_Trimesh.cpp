#include "HPI_Trimesh.h"

HPI_TriMesh::HPI_TriMesh() : points(0) {
}

~HPI_TriMesh::HPI_TriMesh() {
}

bool
HPI_TriMesh::setFromObject(SIM_Object *object){
	const GU_Detail*				const gdp(geometry->getGeometry().readLock());
	const GEO_Primitive*			prim;
	
	FOR_ALL_PRIMITIVES(gdp, prim)
	{
		if(prim->getPrimitiveId() == GEOPRIMPOLY){
			const GEO_Vertex*	v;
			GEO_Point*	p;
			pair<std::map<GEO_Point*, int>::iterator,bool> ret;
			int i1, i2, i3, i4;
			switch(prim->getVertexCount()){
				case 3:
					v = &prim->getVertex(0);
					p = v->getPt();
					ret = trimesh->points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i1 = ret.first->second; // this point already existed. use existing index
					}else{
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i1 = pb_vertex_index++;
					}	
					
					v = &prim->getVertex(1);
					p = v->getPt();
					ret = trimesh->points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i2 = ret.first->second; // this point already existed. use existing index
					}else{
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i2 = pb_vertex_index++;
					}

					v = &prim->getVertex(2);
					p = v->getPt();
					ret = trimesh->points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i3 = ret.first->second; // this point already existed. use existing index
					}else{
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i3 = pb_vertex_index++;
					}
					db.mesh.insert_polygon(data_exchange::vi3(i3, i2, i1)); //std::cout << "indexes3:" << i1 << " " << i2 << " " << i3 << std::endl;
					break;
				case 4:
					v = &prim->getVertex(0);
					p = v->getPt();
					ret = trimesh->points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i1 = ret.first->second; // this point already existed. use existing index
					}else{
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i1 = pb_vertex_index++;
					}	
					
					v = &prim->getVertex(1);
					p = v->getPt();
					ret = trimesh->points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i2 = ret.first->second; // this point already existed. use existing index
					}else{
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i2 = pb_vertex_index++;
					}

					v = &prim->getVertex(2);
					p = v->getPt();
					ret = trimesh->points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i3 = ret.first->second; // this point already existed. use existing index
					}else{
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i3 = pb_vertex_index++;
					}

					v = &prim->getVertex(3);
					p = v->getPt();
					ret = trimesh->points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						i4 = ret.first->second; // this point already existed. use existing index
					}else{
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2])); // new point. we need to store in in def body
						i4 = pb_vertex_index++;
					}					
					db.mesh.insert_polygon(data_exchange::vi4(i4, i3, i2, i1)); //std::cout << "indexes4:" << i1 << " " << i2 << " " << i3 << " " << i4 << std::endl;			
					break;
				default:
					std::cout << prim->getVertexCount();
					break;
			}
		}
	}
}


bool
HPI_TriMesh::setToObject(SIM_Object *object){
	/// Get the object's last state before this time step
	geometry_copy = SIM_DATA_CREATE(*object, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy, SIM_DATA_RETURN_EXISTING | SIM_DATA_ADOPT_EXISTING_ON_DELETE);	

	if(geometry_copy){
		GU_DetailHandleAutoWriteLock	gdl(geometry_copy->lockGeometry());
		GU_Detail           			&gdp = *gdl.getGdp();
		GEO_Point           			*geopt;
		
		std::cout << "pointer:" << pb_object << " readed for object: " << object->getObjectId() << std::endl;
		HPI_TriMesh		*trimesh = worlddata->getSolidObject(object->getObjectId())->getTrimesh();
		if(!trimesh){
			std::cout << "Unable to get trimesh for object: " << object->getObjectId() << std::endl;
			return false;
		}

		std::map<GEO_Point*, int>::iterator iter;
		for (iter = trimesh->points.begin(); iter != trimesh->points.end(); iter++) {
			GEO_Point *pt = iter->first;
			int			i = iter->second;
			pt->setPos(x_array[i].data[0], x_array[i].data[1], x_array[i].data[2], 1);
		}
		geometry_copy->releaseGeometry(); /// Store the integrated simulation state in geometry_copy
	}else{
		std::cout << "Unable to get GeometryCopy for object: " << object->getObjectId() << " to update!" <<std::endl;
	}	
}
