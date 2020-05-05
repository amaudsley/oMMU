/*** Orbiter MMU API ***/
/* Part of the Orbiter MMU package - Version 1
 *
 * Copyright 2017-2019 Adam "Woo482" Maudsley
 * All rights reserved. Do not redestribute.
*/

#pragma once
#include "atlstr.h"
#include "Orbitersdk.h"
namespace oMMU_API {
	enum class oMMUStatus
	{
		OK,
		Failure,
		EVASucess,
		EVAFailure,
		AirlockClosed,
		TargetVesselFull,
		TargetAirlockClosed,
		VesselEmpty,
		CrewTransferSucess,
		CrewTransferFailure,
		VesselFull,
		oMMUNotInstalled,
		CrewLimitChangeSucess,
		CrewLimitChangeFailure,
		AirlockNotFound,
		AirlockStateChangeFailed,
		SlotNotEmpty,
		SlotOutOfBounds
	};

	enum AirlockType {
		dockingPort, sphere, box
	};

	struct Airlock {
		bool isOpen; // Is the airlock open?
		AirlockType type; // Airlock type
		VECTOR3 position; // Airlock position in local coordinates
		VECTOR3 rotation;
		VECTOR3 direction;
		double radius; // Capture radius of the airlock, may be removed and set as constant.
		int associatedDockingPort; // Docking port the airlock is associated with, if any - allows for automatic transfer management if set.
		int GetID() const { return ID; }
	protected:
		int ID;

	};

	struct InteractionArea {
		int ID; // Interaction ID - This does *not* have to be unique, e.g. if there are two possible areas on the vessel to carry out the same action
		bool isActive; // Is the interaction area interactable?
		VECTOR3 position; // Position of the interaction area in vessel-centric coordinates
		double radius; // Radius of the interaction sphere.
	};

	/* Defines an abstract crew member */
	struct oMMUCrew {
		oMMUCrew()
		{

		}
		// Copy constructor
		oMMUCrew(const oMMUCrew& other)
		{
			role = other.role;
			name = other.name;
			age = other.age;
			weight = other.weight;
			pulse = other.pulse;
			evaMesh = other.evaMesh;
			miscData = other.miscData;
		}

		CString role; // Crew member role
		CString name; // Crew member name
		int age; // Crew member age (Deprecate?)
		double weight; // Crew member weight
		int pulse; // Crew member's pulse (Deprecate?)
		CString evaMesh; // Mesh file to use when spawning this crew member
		CString miscData; // Added at the request of dbeachy1 - store arbitary data of your choice
	};

	struct InteractionEventArgs {
		InteractionArea& area;
	};

	struct TransferEventArgs {

		VESSEL* pOtherVessel;
		oMMUCrew& crewMember;
		int dockingPortID;
	};

	struct IngressEventArgs {
		oMMUCrew& crewMember;
		int airlockID;
	};

	/* Event Handler types */
	typedef bool (*OnInteractionHandler) (const InteractionEventArgs& args);
	typedef bool (*OnTransferCrewHandler) (const TransferEventArgs& args);
	typedef bool (*OnCrewIngressHandler) (const IngressEventArgs& args);
}