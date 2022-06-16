#include "Spotlight.h"

Spotlight::Spotlight(VESSEL* vessel, VECTOR3 position, VECTOR3 direction, bool active)
{
	m_pVessel = vessel;
	
	static COLOUR4 col_a = { 0,0,0,0 };
	static COLOUR4 col_white = {0.5,0.5,0.7,0};
	
	// Create the spotlight
	m_emitter = m_pVessel->AddSpotLight(position, direction, 150, 1e-3, 0, 1e-3, RAD * 30, RAD * 60, col_white, col_white, col_a);
	m_emitter->Activate(active);

	// Add a beacon to the origin.
	static VECTOR3 beacon_color = { 1,1,1 };
	m_beacon = new BEACONLIGHTSPEC();
	m_beacon->pos = new VECTOR3(position);
	m_beacon->shape = (BEACONSHAPE_DIFFUSE);
	m_beacon->col = &beacon_color;
	m_beacon->active = active;
	m_beacon->period = 0;
	m_beacon->size = 0.1;
	m_pVessel->AddBeacon(m_beacon);
}

Spotlight::~Spotlight()
{
	m_pVessel->DelBeacon(m_beacon);
	m_beacon = nullptr;
	m_emitter = nullptr;
	m_pVessel = nullptr;
}

bool Spotlight::Toggle()
{
	bool newState = !m_emitter->IsActive();
	m_emitter->Activate(newState);
	m_beacon->active = newState;
	return newState;
}
