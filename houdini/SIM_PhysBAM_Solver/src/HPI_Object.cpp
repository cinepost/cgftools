#include "HPI_Object.h"

HPI_Object::HPI_Object() : pb_object(0), uid(0) {

}

physbam_object*
HPI_Object::getPhysbamObject(){
	return pb_object;
}

int
HPI_Object::getUid(){
	return uid;
}
