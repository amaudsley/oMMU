#pragma once
#include "../oMMU API/oMMU_Data.h"
#include "stringhasher.h"
#include "orbitersdk.h"
#include <string>
#include <vector>
#include <map>

using namespace oMMU_API;

struct AirlockExt : public Airlock{
	void SetID(int _ID) { ID = _ID; }
};

class oMMUCore {
public:
	VESSEL * pParentVessel;
	oMMUCore(VESSEL* hVessel);
	~oMMUCore();

	//std::vector<oMMUCrew> mCrew;
	std::map<int, oMMUCrew> mCrew;
	std::map<int,AirlockExt> mAirlocks;
	
	int mCrewLimit = 0;
	int mActiveAirlock = 0;

	/*** Interface Implementaiton ***/
	/*--- API VERSION 1.0 - DO NOT RE-ORDER ---*/

	virtual oMMUStatus SetCrewLimit(int crewLimit);

	virtual oMMUStatus ProcessClbkPreStep();

	/* Airlock Management */
	virtual int CreateAirlockFromPort(int portID, bool openByDefault);

	virtual int AddAirlock(const Airlock & airlock);

	virtual oMMUStatus SetAirlockState(const Airlock &airlockState);
	virtual oMMUStatus GetAirlockState(int airlockID, Airlock &airlockOut);

	virtual std::map<int,AirlockExt> GetAirlocks();


	/* Crew Management */
	//virtual oMMUStatus GetCrewByIndex(int index, oMMUCrew &crew);
	//virtual oMMUStatus SetCrewByIndex(int index, const oMMUCrew &crew);

	virtual oMMUStatus AddCrew(const oMMUCrew &crewToAdd, int slot = 0);
	virtual oMMUStatus RemoveCrew(int index);

	virtual oMMUStatus GetCrewState(int index, oMMUCrew& crew);

	virtual oMMUStatus SetCrewState(int index, const oMMUCrew& crew);

	virtual oMMUStatus BeginEVA(int crewIndex, int airlockIndex = 0, bool setOnGround = false, bool setFocus = false);

	virtual oMMUStatus TransferCrew(int crewIndex, int portIndex = 0);
	
	virtual oMMUStatus TransferCrewDirect(int crewIndex, const VESSEL* targetVessel);

	virtual int AddDefaultCrew();

	virtual bool RecallState(const char * line);

	virtual void SaveState(FILEHANDLE scn);

	virtual oMMUStatus TryIngress(const VESSEL *hMMU, double *ret);

	virtual int AddInteractionArea(const InteractionArea& area);
	
	virtual oMMUStatus GetInteractionArea(int areaID, InteractionArea& out);
	
	virtual oMMUStatus SetInteractionAreaState(int areaID, const InteractionArea& in);
	
	//virtual oMMUStatus RegisterInteractionHandler(OnInteractionTriggered handler);

	/*--- END API VERSION 1.0 ---*/
	/* Private API */
	oMMUStatus TryTransfer(const VESSEL * pTargetVessel, int crewIndex);
	oMMUCrew *mGetCrewByIndex(int crewIndex);

private:
	int m_mmuCompatIndex;
};

typedef oMMUCore* (__cdecl *ioMMUFactory)(VESSEL *hVessel);

extern "C" __declspec(dllexport) oMMUCore* __cdecl GetoMMUInstance(VESSEL *hVessel);

extern "C" __declspec(dllexport) oMMUCore* __cdecl GetClosestMMUCompatibleVessel(const VESSEL *hVessel);

