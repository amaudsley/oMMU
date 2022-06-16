#pragma once
#define ORBITER_MODULE
#define OMMU_DEFAULT_MESH "mmu"
#define OMMU_DEFAULT_ROLE "Passenger"
#define OMMU_DEFAULT_FUEL 10
#define OMMU_DEFAULT_RCSTH 500
#define OMMU_DEFAULT_RCSISP 10000000
#define OMMU_DEFAULT_WALKING_SPEED 1.4

#include "Orbitersdk.h"
#include "../oMMU Core/oMMU_Core.h"
#include "../oMMU API/oMMU_API.h"
#include "Spotlight.h"

enum mmuState {
	IN_SPACE,
	ON_GROUND_MOBILE,
	ON_GROUND_IMMOBILE,
	ON_GROUND_JUMPING,
	DEAD
};

struct SurfaceMovementInputStatus {
	int xInput = 0;
	int yInput = 0;
	int rotateInput = 0;
	bool doJump = 0;
};

/* Utilized for the (poor) attempt at having a private API */
typedef oMMUCore* (__cdecl* getClosestVessel)(VESSEL* hVessel);
static bool s_isOMMULoaded = false;
static HINSTANCE s_ommuDLLHandle;
static getClosestVessel getClosestMMUVessel;

class oMMU_MMU : public VESSEL4, public IMMUVessel {
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
	void clbkPostCreation();

	void setMMUState(mmuState newState = mmuState::IN_SPACE);
	MATRIX3 RotationMatrix(VECTOR3 angles, bool xyz);
	void doGroundMovement(double deltaT);
	void TryIngress();
	
	virtual void SetCrewData(const oMMUCrew& crew) override {
		mmuData = new oMMUCrew(crew);
	}
	virtual const oMMUCrew* GetCrewData() override {
		return mmuData;
	}

private:
	oMMUCrew* mmuData;
	Spotlight* m_pHelmetSpotlight;

	PROPELLANT_HANDLE m_oxygenResource;
	mmuState m_currentState = mmuState::IN_SPACE;
	bool m_hasBeenInitialized = false;
	SurfaceMovementInputStatus inputStatus;

	// Actual surface velocity.
	// X - Left / Right, Y - Forward / Backwards, Z - Heading?!
	VECTOR3 m_surfaceVelocity = VECTOR3();

	bool doJump = false;
};
