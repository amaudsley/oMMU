#pragma once
#include "oMMU_Data.h"
namespace oMMU_API {
	/**
	 * \brief OMMU Interface class.
	 */
	class oMMU {
	public:

		/** \name Core
		 * Core functions of OMMU - used for initialization / destruction of the OMMUManagement interface
		 */
		 ///@{
		 /**
		  * \brief Instantiates an instance of the OMMUManagement class and then returns a pointer to this instance.
		  * \param hVessel Vessel implementing OMMU, usually 'this'
		  * \return Returns an instance of OMMU, or null if an error has occured
		  */
		static oMMU* GetInstance(VESSEL4* hVessel);

		///@}

		/** \name Simulation
		 * Simulation callbacks of OMMU - these should be placed at the top of their respective functions to ensure that they are called correctly.
		 */
		 ///@{
		/**
		 * \brief Simulate OMMU crew functions - this should be called in clbkPreStep
		 * \return Not used at this time
		 */
		virtual oMMUStatus ProcessClbkPreStep() = 0;

		/**
		 * \brief Recall oMMU crew state from the scenario file
		 * \param line
		 * \return Not used at this time
		 */
		virtual bool RecallState(const char* line) = 0;
		virtual void SaveState(FILEHANDLE scn) = 0;
		///@}

		/** \name Configuration
		* Configuration functions.
		*/
		///@{

		/**
		* \brief Sets the maximum amount of crew that the vessel is able to contain.
		* \param crewLimit The new crew limit.
		* \return Returns CREW_LIMIT_CHANGE_SUCESS on sucess, -1 otherwise
		*/
		virtual oMMUStatus SetCrewLimit(int crewLimit) = 0;

		/**
		 * \brief Creates an arbitary airlock of the given parameters.
		 * \In most scenarios I would recomend using the CreateAirlockFromPort function to ease creation, as this handles creating the
		 * \area of the airlock and tying together the systems needed to allow for crew transfer.
		 * \param airlock Reference to the airlock data structure that should be tied to this airlock. See sample code for example.
		 * \return ID of the created airlock
		 */
		virtual int AddAirlock(const Airlock& airlock) = 0;

		/**
		 * \brief Creates an airlock based on an existing docking port.
		 * \param portID numeric ID of the docking port the airlock should be created against.
		 * \param openByDefault Sets the airlock's default state.
		 * \return ID of the created airlock
		 */
		virtual int CreateAirlockFromPort(int portID, bool initialState) = 0;

		/**
		 * \brief Gets the current status of the airlock identified by airlockID, returns the status in the airlockOut struct.
		 * \This is most commonly used alongside SetAirlockState to update the state of an airlock.
		 * \in the airlockOut variable.
		 * \param airlockID numeric ID of the airlock being queried
		 * \param airlockOut Airlock struct to populate with state data.
		 * \return oMMUStatus indicating method return state.
		 */
		virtual oMMUStatus GetAirlockState(int airlockID, Airlock& airlockOut) = 0;

		/**
		 * \brief Updates the state of the airlock identified by the airlockState structure. We do not pass an airlockID variable (or similar) here
		 * \as that is contained within the airlock struct.
		 * \This is most commonly used alongside GetAirlockState to update the state of an airlock.
		 * \in the airlockOut variable.
		 * \param airlockState Airlock struct indicating the new state of the airlock.
		 * \return oMMUStatus indicating method return state.
		 */
		virtual oMMUStatus SetAirlockState(const Airlock& airlockState) = 0;

		///@}

		/** \name Crew Management
		* Crew management functions.
		*/
		///@{
		/**
		* \brief Adds the crew member specified by the given struct to the vessel if possible.
		* \param crewToAdd a oMMUCrew structure containing the crew member to be added to the vessel
		* \param slot (OPTIONAL) Slot to insert the crew member at, if this parameter is ommited they will be inserted at the first
		* \available crew slot in the vessel
		* \return OMMUStatus::CrewAdded on success, context specific failure message otherwise
		* TODO : Deprecate? SetCrew can serve a similar purpose.
		*/
		virtual oMMUStatus AddCrew(const oMMUCrew& crewToAdd, int slot = 0) = 0;

		/**
		* \brief Removes the crew member at the given index
		* \param index The name of the crew member to remove from the vessel
		* \return OMMUStatus::OK on success, context specific failure message otherwise.
		*/
		virtual oMMUStatus RemoveCrew(int index) = 0;

		/**
		* \brief Retrives the state data of the crew member at the given slot, this may be used to update the state of the given crew member
		* (by calling SetCrew after modifying the structure) or to populate status displays.
		* \param index Index to retrieve a crew member from
		* \param crew reference to the oMMUCrew structure to populate with crew data
		* \return oMMUStatus::OK on sucessful retrival of a crew member, oMMUStatus::Empty if no crew member was present.
		*/
		virtual oMMUStatus GetCrewState(int index, oMMUCrew& crew) = 0;

		/**
		* \brief Updates the state data of the crew member at the given slot.
		* \param index Index of the crew member to update
		* \param crew reference to the oMMUCrew structure containing the updated status.
		* \return oMMUStatus::OK on sucessful update
		*/
		virtual oMMUStatus SetCrewState(int index, const oMMUCrew& crew) = 0;

		/**
		* \brief EVA the crew member at the given index, out of the identified airlock (or the default airlock if non specified)
		* \param crewIndex Index of the crew member to be EVA'd
		* \param airlockIndex Index of the airl ock to EVA from,
		* \return OMMUStatus message - EVASucess on sucess, context specific failure message otherwise
		*/
		virtual oMMUStatus BeginEVA(int crewIndex, int airlockIndex = 0, bool setOnGround = false, bool setFocus = false) = 0;

		/**
		* \brief Transfer the crew member at the given index to the vessel docked to the specified port.
		* This will remove the crew member from the current vessel and transfer them to the other on sucess.
		* \param crewIndex Index of the crew member to be transfered.
		* \param portIndex Index of docking port to transfer to.
		* \return OMMUStatus message - EVASucess on sucess, context specific failure message otherwise
		*/
		virtual oMMUStatus TransferCrew(int crewIndex, int portIndex = 0) = 0;

		/**
		* \brief Attempts to transfer the crew member at the given index to the vessel identified by targetHandle
		* This will remove the crew member from the current vessel and transfer them to the other on sucess.
		* This method should *not* be used for normal docked crew transfer, instead this is for special cases where crew
		* needs to be sent between vessels without them having docked (e.g., transfering crew from a flying vessel to a vessel
		* implementing the crew compliment of a surface base)
		* \param crewIndex Index of the crew member to be transfered.
		* \param targetHandle VESSEL pointer of the targetted vessel.
		* \return OMMUStatus message - EVASucess on sucess, context specific failure message otherwise
		*/
		virtual oMMUStatus TransferCrew(int crewIndex, const VESSEL* targetVessel) = 0;

		///@}


		///@}

		/** \name Interaction Areas
		 * Allows for oMMU crew vessels to interact with other vessels via actions - Carry out repairs, start fires, deploy cargo, etc.
		 */
		 ///@{
		/**
		 * \brief Adds an interaction area.
		 * \return internal ID - used for later retrival / update. This is *not* the interaction ID (Confusing, I know.)
		 */
		virtual int AddInteractionArea(const InteractionArea& area) = 0;

		/**
		 * \brief Retrieves the interaction area identified by areaID to allow for state updates
		 * \return oMMUStatus::OK on Success
		 */
		virtual oMMUStatus GetInteractionArea(int areaID, InteractionArea& out) = 0;

		/**
		* \brief Updates the interaction area identified by AreaID to the state contained in 'in'
		* \return oMMUStatus::OK on Success
		*/
		virtual oMMUStatus SetInteractionAreaState(int areaID, const InteractionArea& in) = 0;

		///@}
	protected:
		oMMU() {}
	};


	/** \name Interfaces
	 * Interfaces allowing for advanced functionality.
	 */
	 ///@{
	/* Interface exposing event handling methods for oMMU capable vessels */
	class ISupportsCrewCallbacks {
	public:
		/**
		 * \brief Event handler for crew transfer events.
		 */
		virtual bool OnTryTransferCrew(const VESSEL* otherVessel, const oMMUCrew& crewMember, int dockingPortID = 0) = 0;

		/**
		 * \brief Event handler for crew ingress events.
		 */
		virtual bool OnTryCrewIngress(const oMMUCrew& crewMember, int airlockID = 1) = 0;
	};

	class ISupportsInteractionAreaCallbacks {
	public:
		virtual bool OnInteractionTriggered(const InteractionArea& area) = 0;
	};

	/* Interface for MMU Vessels. Allows accessing / updating oMMUCrew data against a spawned MMU*/
	class IMMUVessel {
	public:
		/**
		* \brief Sets the oMMUCrew data structure held by an MMU Vessel.
		* \param crew oMMUCrew data to set.
		* \return OMMUStatus message - EVASucess on sucess, context specific failure message otherwise
		*/
		virtual void SetCrewData(const oMMUCrew& crew) = 0;

		/**
		* \brief Gets the oMMUCrew data structure held by an MMU Vessel.
		* \return Pointer to the vessels' oMMUCrew data structure.
		*/
		virtual const oMMUCrew* GetCrewData() = 0;
	};
	///@}

}