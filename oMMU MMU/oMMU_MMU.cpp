#include "oMMU_MMU.h"

static TOUCHDOWNVTX boundingBoxVerticies[8] = {
	{ _V(0,  -1.3, 10.5),	3e4, 1e3, 15.0, 3.0 },
	{ _V(-1.5,  -1.3,-10.5), 3e4, 1e3, 15.0, 3.0 },
	{ _V(1.5,  -1.3,-10.5),	3e4, 1e3, 15.0, 3.0 },
	{ _V(0,  -1.3, -10.5),	3e4, 1e3, 15.0 },
	{ _V(0,  1.5, 3.0),	 3e4, 1e3, 15.0 },
	{ _V(-0.4,  1.5,-3.0),  3e4, 1e3, 15.0 },
	{ _V(0.4,  1.5,-3.0),	 3e4, 1e3, 15.0 },
	{ _V(0,  1.5, -3.0),	3e4, 1e3, 15.0 }
};

oMMU_MMU::oMMU_MMU(OBJHANDLE hVessel, int flightmodel)
	: VESSEL4(hVessel, flightmodel)
{
	s_isOMMULoaded = false;
	mmuData = nullptr;
	s_ommuDLLHandle = nullptr;

}

oMMU_MMU::~oMMU_MMU()
{
	s_isOMMULoaded = false;
	if (s_ommuDLLHandle != nullptr)
		FreeLibrary(s_ommuDLLHandle);

	delete mmuData;
	mmuData = nullptr;
}


// ==============================================================
// Overloaded callback functions
// ==============================================================

// --------------------------------------------------------------
// Set the capabilities of the vessel class
// --------------------------------------------------------------
void oMMU_MMU::clbkSetClassCaps(FILEHANDLE cfg)
{
	THRUSTER_HANDLE th_rcs[14], th_group[4];
	PROPELLANT_HANDLE hMainFuel = CreatePropellantResource(OMMU_DEFAULT_FUEL);
	m_oxygenResource = CreatePropellantResource(0.5, 0.5, 1);
	SetTouchdownPoints(boundingBoxVerticies, 8);
	//SetTouchdownPoints(_V(0, -1.3, 1), _V(-1, -1.3, -1), _V(1, -1.3, -1));
	/* Borrowed from the shuttle PB, I'm about 90% sure we can simplify these but it will work for testing purposes */
	th_rcs[0] = CreateThruster(_V(1, 0, 3), _V(0, 1, 0), OMMU_DEFAULT_RCSTH, hMainFuel, OMMU_DEFAULT_RCSISP);
	th_rcs[1] = CreateThruster(_V(1, 0, 3), _V(0, -1, 0), OMMU_DEFAULT_RCSTH, hMainFuel, OMMU_DEFAULT_RCSISP);
	th_rcs[2] = CreateThruster(_V(-1, 0, 3), _V(0, 1, 0), OMMU_DEFAULT_RCSTH, hMainFuel, OMMU_DEFAULT_RCSISP);
	th_rcs[3] = CreateThruster(_V(-1, 0, 3), _V(0, -1, 0), OMMU_DEFAULT_RCSTH, hMainFuel, OMMU_DEFAULT_RCSISP);
	th_rcs[4] = CreateThruster(_V(1, 0, -3), _V(0, 1, 0), OMMU_DEFAULT_RCSTH, hMainFuel, OMMU_DEFAULT_RCSISP);
	th_rcs[5] = CreateThruster(_V(1, 0, -3), _V(0, -1, 0), OMMU_DEFAULT_RCSTH, hMainFuel, OMMU_DEFAULT_RCSISP);
	th_rcs[6] = CreateThruster(_V(-1, 0, -3), _V(0, 1, 0), OMMU_DEFAULT_RCSTH, hMainFuel, OMMU_DEFAULT_RCSISP);
	th_rcs[7] = CreateThruster(_V(-1, 0, -3), _V(0, -1, 0), OMMU_DEFAULT_RCSTH, hMainFuel, OMMU_DEFAULT_RCSISP);
	th_rcs[8] = CreateThruster(_V(1, 0, 3), _V(-1, 0, 0), OMMU_DEFAULT_RCSTH, hMainFuel, OMMU_DEFAULT_RCSISP);
	th_rcs[9] = CreateThruster(_V(-1, 0, 3), _V(1, 0, 0), OMMU_DEFAULT_RCSTH, hMainFuel, OMMU_DEFAULT_RCSISP);
	th_rcs[10] = CreateThruster(_V(1, 0, -3), _V(-1, 0, 0), OMMU_DEFAULT_RCSTH, hMainFuel, OMMU_DEFAULT_RCSISP);
	th_rcs[11] = CreateThruster(_V(-1, 0, -3), _V(1, 0, 0), OMMU_DEFAULT_RCSTH, hMainFuel, OMMU_DEFAULT_RCSISP);
	th_rcs[12] = CreateThruster(_V(0, 0, -3), _V(0, 0, 1), OMMU_DEFAULT_RCSTH, hMainFuel, OMMU_DEFAULT_RCSISP);
	th_rcs[13] = CreateThruster(_V(0, 0, 3), _V(0, 0, -1), OMMU_DEFAULT_RCSTH, hMainFuel, OMMU_DEFAULT_RCSISP);

	th_group[0] = th_rcs[0];
	th_group[1] = th_rcs[2];
	th_group[2] = th_rcs[5];
	th_group[3] = th_rcs[7];
	CreateThrusterGroup(th_group, 4, THGROUP_ATT_PITCHUP);

	th_group[0] = th_rcs[1];
	th_group[1] = th_rcs[3];
	th_group[2] = th_rcs[4];
	th_group[3] = th_rcs[6];
	CreateThrusterGroup(th_group, 4, THGROUP_ATT_PITCHDOWN);

	th_group[0] = th_rcs[0];
	th_group[1] = th_rcs[4];
	th_group[2] = th_rcs[3];
	th_group[3] = th_rcs[7];
	CreateThrusterGroup(th_group, 4, THGROUP_ATT_BANKLEFT);

	th_group[0] = th_rcs[1];
	th_group[1] = th_rcs[5];
	th_group[2] = th_rcs[2];
	th_group[3] = th_rcs[6];
	CreateThrusterGroup(th_group, 4, THGROUP_ATT_BANKRIGHT);

	th_group[0] = th_rcs[0];
	th_group[1] = th_rcs[4];
	th_group[2] = th_rcs[2];
	th_group[3] = th_rcs[6];
	CreateThrusterGroup(th_group, 4, THGROUP_ATT_UP);

	th_group[0] = th_rcs[1];
	th_group[1] = th_rcs[5];
	th_group[2] = th_rcs[3];
	th_group[3] = th_rcs[7];
	CreateThrusterGroup(th_group, 4, THGROUP_ATT_DOWN);

	th_group[0] = th_rcs[8];
	th_group[1] = th_rcs[11];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_YAWLEFT);

	th_group[0] = th_rcs[9];
	th_group[1] = th_rcs[10];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_YAWRIGHT);

	th_group[0] = th_rcs[8];
	th_group[1] = th_rcs[10];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_LEFT);

	th_group[0] = th_rcs[9];
	th_group[1] = th_rcs[11];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_RIGHT);

	CreateThrusterGroup(th_rcs + 12, 1, THGROUP_ATT_FORWARD);
	CreateThrusterGroup(th_rcs + 13, 1, THGROUP_ATT_BACK);
}

/// <summary>
/// Called before each simulation step.
/// Here we handle runtime state transitions, ground movement, etc.
/// </summary>
/// <param name="simt"></param>
/// <param name="simdt"></param>
/// <param name="mjd"></param>
void oMMU_MMU::clbkPreStep(double simt, double simdt, double mjd)
{
	// TODO : Move to clbkPostCreation?
	if (m_hasBeenInitialized == false)
		setMMUState();

	/* Pre-step behaviour for an MMU that is still alive */
	if (m_currentState != mmuState::DEAD) {
		if (GroundContact()) {
			if (m_currentState == mmuState::IN_SPACE) {
				if (GetAirspeed() < 5.0f) {
					setMMUState(mmuState::ON_GROUND_MOBILE);
				}
				else {
					setMMUState(mmuState::DEAD);
				}
			}
			else if (m_currentState == mmuState::ON_GROUND_MOBILE) {
				doGroundMovement(simdt);
			}
		}
		else if (m_currentState != mmuState::DEAD && m_currentState != mmuState::IN_SPACE) {
			setMMUState(mmuState::IN_SPACE);
		}
	}
}

void oMMU_MMU::clbkPostStep(double simt, double simdt, double mjd)
{

}

int oMMU_MMU::clbkConsumeBufferedKey(DWORD key, bool down, char* kstate)
{
	if (!down)
		return 0;
	if (key == OAPI_KEY_E) {
		TryIngress();
	}

	return 0;
}

int oMMU_MMU::clbkConsumeDirectKey(char* kstate)
{
	if (m_currentState == mmuState::ON_GROUND_MOBILE) {
		this->SetAttitudeMode(RCS_NONE);
		if (KEYDOWN(kstate, OAPI_KEY_W)) {
			inputStatus.yInput = 1;
		}
		else if (KEYDOWN(kstate, OAPI_KEY_S)) {
			inputStatus.yInput = -1;
		}
		else
		{
			inputStatus.yInput = 0;
		}

		if (KEYDOWN(kstate, OAPI_KEY_A)) {
			inputStatus.xInput = -1;
		}
		else if (KEYDOWN(kstate, OAPI_KEY_D)) {
			inputStatus.xInput = 1;
		}
		else
		{
			inputStatus.xInput = 0;
		}

		if (KEYDOWN(kstate, OAPI_KEY_E)) {
			inputStatus.rotateInput = 1;
		}
		else if (KEYDOWN(kstate, OAPI_KEY_Q)) {
			inputStatus.rotateInput = -1;
		}
		else {
			inputStatus.rotateInput = 0;
		}
	}
	return 0;
}

bool oMMU_MMU::clbkLoadVC(int id)
{
	SetCameraOffset(_V(0.0, 1.5, 0.0));
	SetCameraDefaultDirection(_V(0, 0, 1));
	SetCameraRotationRange(RAD * 120, RAD * 120, RAD * 70, RAD * 70);
	SetCameraShiftRange(_V(0, 0, 0.1), _V(-0.2, 0, 0), _V(0.2, 0, 0));
	return true;
}

/* Sets the MMU state based on the oMMUCrew data structure passed to it. */
void oMMU_MMU::setMMUState(mmuState newState)
{
	if (m_hasBeenInitialized == false) {
		/* Set some defaults if we've started without a mmuData struct (Usually only happens when something breaks terribly, or the user spawns a mmu manually)*/
		if (mmuData == nullptr) {
			mmuData = new oMMUCrew();
			mmuData->age = 30;
			mmuData->pulse = 70;
			mmuData->weight = 140;
			mmuData->name = GetName(); // Default to the name of the vessel.
			mmuData->role = OMMU_DEFAULT_ROLE;
		}
		if (mmuData->evaMesh != "") {
			AddMesh(mmuData->evaMesh);
		}
		else
			AddMesh(OMMU_DEFAULT_MESH);

		SetEmptyMass(mmuData->weight);

		m_hasBeenInitialized = true;
	}
	m_currentState = newState;
}

/// <summary>
/// Taken from general vessel.
/// Implements the rotation matrix needed to convert local XYZ rotations to global orientation.
/// </summary>
/// <param name="angles"></param>
/// <param name="xyz"></param>
/// <returns></returns>
MATRIX3 oMMU_MMU::RotationMatrix(VECTOR3 angles, bool xyz = FALSE)
{
	MATRIX3 m;
	MATRIX3 RM_X, RM_Y, RM_Z;
	RM_X = _M(1, 0, 0, 0, cos(angles.x), -sin(angles.x), 0, sin(angles.x), cos(angles.x));
	RM_Y = _M(cos(angles.y), 0, sin(angles.y), 0, 1, 0, -sin(angles.y), 0, cos(angles.y));
	RM_Z = _M(cos(angles.z), -sin(angles.z), 0, sin(angles.z), cos(angles.z), 0, 0, 0, 1);
	if (!xyz) {
		m = mul(RM_Z, mul(RM_Y, RM_X));
	}
	else {
		m = mul(RM_X, mul(RM_Y, RM_Z));
	}
	return m;
}


static inline void CalculateVelocity(double& velocity, int inputStatus, double simdt) {
	if (inputStatus != 0) {
		velocity += (OMMU_DEFAULT_WALKING_SPEED * simdt) * inputStatus;
		if (velocity > OMMU_DEFAULT_WALKING_SPEED) velocity = OMMU_DEFAULT_WALKING_SPEED;
		if (velocity < -OMMU_DEFAULT_WALKING_SPEED) velocity = -OMMU_DEFAULT_WALKING_SPEED;
	}
	else {
		// 'Drag' implementation.
		velocity -= (velocity * simdt);
	}

	if (velocity > -0.01 && velocity < 0.01) velocity = 0.0;
}
// TODO : Implement acceleration 
void oMMU_MMU::doGroundMovement(double deltaT)
{
	// Calculate the current X / Y velocity.
	CalculateVelocity(m_surfaceVelocity.x, inputStatus.xInput, deltaT);
	CalculateVelocity(m_surfaceVelocity.y, inputStatus.yInput, deltaT);

	VESSELSTATUS2 vs2;
	memset(&vs2, 0, sizeof(vs2));
	vs2.version = 2;
	GetStatusEx(&vs2);

	double planetRadius = oapiGetSize(GetSurfaceRef());
	double degreesPerMeter = (planetRadius * 2 * PI) / 360;

	if (inputStatus.rotateInput == 0) {
		vs2.surf_hdg += (45 * deltaT) * RAD;
	}
	if (inputStatus.rotateInput == -1) {
		vs2.surf_hdg -= (45 * deltaT) * RAD;
	}

	if (vs2.surf_hdg > 360)
		vs2.surf_hdg = 0;
	if (vs2.surf_hdg < 0) {
		vs2.surf_hdg = 360;
	}

	// TODO: Tidy this up once I remember basic trig.
	double d_lat, d_lng;
	d_lat = (m_surfaceVelocity.y * cos(vs2.surf_hdg));
	d_lng = (m_surfaceVelocity.y * sin(vs2.surf_hdg));

	// Calculate sideways component
	double sidewaysMovementDirection = m_surfaceVelocity.x < -0.00 ? -1 : 1;
	d_lat += ((m_surfaceVelocity.x * cos(vs2.surf_hdg + (1.571 * sidewaysMovementDirection)))) * sidewaysMovementDirection;
	d_lng += ((m_surfaceVelocity.x * sin(vs2.surf_hdg + (1.571 * sidewaysMovementDirection)))) * sidewaysMovementDirection;

	vs2.surf_lat += ((d_lat * deltaT) / degreesPerMeter) * RAD;
	vs2.surf_lng += ((d_lng * deltaT) / degreesPerMeter) * RAD;

	// Update the rotation matix.
	double lng, lat, hdg;
	lng = vs2.surf_lng;
	lat = vs2.surf_lat;
	hdg = vs2.surf_hdg;

	MATRIX3 rot1 = RotationMatrix(_V(0 * RAD, (90 * RAD - lng), 0 * RAD), TRUE);
	MATRIX3 rot2 = RotationMatrix(_V(-lat + 0 * RAD, 0, 0 * RAD), TRUE);
	MATRIX3 rot3 = RotationMatrix(_V(0, 0, 180 * RAD + hdg), TRUE);

	MATRIX3 rot4 = RotationMatrix(_V(90 * RAD, 0, 0), TRUE);

	MATRIX3 RotMatrix_Def = mul(rot1, mul(rot2, mul(rot3, rot4)));

	vs2.arot.x = atan2(RotMatrix_Def.m23, RotMatrix_Def.m33);
	vs2.arot.y = -asin(RotMatrix_Def.m13);
	vs2.arot.z = atan2(RotMatrix_Def.m12, RotMatrix_Def.m11);
	vs2.vrot.x = 1.3;

	// Update the vessel state.
	DefSetStateEx(&vs2);
}

/// <summary>
/// TODO : Move this into oMMUCore?
/// </summary>
void oMMU_MMU::TryIngress()
{
	/* Load the oMMU Core DLL (if not already loaded) */
	if (!s_isOMMULoaded) {
		bool dllLoadSuccess = false;
		s_ommuDLLHandle = LoadLibrary("oMMU_Core.dll");
		if (s_ommuDLLHandle) {
			/* Basic error check - to be improved */
			if (!s_ommuDLLHandle) {
				s_isOMMULoaded = false;
			}

			getClosestMMUVessel = reinterpret_cast<getClosestVessel>(GetProcAddress(s_ommuDLLHandle, "GetClosestMMUCompatibleVessel"));
			if (!getClosestMMUVessel) {
				dllLoadSuccess = false;
				FreeLibrary(s_ommuDLLHandle);
			}
			s_isOMMULoaded = true;
		}
	}

	/* Get the closest vessel, or NULL if no vessels within range (1 KM) */
	oMMUCore* mgr = getClosestMMUVessel(this);
	if (mgr == nullptr) {
		// Something
	}
	else {
		oMMUStatus retVal = mgr->TryIngress(this, nullptr);
		if (retVal != oMMUStatus::Failure) {
			oapiDeleteVessel(this->GetHandle());
		}

	}
}

void oMMU_MMU::clbkLoadStateEx(FILEHANDLE scn, void* vs)
{
	char* line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!_strnicmp(line, "CREW", 4)) {
			mmuData = new oMMUCrew();
			sscanf(line + 4, "%[^-]::%[^::]::%i::%i::%lf::%[^::]::%[^::]", mmuData->role.GetBuffer(2048), mmuData->name.GetBuffer(2048),
				&mmuData->age, &mmuData->pulse, &mmuData->weight, mmuData->evaMesh.GetBuffer(2048), mmuData->miscData.GetBuffer(2048));
			mmuData->name.ReleaseBuffer();
			mmuData->role.ReleaseBuffer();
			mmuData->evaMesh.ReleaseBuffer();
			mmuData->miscData.ReleaseBuffer();
		}
		ParseScenarioLineEx(line, vs);

	}
}

/* Save status to the scenario file */
void oMMU_MMU::clbkSaveState(FILEHANDLE scn)
{
	// default vessel parameters
	SaveDefaultState(scn);
	CString cbuf;
	cbuf.Format("%s::%s::%i::%i::%lf::%s", mmuData->role, mmuData->name, mmuData->age, mmuData->pulse, mmuData->weight, mmuData->evaMesh, mmuData->miscData);
	oapiWriteScenario_string(scn, "CREW", cbuf.GetBuffer());
}

// ==============================================================
// API callback interface
// ==============================================================

// --------------------------------------------------------------
// Vessel initialisation
// --------------------------------------------------------------
DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel)
{
	return new oMMU_MMU(hvessel, flightmodel);
}

// --------------------------------------------------------------
// Vessel cleanup
// --------------------------------------------------------------
DLLCLBK void ovcExit(VESSEL* vessel)
{
	if (vessel) delete (oMMU_MMU*)vessel;
}
