#pragma once
#include "oMMU_API.h"
#include "../oMMU Core/oMMU_Core.h"
namespace oMMU_API {
	/* oMMUBridge - provides the actual interace between the public oMMU API and the oMMU core */
	class oMMUBridge : public oMMU {
	public:
		oMMUBridge(VESSEL4* hVessel);
		~oMMUBridge()
		{
			delete m_poMMUCore;
			m_poMMUCore = nullptr;
		}
		// Inherited via oMMU
		bool Initialize();
		bool IsMMULoaded() { return (m_hDLL != nullptr); }
		virtual oMMUStatus ProcessClbkPreStep() override;
		virtual bool RecallState(const char* line) override;
		virtual void SaveState(FILEHANDLE scn) override;
		virtual oMMUStatus SetCrewLimit(int crewLimit) override;
		virtual int AddAirlock(const Airlock & airlock) override;
		virtual int CreateAirlockFromPort(int portID, bool openByDefault) override;
		virtual oMMUStatus SetAirlockState(const Airlock & airlockState) override;
		virtual oMMUStatus GetAirlockState(int airlockID, Airlock & airlockOut) override;
		virtual oMMUStatus BeginEVA(int crewIndex, int airlockIndex = 0, bool setOnGround = false, bool setFocus = false) override;
		virtual oMMUStatus AddCrew(const oMMUCrew& crewToAdd, int slot = 0) override;
		virtual oMMUStatus RemoveCrew(int index) override;
		virtual oMMUStatus GetCrewState(int index, oMMUCrew& crew) override;
		virtual oMMUStatus SetCrewState(int index, const oMMUCrew& crew) override;
		virtual oMMUStatus TransferCrew(int crewIndex, int portIndex = 0) override;
		virtual oMMUStatus TransferCrew(int crewIndex, const VESSEL* targetVessel) override;
		virtual int AddInteractionArea(const InteractionArea& area) override;
		virtual oMMUStatus GetInteractionArea(int areaID, InteractionArea& out) override;
		virtual oMMUStatus SetInteractionAreaState(int areaID, const InteractionArea& in) override;

	private:
		HMODULE m_hDLL;
		oMMUCore * m_poMMUCore;
		VESSEL4 * m_pVessel;		

	};
}