#include "SIM_pbSolver.h"
#include "SIM_pbDefGeometry.h"
#include "SIM_pbDefVisualize.h"
#include "SIM_pbWorldData.h"

#define		DEBUG
#define 	PHYSLIB_FILENAME	"libPhysBAM_Wrapper.so"

extern interface_routines ir;

bool   PB_LIB_READY	= false;	

static PRM_Name		theSimeFileName("simfilename", "Simulation file");
static PRM_Default	theSimeFileNameDefault(0, "$HIH/projects/simfile.$SF4.pbs");

void
initializeSIM(void *){
	srand((unsigned)time(0));
	IMPLEMENT_DATAFACTORY(SIM_pbSolver);
	IMPLEMENT_DATAFACTORY(SIM_pbDefGeometry);
	IMPLEMENT_DATAFACTORY(SIM_pbDefVisualize);
    IMPLEMENT_DATAFACTORY(SIM_PhysBAM_WorldData);
    
    logutils_indentation = 0;
    data_exchange::register_ids();
    void* handle = dlopen(PHYSLIB_FILENAME, RTLD_LAZY);
    if(!handle){
        const char *p = dlerror();
        printf("dlopen error %s. Will bypass actual simulation !!!\n", p);
    }else{
		if(!ir.init(handle))
		{
			printf("Failed to initialize interface routines.\n");
			exit(1);
		}
		
		PB_LIB_READY = true;
		
		ir.physbam_init();
		//ir.set_string(0, ir.get_id(0, "playback_log"), "main.log");
		ir.set_int(0, ir.get_id(0, "enable_cout_logging"), 1);
	}
	std::cout << "initializeSIM called." << std::endl;
};
	
SIM_pbSolver::SIM_pbSolver(const SIM_DataFactory *factory) : BaseClass(factory), SIM_OptionsUser(this){
	
};

SIM_pbSolver::~SIM_pbSolver(){
	
};

const SIM_DopDescription* SIM_pbSolver::getSolverDopDescription(){
    static PRM_Template  theTemplates[] = {
        PRM_Template(PRM_FILE,         1, &theSimeFileName, &theSimeFileNameDefault),
        PRM_Template()
    };

    static SIM_DopDescription    theDopDescription(true, "pbm_def_solver", "PhysBAM Deformable Solver", SIM_SOLVER_DATANAME, classname(), theTemplates);
    return &theDopDescription;
};	

bool SIM_pbSolver::setupNewSimObject(physbam_simulation* sim, SIM_Object* object){
	std::cout << "\t" << object->getName() << " id: " << object->getObjectId();
	SIM_pbDefGeometry* defgeo;
	
	defgeo = SIM_DATA_CAST(object->getNamedSubData("PhysBAM_Geometry"), SIM_pbDefGeometry);
    if (!defgeo)
    {
		std::cout << " not a physbam deformable geo. skipping." << std::endl;
		return true;
    }else{
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
		
		const GU_Detail*				const gdp(defgeo->getGeometry().readLock());
		const UT_Vector4F*				pt_pos;
		const GEO_Primitive*			prim;
		data_exchange::deformable_body 	db;
		
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
		
		/*
		db.position.push_back(data_exchange::vf3(0,0,0));
		db.position.push_back(data_exchange::vf3(0,0,1));
		db.position.push_back(data_exchange::vf3(0,1,0));
		db.position.push_back(data_exchange::vf3(0,1,1));
		db.position.push_back(data_exchange::vf3(1,0,0));
		db.position.push_back(data_exchange::vf3(1,0,1));
		db.position.push_back(data_exchange::vf3(1,1,0));
		db.position.push_back(data_exchange::vf3(1,1,1));
		db.mesh.insert_polygon(data_exchange::vi4(7,6,2,3));
		db.mesh.insert_polygon(data_exchange::vi4(2,6,4,0));
		db.mesh.insert_polygon(data_exchange::vi4(1,0,4,5));
		db.mesh.insert_polygon(data_exchange::vi4(0,1,3,2));
		db.mesh.insert_polygon(data_exchange::vi4(3,1,5,7));
		db.mesh.insert_polygon(data_exchange::vi4(4,6,7,5));
		*/
		
		physbam_object* pb_object;
		pb_object = ir.add_object(sim, &db);
		if(pb_object){
			//defgeo->getMesh() = &trimesh;
			
			worlddata->getObjects()->insert( std::pair<int, physbam_object*>(object->getObjectId(), pb_object));
			std::cout << "pointer:" << pb_object << " stored for object: " << object->getObjectId() << std::endl;
			return true;
		}else{
			printf("Unable to add new object %d into simulation !!!\n", object->getObjectId());
			return false;
		}
	}
}
bool SIM_pbSolver::updateSimObject(physbam_simulation* sim, SIM_Object* object){
	std::cout << "\t" << object->getName() << " id: " << object->getObjectId() << std::endl;
	SIM_pbDefGeometry* defgeo;
	
	defgeo = SIM_DATA_CAST(object->getNamedSubData("PhysBAM_Geometry"), SIM_pbDefGeometry);
	if(defgeo){
		// Get the object's last state before this time step
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
	}
	return true;
}
	
SIM_Solver::SIM_Result
SIM_pbSolver::solveObjectsSubclass ( SIM_Engine &engine, SIM_ObjectArray &objects, SIM_ObjectArray &newobjects, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep){
	LOG_INDENT;
	LOG("SIM_pbSolver solveObjectsSubclass() called.");		
	
	if(PB_LIB_READY){
		bool new_world = false;
		if(!worlddata){
			new_world = true;
			worlddata = SIM_DATA_CREATE(*this, "PhysBAM_World", SIM_PhysBAM_WorldData, SIM_DATA_RETURN_EXISTING);
			if(!worlddata){
				LOG("SIM_pbSolver solveObjectsSubclass() unable to get PhysBAM_World data.");
				LOG_UNDENT;
				return SIM_Solver::SIM_SOLVER_FAIL;
			}
		}
		
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
		
		sim = worlddata->getSimulation();
		
		/*
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
		
		}*/
		if (new_world) {
			// setup basic forces and ground
			LOG("SIM_pbSolver solveObjectsSubclass() setting up simple ground:");
			data_exchange::ground_plane gp;
			gp.position = data_exchange::vf3(0,-10,0);
			gp.normal = data_exchange::vf3(0,1,0);
			ir.add_object(sim, &gp);
		}
		
		// Loop through new objects and add them into sim.
		if (newobjects.entries() > 0) {
			LOG("SIM_pbSolver solveObjectsSubclass() setting up new objects in sim:");	
			for( i = 0; i < newobjects.entries(); i++ ){	
		    	if(!setupNewSimObject(sim, newobjects(i))){
					return SIM_Solver::SIM_SOLVER_FAIL;
				}
			}	
		}
	
		// Run the simulation for the given time_step
		ir.simulate_frame(sim, 1.0f);
	
		// Update all the objects
		// Loop through all the objects.
		if (objects.entries() > 0){
			LOG("SIM_pbSolver solveObjectsSubclass() updating objects in sim:");
			for( i = 0; i < objects.entries(); i++ ){ 
		    	if(!updateSimObject(sim, objects(i))){
					return SIM_Solver::SIM_SOLVER_FAIL;
				}  
			}	
		}
		LOG("Done");
		LOG_UNDENT;			
		return SIM_Solver::SIM_SOLVER_SUCCESS;
	}else{
		std::cerr << PHYSLIB_FILENAME << " not loaded!!!"<< std::endl;
		return SIM_Solver::SIM_SOLVER_FAIL;	
	}
}	
