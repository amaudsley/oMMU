#pragma once
#include "Orbitersdk.h"

class Spotlight
{
public:
	Spotlight(VESSEL* vessel, VECTOR3 position, VECTOR3 direction, bool active);
	~Spotlight();

	bool Toggle();
private:
	// Parent vessel.
	VESSEL* m_pVessel;
	const VECTOR3 m_beaconColour = { 1,1,1 };

	// Light emitter pointer.
	LightEmitter* m_emitter;

	// Beacon pointer.
	BEACONLIGHTSPEC* m_beacon;
};

