#include "GTAObject.h"


GTAObject getGTAObjectFromObjName(std::string aObjName) {
	GTAObject gtaObject = {};
	gtaObject.objName = strdup(aObjName.c_str());
	gtaObject.objHash = -1;
	gtaObject.title = aObjName;
	return gtaObject;
}