#ifndef __HPI_Baked_Object_h__
#define __HPI_Baked_Object_h__

#include <SIM/SIM_EmptyData.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_Geometry.h>
#include <SIMZ/SIM_SopGeometry.h>
#include <SIM/SIM_Constraint.h>
#include <SIM/SIM_ConstraintIterator.h>
#include <SIM/SIM_DataFilter.h>
#include <SIM/SIM_ConAnchorSpatial.h>
#include <SIM/SIM_ConAnchorRotational.h>
#include <SIM/SIM_ConRel.h>
#include <SIM/SIM_Position.h>
#include "HPI_Object.h"
#include "HPI_Trimesh.h"

class HPI_Baked_Object : public HPI_Object
{
	public:
		HPI_Baked_Object();
		~HPI_Baked_Object();
	
		virtual	bool	setFromObject(SIM_Object *object, physbam_simulation *sim);
		virtual bool	updateSimulatedObject(SIM_Object *object, physbam_simulation *sim, const SIM_Time &timestep);
		virtual void	appendKeyframe(const SIM_Object *object, physbam_simulation *sim, const SIM_Time &time);
		HPI_Trimesh*	getTrimesh();
		
	private:
		HPI_Trimesh*	trimesh;
};

#endif
