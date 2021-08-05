#include "oMMU_Bridge.h"
namespace oMMU_API {

	oMMUBridge::oMMUBridge(VESSEL* hVessel)
	{
		m_pVessel = hVessel;
	}

	bool oMMUBridge::Initialize()
	{
		bool retVal = false;
		m_hDLL = LoadLibrary("oMMU_Core.dll");
		DWORD test = GetLastError();
		if (m_hDLL)
		{
			ioMMUFactory poMMU = reinterpret_cast<ioMMUFactory>(GetProcAddress(m_hDLL, "GetoMMUInstance"));
			if (poMMU)
				m_poMMUCore = (poMMU)(m_pVessel);
			return true;
		}
		return false;
	}

	oMMUStatus oMMUBridge::ProcessClbkPreStep()
	{
		if (IsMMULoaded())
			return m_poMMUCore->ProcessClbkPreStep();
		else
			return oMMUStatus::oMMUNotInstalled;
	}

	bool oMMUBridge::RecallState(const char* line)
	{
		if (IsMMULoaded())
			return m_poMMUCore->RecallState(line);
		else
			return false;
	}

	void oMMUBridge::SaveState(FILEHANDLE scn)
	{
		if (IsMMULoaded())
			return m_poMMUCore->SaveState(scn);

	}

	oMMUStatus oMMUBridge::SetCrewLimit(int crewLimit)
	{
		if (IsMMULoaded())
			return m_poMMUCore->SetCrewLimit(crewLimit);
		else
			return oMMUStatus::oMMUNotInstalled;

	}

	int oMMUBridge::AddAirlock(const Airlock& airlock)
	{
		if (IsMMULoaded())
			return m_poMMUCore->AddAirlock(airlock);
		else
			return -1;
	}

	int oMMUBridge::CreateAirlockFromPort(int portID, bool openByDefault)
	{
		if (IsMMULoaded())
			return m_poMMUCore->CreateAirlockFromPort(portID, openByDefault);
		else
			return -1;
	}

	oMMUStatus oMMUBridge::SetAirlockState(const Airlock& airlockState)
	{
		if (IsMMULoaded())
			return m_poMMUCore->SetAirlockState(airlockState);
		else
			return oMMUStatus::oMMUNotInstalled;
	}

	oMMUStatus oMMUBridge::GetAirlockState(int airlockID, Airlock& airlockOut)
	{
		if (IsMMULoaded())
			return m_poMMUCore->GetAirlockState(airlockID, airlockOut);
		else
			return oMMUStatus::oMMUNotInstalled;

	}


	oMMUStatus oMMUBridge::BeginEVA(int crewIndex, int airlockIndex, bool setOnGround, bool setFocus)
	{
		if (IsMMULoaded())
			return m_poMMUCore->BeginEVA(crewIndex, airlockIndex, setOnGround, setFocus);
		else
			return oMMUStatus::oMMUNotInstalled;
	}

	oMMUStatus oMMUBridge::AddCrew(const oMMUCrew& crewToAdd, int slot)
	{
		if (IsMMULoaded())
			return m_poMMUCore->AddCrew(crewToAdd, slot);
		else
			return oMMUStatus::oMMUNotInstalled;
	}

	oMMUStatus oMMUBridge::RemoveCrew(int index)
	{
		if (IsMMULoaded())
			return m_poMMUCore->RemoveCrew(index);
		else
			return oMMUStatus::oMMUNotInstalled;
	}

	oMMUStatus oMMUBridge::GetCrewState(int index, oMMUCrew& crew)
	{
		if (IsMMULoaded())
			return m_poMMUCore->GetCrewState(index, crew);
		else
			return oMMUStatus::oMMUNotInstalled;
	}

	oMMUStatus oMMUBridge::SetCrewState(int index, const oMMUCrew& crew)
	{
		if (IsMMULoaded())
			return m_poMMUCore->SetCrewState(index, crew);
		else
			return oMMUStatus::oMMUNotInstalled;
	}

	oMMUStatus oMMUBridge::TransferCrew(int crewIndex, int portIndex)
	{
		if (IsMMULoaded())
			return m_poMMUCore->TransferCrew(crewIndex, portIndex);
		else
			return oMMUStatus::oMMUNotInstalled;
	}
	
	oMMUStatus oMMUBridge::TransferCrew(int crewIndex, const VESSEL* targetVessel)
	{
		if (IsMMULoaded())
			return m_poMMUCore->TransferCrewDirect(crewIndex, targetVessel);
		else
			return oMMUStatus::oMMUNotInstalled;
	}

	int oMMUBridge::AddInteractionArea(const InteractionArea& area)
	{
		if (IsMMULoaded())
			return m_poMMUCore->AddInteractionArea(area);
		else
			return -1;
	}

	oMMUStatus oMMUBridge::GetInteractionArea(int areaID, InteractionArea& out)
	{
		if (IsMMULoaded())
			return m_poMMUCore->GetInteractionArea(areaID, out);
		else
			return oMMUStatus::oMMUNotInstalled;
	}

	oMMUStatus oMMUBridge::SetInteractionAreaState(int areaID, const InteractionArea& in)
	{
		if (IsMMULoaded())
			return m_poMMUCore->SetInteractionAreaState(areaID, in);
		else
			return oMMUStatus::oMMUNotInstalled;
	}
}