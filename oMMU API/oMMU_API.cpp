#pragma once
#include "oMMU_Bridge.h"
namespace oMMU_API {

	oMMU* oMMU::GetInstance(VESSEL4* hVessel)
	{
		oMMUBridge* ommuInstance = new oMMUBridge(hVessel);

		ommuInstance->Initialize();

		return ommuInstance;
	}
	
}