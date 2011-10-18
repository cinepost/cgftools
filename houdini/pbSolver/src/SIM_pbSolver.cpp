#include "SIM_pbSolver.h"

#define		DEBUG
#define 	PHYSLIB_FILENAME	"libPhysBAM_Wrapper.so"

bool   PB_LIB_READY	= false;	

std::map<int, physbam_simulation*>	SIM_pbSolver::simulations; 
std::map<int, physbam_object*>		SIM_pbSolver::objects;
std::map<int, HPI_TriMesh>			SIM_pbSolver::meshes;	

static PRM_Name		theSimeFileName("simfilename", "Simulation file");
static PRM_Default	theSimeFileNameDefault(0, "$HIH/projects/simfile.$SF4.pbs");

void
initializeSIM(void *){
	srand((unsigned)time(0));
    IMPLEMENT_DATAFACTORY(SIM_pbSolver);
    
    void* handle = dlopen(PHYSLIB_FILENAME, RTLD_LAZY);
    if(!handle){
        const char *p = dlerror();
        printf("dlopen error %s. Will bypass actual simulation !!!\n", p);
    }else{
		hpi_create_simulation 		= (physbam_simulation* (*)()) dlsym(handle, "create_simulation");
		hpi_destroy_simulation		= (bool (*)(physbam_simulation*)) dlsym(handle, "destroy_simulation");
		hpi_add_object 				= (physbam_object* (*)(physbam_simulation*, const data_exchange::simulation_object*)) dlsym(handle, "add_object");
		hpi_add_force				= (physbam_force* (*)(physbam_simulation*, const data_exchange::force*)) dlsym(handle, "add_force");
		hpi_apply_force_to_object	= (bool (*)(physbam_object*, physbam_force*)) dlsym(handle, "apply_force_to_object");
		hpi_simulate_frame			= (bool (*)(physbam_simulation*)) dlsym(handle, "simulate_frame");
		hpi_get_id					= (int (*)(physbam_base * obj, const char * attribute)) dlsym(handle, "get_id");
		hpi_set_int 				= (void (*)(physbam_base * obj, int id, int x)) dlsym(handle, "set_int");
		hpi_get_int 				= (int (*)(const physbam_base * obj, int id)) dlsym(handle, "get_int");
		hpi_set_float				= (void (*)(physbam_base * obj, int id, float x)) dlsym(handle, "set_float");
		hpi_get_float 				= (float (*)(const physbam_base * obj, int id)) dlsym(handle, "get_float");
		hpi_get_vf3_array			= (void (*)(const physbam_base * obj, int id, data_exchange::vf3 * x, int length, int start)) dlsym(handle, "get_vf3_array");
		
		hpi_enable_logging			= (void (*)(const char* file)) dlsym(handle, "enable_logging");
		hpi_finish_logging			= (void (*)()) dlsym(handle, "finish_logging");
		
		data_exchange::register_ids();
		PB_LIB_READY 				= true;
		
		printf("%p %p %p %p %p %p\n", hpi_create_simulation, hpi_destroy_simulation, hpi_add_object, hpi_add_force, hpi_apply_force_to_object, hpi_simulate_frame);
	}
	std::cout << "initializeSIM called." << std::endl;
}
	
SIM_pbSolver::SIM_pbSolver(const SIM_DataFactory *factory) : BaseClass(factory), SIM_OptionsUser(this){
	
}

SIM_pbSolver::~SIM_pbSolver(){
	
}

const SIM_DopDescription* SIM_pbSolver::getSolverDopDescription(){
    static PRM_Template  theTemplates[] = {
        PRM_Template(PRM_FILE,         1, &theSimeFileName, &theSimeFileNameDefault),
        PRM_Template()
    };

    static SIM_DopDescription    theDopDescription(true, "pbm_def_solver", "PhysBAM Deformable Solver", SIM_SOLVER_DATANAME, classname(), theTemplates);
    return &theDopDescription;
}	

bool SIM_pbSolver::setupNewSimObject(physbam_simulation* sim, SIM_Object* object){
	std::cout << "\t" << object->getName() << " id: " << object->getObjectId();
	const SIM_Data	*force;
	SIM_ConstDataArray forces;
	object->filterConstSubData(
		forces, 0,
		SIM_DataFilterByType("SIM_Force"),
		SIM_FORCES_DATANAME,
		SIM_DataFilterNone()
    );
    if(!forces.isEmpty()){
		std::cout << "Affected by: ";
		for(int i=0; i < forces.entries(); i++){
			force = forces[i];
			std::cout << force->getDataType();
		}
	}
	std::cout << std::endl;
	//----------------------------------
	
	const SIM_Geometry* 			const geometry(object->getGeometry());
	const GU_Detail*				const gdp(geometry->getGeometry().readLock());
	const UT_Vector4F*				pt_pos;
	const GEO_Primitive*			prim;
	data_exchange::deformable_body 	db;
	
	/*
	const GEO_Point*		point;
	FOR_ALL_GPOINTS(gdp, point)
	{
		pt_pos	= &geopt->getPos();
		db.position.push_back(data_exchange::vf3(0, 1, 2));
	}
	*/
	int	pb_vertex_index = 0;
	HPI_TriMesh	trimesh;

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
					ret = trimesh.points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						// this point already existed. use existing index
						i1 = ret.first->second;
					}else{
						// new point. we need to store in in def body
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2]));
						i1 = pb_vertex_index++;
					}	
					
					v = &prim->getVertex(1);
					p = v->getPt();
					ret = trimesh.points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						// this point already existed. use existing index
						i2 = ret.first->second;
					}else{
						// new point. we need to store in in def body
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2]));
						i2 = pb_vertex_index++;
					}

					v = &prim->getVertex(2);
					p = v->getPt();
					ret = trimesh.points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						// this point already existed. use existing index
						i3 = ret.first->second;
					}else{
						// new point. we need to store in in def body
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2]));
						i3 = pb_vertex_index++;
					}
					
					
					std::cout << "indexes3:" << i1 << " " << i2 << " " << i3 << std::endl;
					db.mesh.insert_polygon(data_exchange::vi3(i3, i2, i1));
					break;
				case 4:
					v = &prim->getVertex(0);
					p = v->getPt();
					ret = trimesh.points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						// this point already existed. use existing index
						i1 = ret.first->second;
					}else{
						// new point. we need to store in in def body
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2]));
						i1 = pb_vertex_index++;
					}	
					
					v = &prim->getVertex(1);
					p = v->getPt();
					ret = trimesh.points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						// this point already existed. use existing index
						i2 = ret.first->second;
					}else{
						// new point. we need to store in in def body
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2]));
						i2 = pb_vertex_index++;
					}

					v = &prim->getVertex(2);
					p = v->getPt();
					ret = trimesh.points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						// this point already existed. use existing index
						i3 = ret.first->second;
					}else{
						// new point. we need to store in in def body
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2]));
						i3 = pb_vertex_index++;
					}

					v = &prim->getVertex(3);
					p = v->getPt();
					ret = trimesh.points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						// this point already existed. use existing index
						i4 = ret.first->second;
					}else{
						// new point. we need to store in in def body
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2]));
						i4 = pb_vertex_index++;
					}					
					
					std::cout << "indexes4:" << i1 << " " << i2 << " " << i3 << " " << i4 << std::endl;
					
					db.mesh.insert_polygon(data_exchange::vi4(i4, i3, i2, i1));				
					break;
				default:
					std::cout << prim->getVertexCount();
					break;
			}
		}
	}

	physbam_object* pb_object = hpi_add_object(sim, &db);
	if(pb_object){
		meshes[object->getObjectId()] = trimesh;
		
		objects[object->getObjectId()] = pb_object;
		std::cout << "pointer:" << pb_object << " stored for object: " << object->getObjectId() << std::endl;
		return true;
	}else{
		printf("Unable to add new object %d into simulation !!!\n", object->getObjectId());
		return false;
	}
}
bool SIM_pbSolver::updateSimObject(physbam_simulation* sim, SIM_Object* object){
	std::cout << "\t" << object->getName() << " id: " << object->getObjectId() << std::endl;
	
	// Get the object's last state before this time step
	const SIM_Geometry* const geometry(object->getGeometry());
	
	// Extract simulation state from geome
	RBD_State *rbdstate = SIM_DATA_GET(*object, "Position", RBD_State);

	
	// Integrate simulation state forward by time step using the solver parameters, e.g., getMyOwnAccuracy()
	std::cout << "\t\tpos: " << rbdstate->getPosition() << std::endl;
	std::cout << "\t\trot: " << rbdstate->getOrientation() << std::endl;
	std::cout << "\t\tpvt: " << rbdstate->getPivot() << std::endl;
	
	SIM_GeometryCopy* geometry_copy(SIM_DATA_CREATE(*object, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy, SIM_DATA_RETURN_EXISTING | SIM_DATA_ADOPT_EXISTING_ON_DELETE));	
	
	GU_DetailHandleAutoWriteLock	gdl(geometry_copy->lockGeometry());
	GU_Detail           			&gdp = *gdl.getGdp();
	GEO_Point           			*geopt;
	
	/* translate in case in rigid body object
	gdp.translate( UT_Vector3(1,0,0), 0, 0, 0.1, 0, 0); 	
	*/
	
	//physbam_object* pb_object = objects[object->getObjectId()];
	//std::cout << "pointer:" << pb_object << " readed for object: " << object->getObjectId() << std::endl;
	//int pb_attr_id = hpi_get_id(pb_object, "position");
	
	
	// Integrate our velocities.
	/*
	FOR_ALL_GPOINTS((&gdp), geopt)
	{
		geopt->setPos();
	}
	*/
	
	// Store the integrated simulation state in geometry_copy
	geometry_copy->releaseGeometry();
	
	return true;
}
	
SIM_Solver::SIM_Result
SIM_pbSolver::solveObjectsSubclass ( SIM_Engine &engine, SIM_ObjectArray &objects, SIM_ObjectArray &newobjects, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep){
	if(PB_LIB_READY){
		OP_Node*				obj_parent;
		const SIM_Object*		obj;
		const SIM_Geometry*		sourcegeo;
		SIM_ColliderInfoArray	colliderinfo;  
		SIM_ObjectArray			filtered;
		SIM_Time				sim_time;
		UT_String				group, simfilename;
		physbam_simulation* 	sim;
		int						i;
		const SIM_Time 			curr_time = engine.getSimulationTime();
		
		getStartTime(sim_time);
		if( sim_time == curr_time) {
			// This is a simulation start point, so we need to set-up simulation first
			sim = pbSim(true);
		}else{
			// This is a simulation step, we need to get existing sim here
			sim = pbSim(false);
		}
		
		for(int s = 0; s < engine.getNumSimulationObjects(); s++){
			obj = engine.getSimulationObject(s);
			sourcegeo = obj->getGeometry();
			
			std::cout << "engine: " << obj->getName() << " num subd: " << obj->getNumSubData() << " static: " << obj->getIsStatic();
			if(!sourcegeo || sourcegeo->getGeometry().isNull()){
				std::cout << " no geo!" << std::endl;
			}else{
				std::cout << " geo exist: " << sourcegeo->getGeometry().readLock()->primitives().entries() << std::endl;
			}
			
			for(int d = 0; d < obj->getNumSubData(); d++){
				const char * data_name = obj->getSubDataName(d);
				std::cout << "data name: " << data_name << std::endl;
			}
			//std::cout << "engine: " << engine.getSimulationObject(s)->getName() << " of subclass type: " << engine.getSimulationObject(s)->getDataTypeSubclass() << std::endl;
		
		}
		
		// Loop through new objects and add them into sim.
		if (newobjects.entries() > 0) {
			std::cout << "Setting up new objects in sim:" << std::endl; 
			for( i = 0; i < newobjects.entries(); i++ ){	
		    	if(!setupNewSimObject(sim, newobjects(i))){
					return SIM_Solver::SIM_SOLVER_FAIL;
				}
			}	
		}
	
		// Run the simulation for the given time_step
		//hpi_simulate_frame(sim);
	
		// Update all the objects
		// Loop through all the objects.
		if (objects.entries() > 0){
			std::cout << "Updating objects in sim:" << std::endl; 
			for( i = 0; i < objects.entries(); i++ ){ 
		    	if(!updateSimObject(sim, objects(i))){
					return SIM_Solver::SIM_SOLVER_FAIL;
				}  
			}	
		}
			
		return SIM_Solver::SIM_SOLVER_SUCCESS;
	}else{
		std::cerr << PHYSLIB_FILENAME << " not loaded!!!"<< std::endl;
		return SIM_Solver::SIM_SOLVER_FAIL;	
	}
}


physbam_simulation*		SIM_pbSolver::pbSim(bool reset){
	int uid = getCreatorNode()->getUniqueId();
	if(simulations.find(uid) == simulations.end()){
		// simulation for this solver not found. create a new one anyway
		std::cout << "New PhysBAM simulation instace created for solver: " << uid << std::endl;
		simulations[uid] = hpi_create_simulation();
		printf("sim %p\n", simulations[uid]);
	}else{
		// simulation exists
		if(reset){
			std::cout << "Resetting simulation instance for solver: " << uid << std::endl;
			hpi_destroy_simulation(simulations[uid]);
			simulations[uid] = hpi_create_simulation();
		}
		std::cout << "Using existing simulation instance for solver: " << uid << std::endl;
	}
	return simulations[uid];
}	

