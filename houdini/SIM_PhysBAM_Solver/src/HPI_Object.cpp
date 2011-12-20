#include "HPI_Object.h"

HPI_Object::HPI_Object(){
	uid = 0;
	pb_object = NULL;
}

HPI_Object::~HPI_Object(){

}

physbam_object*
HPI_Object::getPhysbamObject(){
	return pb_object;
}

int
HPI_Object::getUid(){
	return uid;
}
