#pragma once
#include <Orbitersdk.h>
#include "../oMMU API/oMMU_Data.h"
#include "../oMMU API/oMMU_API.h"
/* Defines a simplistic interface for oMMU Core */
class IOMMU {
public:
	virtual void destroy() = 0;
	virtual bool AddMMUVessel(VESSEL *obj, OMMUManagement *mgr) = 0;
	virtual bool RemoveMMUVessel(VESSEL *obj) = 0;
	virtual int GetVersion() = 0;
	virtual OBJHANDLE CreateMMU(VESSEL *parent,oMMUCrew *crew) = 0; // Returns handle to the created MMU, or null if the MMU was unable to be created.
};

/*








*/