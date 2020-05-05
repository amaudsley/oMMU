#pragma once
#define ORBITER_MODULE
#define OMMU_DEFAULT_MESH "oMMU\\Crew\\anna_flightsuit_ground"
#define OMMU_DEFAULT_ROLE "Passenger"
#define OMMU_DEFAULT_FUEL 10
#define OMMU_DEFAULT_RCSTH 500
#define OMMU_DEFAULT_RCSISP 10000000
#include "Orbitersdk.h"
#include "../oMMU Core/oMMU_Core.h"
#include "XRSound.h"

enum mmuState {
	IN_SPACE,
	ON_GROUND_MOBILE,
	ON_GROUND_IMMOBILE,
	ON_GROUND_JUMPING,
	DEAD
};

/* Utilized for the (poor) attempt at having a private API */
typedef oMMUCore* (__cdecl* getClosestVessel)(VESSEL* hVessel);
static bool s_isOMMULoaded = false;
static HINSTANCE s_ommuDLLHandle;
static getClosestVessel getClosestMMUVessel;

class oMMU_MMU : public VESSEL4 {
public:
	oMMU_MMU(OBJHANDLE hVessel, int flightmodel);
	~oMMU_MMU();
	/* Orbiter callback methods */
	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkPostStep(double simt, double simdt, double mjd);
	int clbkConsumeBufferedKey(DWORD key, bool down, char* kstate);
	int clbkConsumeDirectKey(char* kstate);
	bool clbkLoadVC(int id);
	void clbkSaveState(FILEHANDLE scn);
	void clbkLoadStateEx(FILEHANDLE scn, void* vs);

	// Inherited via IMMU
	//DLLEXPORT VVV
	void setMMUData(const oMMUCrew& crew) { mmuData = new oMMUCrew(crew); }
	const oMMUCrew* getMMUData() {
		return mmuData;
	}

	void setMMUState(mmuState newState = mmuState::IN_SPACE);
	MATRIX3 RotationMatrix(VECTOR3 angles, bool xyz);
	void doGroundMovement(double deltaT);
	void TryIngress();
	XRSound* m_pXRSound;

private:
	oMMUCrew* mmuData;
	PROPELLANT_HANDLE m_oxygenResource;
	mmuState m_currentState = mmuState::IN_SPACE;
	bool m_hasBeenInitialized = false;


	int rotate = 0;
	/* Surface movement */
	bool m_isInGroundMode = false;
	double m_surfaceAngle = 0;
	double m_surfaceVelocityX = 0;
	double m_surfaceVelocityY = 0;
	double xVelocity;
	double forwardVelocity;
	bool doJump = false;
};
