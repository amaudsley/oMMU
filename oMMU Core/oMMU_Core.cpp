#include "../oMMU MMU/oMMU_MMU.h"
#include "oMMU_Core.h"
#include <atlstr.h>  
#include "../oMMU API/oMMU_API.h"
using namespace oMMU_API;
#define SCENARIO_READ_BUFFER_LENGTH 2048

static std::vector<oMMUCore*> m_mmuCompatVessels;


oMMUCore::oMMUCore(VESSEL* hVessel)
{

#if DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |
		_CRTDBG_CHECK_CRT_DF |
		_CRTDBG_LEAK_CHECK_DF);
#endif // DEBUG

	pParentVessel = hVessel;
	m_mmuCompatVessels.push_back(this);
	m_mmuCompatIndex = m_mmuCompatVessels.size() - 1;
}

// TODO : Add proper cleanup
oMMUCore::~oMMUCore() {
	m_mmuCompatVessels.erase(m_mmuCompatVessels.begin() + m_mmuCompatIndex);
}

// TODO : Implement ProcessClbkPreStep
// TODO : Comment ProcessClbkPreStep
// TODO : Surely this should take some parameters (e.g, Time, fix it!)
oMMUStatus oMMUCore::ProcessClbkPreStep()
{
	return oMMUStatus::OK;
}

/* Creates an airlock based on a pre-existing docking port, utility func to save some sanity */
int oMMUCore::CreateAirlockFromPort(int portID, bool openByDefault)
{
	VECTOR3 pos, dir, rot;
	DOCKHANDLE hDock = pParentVessel->GetDockHandle(portID);
	if (hDock == nullptr)
		return -1;
	pParentVessel->GetDockParams(hDock, pos, dir, rot);

	AirlockExt newAirlock;
	newAirlock.radius = 10.0f;
	newAirlock.type = AirlockType::dockingPort;
	newAirlock.position = pos;
	newAirlock.direction = dir;
	newAirlock.rotation = rot;
	newAirlock.associatedDockingPort = portID;
	newAirlock.SetID(portID);
	return AddAirlock(newAirlock);
}

// TODO : Implement AddDefaultCrew()
// TODO : Comment AddDefaultCrew
int oMMUCore::AddDefaultCrew()
{
	return 0;
}


/* Registers an airlock for this vessel - multiple airlocks are allowed (duh), it'd be insane to do anything else*/
int oMMUCore::AddAirlock(const Airlock& airlock)
{
	int nextIndex = 0;
	if (mAirlocks.size() > 0)
		nextIndex = mAirlocks.rbegin()->first + 1;

	mAirlocks[nextIndex] = static_cast<const AirlockExt&>(airlock);
	mAirlocks[nextIndex].SetID(nextIndex);
	return airlock.GetID(); // Return the index of the airlock created. 
}

// TODO : Comment SetAirlockState
oMMUStatus oMMUCore::SetAirlockState(const Airlock& airlockState)
{
	int airlockID = airlockState.GetID();
	for (size_t i = 0; i < mAirlocks.size(); i++)
	{
		/* Perform an uipdate if ID matches */
		if (mAirlocks[i].GetID() == airlockID)
		{
			mAirlocks[i].isOpen = airlockState.isOpen;
			mAirlocks[i].type = airlockState.type;
			mAirlocks[i].position = airlockState.position;
			mAirlocks[i].radius = airlockState.radius;
			return oMMUStatus::OK;
		}
	}
	return oMMUStatus::AirlockStateChangeFailed;
}

// TODO : COmment GetAirlockState
oMMUStatus oMMUCore::GetAirlockState(int airlockID, Airlock& airlockOut)
{
	if (mAirlocks.count(airlockID) == 1) {
		airlockOut = mAirlocks[airlockID];
		return oMMUStatus::OK;
	}
	return oMMUStatus::AirlockNotFound;
}

std::map<int, AirlockExt> oMMUCore::GetAirlocks()
{
	return mAirlocks;
}

// TODO : Comment GetCrewByIndex
oMMUStatus oMMUCore::GetCrewState(int index, oMMUCrew& crew)
{
	if (mCrew.count(index)) {
		crew = mCrew[index];
		return oMMUStatus::OK;
	}
	else
		return oMMUStatus::Failure;
}

oMMUStatus oMMUCore::SetCrewState(int index, const oMMUCrew& crew)
{
	if (mCrew.count(index) == 1) {
		mCrew[index] = crew;
		return oMMUStatus::OK;
		// TODO : This function should allow creating new mmus? maybe? who knows any more.
	}
	return oMMUStatus::Failure;
}


/**
* \brief When called this function will attempt to EVA or transfer the crew member at the given index.
* \If the airlock at airlockIndex is a docking-port based airlock and the vessel is docked only transfers
* \will be possible. If the ability to EVA in in this case is required a second airlock should be added using the appropiate funcitons.
* \param crewIndex The zero-based index of the crew member to EVA or transfer
* \param airlockIndex The index of the airlock to use.
* \return Returns oMMUStatus::EVASucess or oMMUStatus::TransferSucess on sucess.
*/
oMMUStatus oMMUCore::BeginEVA(int crewIndex, int airlockIndex, bool setOnGround, bool setFocus)
{
	// && mAirlocks[airlockIndex]->isOpen
	Airlock airlock = mAirlocks[airlockIndex];

	/* Check the airlock is open - abort if not */
	if (!airlock.isOpen)
		return oMMUStatus::AirlockClosed;

	oMMUCrew crewMember;
	auto retVal = GetCrewState(crewIndex, crewMember);
	/* Check the crew member exists */
	if (retVal == oMMUStatus::OK) {
		/* Handle docking port 'airlocks' - transfer to docked vessels if airlock is open*/
		if (airlock.type == AirlockType::dockingPort) {
			DOCKHANDLE hDock = pParentVessel->GetDockHandle(airlock.GetID());
			if (hDock != nullptr) {
				//TransferCrew(crewIndex, airlock.GetID());

				OBJHANDLE hObj = pParentVessel->GetDockStatus(hDock);
				if (hObj != nullptr) {
					return TryTransfer(oapiGetVesselInterface(hObj), crewIndex);
				}
			}
		}

		/* New MMU status variables */
		VESSELSTATUS2 vesselStatus;
		memset(&vesselStatus, 0, sizeof(vesselStatus));
		vesselStatus.version = 2;
		pParentVessel->GetStatusEx(&vesselStatus);
		CString mmuName;
		mmuName.Format("mmu %s %s", crewMember.role, crewMember.name);
		mmuName.Replace(' ', '_');

		/* Ground EVA behaviour - Place the MMU vessel on the ground at / near the airlock */
		if (pParentVessel->GroundContact()) {
			vesselStatus.status = 1; // Landed

			/* Calculate degrees / meter */
			double planetRadius = oapiGetSize(pParentVessel->GetSurfaceRef());
			double metersPerDegree = (planetRadius * 2 * PI) / 360;

			VECTOR3 rotatedPosition; // Holds the airlock position after being converted to horizon frame
			pParentVessel->HorizonRot(airlock.position, rotatedPosition);

			/* Update the position of the MMU vessel to be at the airlock */
			vesselStatus.surf_lat += (rotatedPosition.z / metersPerDegree) * RAD;
			vesselStatus.surf_lng += (rotatedPosition.x / metersPerDegree) * RAD;

		}
		else {
			pParentVessel->Local2Rel(airlock.position, vesselStatus.rpos); // Convert the airlock position to relative coords
		}

		/* Create MMU Vessel - ensure vessel state changes are made *before* this point */
		auto mmuVessel = oapiCreateVesselEx(mmuName, "oMMU", &vesselStatus);
		oMMU_MMU* newMMU = static_cast<oMMU_MMU*>(oapiGetVesselInterface(mmuVessel)); // Instantiate the new MMU
		newMMU->setMMUData(mCrew[crewIndex]); // Pass crew data along

		RemoveCrew(crewIndex);

		// Set camera / input focus on the new MMU vessel if requested
		if (setFocus) {
			oapiSetFocusObject(mmuVessel);
		}

		return oMMUStatus::EVASucess;

	}
	else
		return oMMUStatus::EVAFailure;

	return oMMUStatus::AirlockClosed;
}

/**
* \brief When called this function will attempt to transfer a crew member to the vessel docked
* at the given port.
* On success the crew member will be removed from the calling vessel and added to the vessel at the
* targetted docking port.
* \param crewIndex The zero-based index of the crew member to EVA or transfer
* \param portIndex The index of the airlock to use.
* \return Returns oMMUStatus::TransferSucess on success. Returns oMMUStatus::TransferFailure on failure (i.e., if the target is full
* or is not oMMU compatible)
* TODO : More granular return types.
*/
oMMUStatus oMMUCore::TransferCrew(int crewIndex, int portIndex)
{
	DOCKHANDLE hDock = pParentVessel->GetDockHandle(portIndex);
	if (hDock != nullptr) {

		OBJHANDLE hObj = pParentVessel->GetDockStatus(hDock);
		if (hObj != nullptr) {
			return TryTransfer(oapiGetVesselInterface(hObj), crewIndex);
		}
	}
	else {
		return oMMUStatus::CrewTransferFailure;
	}
}

oMMUStatus oMMUCore::TransferCrewDirect(int crewIndex, const VESSEL* targetVessel)
{
	return TryTransfer(targetVessel, crewIndex);
}

// TODO : Comment AddCrewMember
oMMUStatus oMMUCore::AddCrew(const oMMUCrew& crewToAdd, int slot)
{
	// TODO : Add out of bounds 
	/* Insert at first available slot */
	if (slot == 0) {
		/* Cover the case of having 0 crew members present - use the 0th slot*/
		if (mCrew.size() == 0) {
			mCrew[0] = crewToAdd;
			return oMMUStatus::OK;
		}

		// Insert the crew member in the next available empty slot.
		for (size_t i = 1; i <= mCrewLimit; i++)
		{
			if (mCrew.count(i) == 0) {
				mCrew[i] = crewToAdd;
				return oMMUStatus::OK;
			}
		}

		return oMMUStatus::VesselFull;

	}
	/* Insert at specific slot */
	else if (slot < mCrewLimit) {
		if (mCrew.count(slot) == 0) {
			mCrew[slot] = crewToAdd;
			return oMMUStatus::OK;
		}
		else
			return oMMUStatus::SlotNotEmpty;
	}
	/* slot out of bounds, return apropop status message */
	else {
		return oMMUStatus::SlotOutOfBounds;
	}
}

// TODO : Comment RemoveCrewMemberByID
oMMUStatus oMMUCore::RemoveCrew(int index)
{
	if (mCrew.count(index)) {
		mCrew.erase(index);
		return oMMUStatus::OK;
	}
	return oMMUStatus::Failure;
}

/**
 * \brief Recall oMMU state from the scenario file.
 * \param line
 * \return Returns 'true' if the line was confsumed. False for any other state.
 */
bool oMMUCore::RecallState(const char* line)
{
	/* CREW RANK-NAME-AGE-PULSE-WEIGHT*/
	if (!_strnicmp(line, "CREW", 4)) {
		oMMUCrew crewMember;
		int slot = -1;
		sscanf(line + 4, "%i::%[^::]::%[^::]::%i::%i::%lf::%[^::]::%[^::]", &slot, crewMember.role.GetBuffer(SCENARIO_READ_BUFFER_LENGTH), crewMember.name.GetBuffer(SCENARIO_READ_BUFFER_LENGTH),
			&crewMember.age, &crewMember.pulse, &crewMember.weight, crewMember.evaMesh.GetBuffer(SCENARIO_READ_BUFFER_LENGTH), crewMember.miscData.GetBuffer(SCENARIO_READ_BUFFER_LENGTH));
		/* Release cstring buffers */
		crewMember.name.ReleaseBuffer();
		crewMember.role.ReleaseBuffer();
		crewMember.evaMesh.ReleaseBuffer();
		crewMember.miscData.ReleaseBuffer();
		AddCrew(crewMember, slot);
		return true;
	}
	return false;
}

/**
 * \brief Saves the current oMMU state to the scenario file
 * \param scn
 */
void oMMUCore::SaveState(FILEHANDLE scn)
{
	CString cbuf;
	for (auto& i : mCrew)
	{
		cbuf.Format("%i::%s::%s::%i::%i::%lf::%s::%s", i.first, i.second.role, i.second.name, i.second.age, i.second.pulse, i.second.weight, i.second.evaMesh, i.second.miscData);
		oapiWriteScenario_string(scn, "CREW", cbuf.GetBuffer());
	}
}

// TODO : Comment TryIngress
oMMUStatus oMMUCore::TryIngress(const VESSEL* hMMU, double* ret)
{
	/* Early escape if attempting to enter a vessel without airlocks. */
	if (mAirlocks.empty())
		return oMMUStatus::AirlockClosed;

	/* Distance checking vars */
	const VECTOR3& zero = _V(0, 0, 0);

	VECTOR3 mmuGlobalCoords;
	VECTOR3 airlockGlobalCoords;
	// TODO : Fix this to actually work with multiple airlocks
	/* Special case for vessels with just a single airlock */
	if (mAirlocks.size() == 1) {
		Airlock airlock = mAirlocks[0];
		if (airlock.isOpen) {
			pParentVessel->Local2Global(airlock.position, airlockGlobalCoords);
			hMMU->Local2Global(zero, mmuGlobalCoords);

			/* Simple 'collision' check to see if we're within the airlock catchment area */
			const double distance = dist(airlockGlobalCoords, mmuGlobalCoords);
			if (distance < airlock.radius) {
				auto crewData = *((oMMU_MMU*)hMMU)->getMMUData();
				return this->AddCrew(crewData);
			}
		}
		else
			return oMMUStatus::TargetAirlockClosed;
	}

	return oMMUStatus::Failure;
}

int oMMUCore::AddInteractionArea(const InteractionArea& area)
{
	return 0;
}

oMMUStatus oMMUCore::GetInteractionArea(int areaID, InteractionArea& out)
{
	return oMMUStatus();
}

oMMUStatus oMMUCore::SetInteractionAreaState(int areaID, const InteractionArea& in)
{
	return oMMUStatus();
}

// TODO :: Comment TryTransfer
oMMUStatus oMMUCore::TryTransfer(const VESSEL* pTargetVessel, int crewIndex) {
	/* Search the mmu vessel compatability list - try and find*/
	oMMUCore* otherVesselInterface = nullptr;
	for each (oMMUCore * vessel in m_mmuCompatVessels)
	{
		if (vessel->pParentVessel == pTargetVessel) {
			otherVesselInterface = vessel;
			break;
		}
	}

	/* If we were unable to retrieve a handle we can presume there's no valid vessels */
	if (otherVesselInterface == nullptr)
		return oMMUStatus::CrewTransferFailure;

	/* Try to transfer to the retrieved vessel, invoke the handler if available. */
	bool canAddCrew = true;
	//if (otherVesselInterface->OnTryTransferCrew != nullptr) {
	//	canAddCrew = otherVesselInterface->OnTryTransferCrew(pParentVessel, mCrew[crewIndex], 0);
	//}

	/* Transfer the crew member */
	if (canAddCrew) {
		oMMUCrew* pCrewMember = mGetCrewByIndex(crewIndex); // Retrieve 
		oMMUStatus ingressStatus = otherVesselInterface->AddCrew(*pCrewMember); // Attempt the transfer, store the return value

		/* oMMUStatus is OK, we've transfered!*/
		if (ingressStatus == oMMUStatus::OK) {
			RemoveCrew(crewIndex); // Remove the crew member, as they've been transfered at this point
			return oMMUStatus::CrewTransferSucess;
		}
		else
			return oMMUStatus::CrewTransferFailure;

	}
	return oMMUStatus::CrewTransferFailure; // TODO : Replace with TransferTargetClosed or similar.	
}

// TODO : Comment mGetCrewByIndex
// NOTE : mGetCrewByIndex is an internal helper function. DO NOT EXPOSE.
oMMUCrew* oMMUCore::mGetCrewByIndex(int crewIndex)
{
	if (crewIndex < mCrew.size())
		return &mCrew[crewIndex];
}

// TODO : Comment SetCrewLimit
// TODO : Add a boolean to force size change.
oMMUStatus oMMUCore::SetCrewLimit(int crewLimit)
{
	if (crewLimit > mCrew.size()) {
		mCrewLimit = crewLimit;
		return oMMUStatus::CrewLimitChangeSucess;
	}
	else
		return oMMUStatus::CrewLimitChangeFailure;
}

// TODO : Comment GetoMMUInstance
oMMUCore* GetoMMUInstance(VESSEL* hVessel)
{
	return new oMMUCore(hVessel);
}

// TODO : Comment get_closest_mmu_vessel
// TODO : Pick a less terrible name
// TODO : Optimize
// TODO : Really, just throw this one out and start over.
// TODO : Return airlock distance to caller (for HUD readout)
oMMUCore* GetClosestMMUCompatibleVessel(const VESSEL* hVessel)
{
	const VECTOR3& zero = _V(0, 0, 0);
	/* Coordinate vectors */
	VECTOR3 targetRelCoords;
	VECTOR3 ourRelCoords;
	VECTOR3 airlockRelCoords;

	double bestDistance = 1e3;
	VESSEL* closestVessel = nullptr;
	oMMUCore* mgr = nullptr;
	std::vector<std::pair<oMMUCore*, double>> candidates;

	/* Iterate through all known OMMU vessels and find the closest */
	/* n.b. while this is fine as a general case a more robust implementation
	 * would find the closest actual airlock, this should be changed to do that at some point.
	 */
	for each (oMMUCore * var in m_mmuCompatVessels)
	{
		var->pParentVessel->Local2Rel(zero, targetRelCoords);
		hVessel->Local2Rel(zero, ourRelCoords);

		double distance = dist(ourRelCoords, targetRelCoords);
		/* 1KM is a good distance */
		// TODO : Change to a constant. Magic numbers are the devil's work.
		if (distance < 1e3) {
			closestVessel = var->pParentVessel;
			mgr = var;
			auto airlocks = mgr->GetAirlocks();
			for each (auto airlock in airlocks)
			{
				closestVessel->Local2Rel(airlock.second.position, airlockRelCoords);
				candidates.push_back(std::pair<oMMUCore*, double>(mgr, dist(ourRelCoords, airlockRelCoords)));
			}
		}
	}

	// Find closest airlock
	if (candidates.size() > 0) {
		auto selectedPair = candidates[0];
		for each (auto candidateAirlock in candidates) {
			if (candidateAirlock.second < bestDistance) {
				bestDistance = candidateAirlock.second;
				selectedPair = candidateAirlock;
			}
		}
		mgr = selectedPair.first;
	}
	return mgr;
}
