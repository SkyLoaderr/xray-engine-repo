// papi.h
//
// Copyright 1997-1998 by David K. McAllister
// http://www.cs.unc.edu/~davemc/Particle
//
// Include this file in all applications that use the Particle System API.

#ifndef _particle_api_h
#define _particle_api_h

#include <stdlib.h>

// This is the major and minor version number of this release of the API.
#define P_VERSION 120

#ifdef WIN32
#include <windows.h>

#ifdef PARTICLEDLL_EXPORTS
#define PARTICLEDLL_API __declspec(dllexport)
#else
#define PARTICLEDLL_API __declspec(dllimport)
#endif

#else
#define PARTICLEDLL_API
#endif

// Actually this must be < sqrt(MAXFLOAT) since we store this value squared.
#define P_MAXFLOAT 1.0e16f

#ifdef MAXINT
#define P_MAXINT MAXINT
#else
#define P_MAXINT 0x7fffffff
#endif

#define P_EPS 1e-3f

extern "C"{
	//////////////////////////////////////////////////////////////////////
	// Type codes for domains
	PARTICLEDLL_API enum PDomainEnum
	{
		PDPoint = 0, // Single point
		PDLine = 1, // Line segment
		PDTriangle = 2, // Triangle
		PDPlane = 3, // Arbitrarily-oriented plane
		PDBox = 4, // Axis-aligned box
		PDSphere = 5, // Sphere
		PDCylinder = 6, // Cylinder
		PDCone = 7, // Cone
		PDBlob = 8, // Gaussian blob
		PDDisc = 9, // Arbitrarily-oriented disc
		PDRectangle = 10, // Rhombus-shaped planar region
		domain_enum_force_dword = DWORD(-1)
	};

	// State setting calls
	PARTICLEDLL_API void __stdcall pResetState();

	PARTICLEDLL_API void __stdcall pColor(float red, float green, float blue, float alpha = 1.0f);

	PARTICLEDLL_API void __stdcall pColorD(float alpha, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __stdcall pRotate(float rot_x, float rot_y=0.f, float rot_z=0.f);

	PARTICLEDLL_API void __stdcall pRotateD(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __stdcall pSize(float size_x, float size_y = 1.0f, float size_z = 1.0f);

	PARTICLEDLL_API void __stdcall pSizeD(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __stdcall pStartingAge(float age, float sigma = 1.0f);

	PARTICLEDLL_API void __stdcall pTimeStep(float new_dt);

	PARTICLEDLL_API void __stdcall pVelocity(float x, float y, float z);

	PARTICLEDLL_API void __stdcall pVertexB(float x, float y, float z);

	PARTICLEDLL_API void __stdcall pVertexBD(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __stdcall pVertexBTracks(BOOL track_vertex = TRUE);


	// Action List Calls
	PARTICLEDLL_API void __stdcall pCallActionList(int action_list_num);

	PARTICLEDLL_API void __stdcall pDeleteActionLists(int action_list_num, int action_list_count = 1);

	PARTICLEDLL_API void __stdcall pEndActionList();

	PARTICLEDLL_API int __stdcall pGenActionLists(int action_list_count = 1);

	PARTICLEDLL_API void __stdcall pNewActionList(int action_list_num);

	PARTICLEDLL_API void __stdcall pSetActionListParenting(int action_list_num, const Fmatrix& m, const Fvector& velocity);

	// Particle Group Calls

	PARTICLEDLL_API void __stdcall pCopyGroup(int p_src_group_num, int index = 0, int copy_count = P_MAXINT);

	PARTICLEDLL_API void __stdcall pCurrentGroup(int p_group_num);

	PARTICLEDLL_API void __stdcall pDeleteParticleGroups(int p_group_num, int p_group_count = 1);

	PARTICLEDLL_API void __stdcall pDrawGroupl(int dlist, BOOL const_size = FALSE,
		BOOL const_color = FALSE, BOOL const_rotation = FALSE);

	PARTICLEDLL_API void __stdcall pDrawGroupp(int primitive, BOOL const_size = FALSE,
		BOOL const_color = FALSE);

	PARTICLEDLL_API int __stdcall pGenParticleGroups(int p_group_count = 1, int max_particles = 0);

	PARTICLEDLL_API int __stdcall pGetGroupCount();

	PARTICLEDLL_API int __stdcall pGetParticles(int index, int count, float *position = NULL, float *color = NULL,
		float *vel = NULL, float *size = NULL, float *age = NULL);

	PARTICLEDLL_API int __stdcall pSetMaxParticles(int max_count);

	// Actions
	PARTICLEDLL_API void __stdcall pAvoid(float magnitude, float epsilon, float look_ahead,
		PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pBounce(float friction, float resilience, float cutoff,
		PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pCopyVertexB(BOOL copy_pos = TRUE, BOOL copy_vel = FALSE);

	PARTICLEDLL_API void __stdcall pDamping(float damping_x, float damping_y, float damping_z,
		float vlow = 0.0f, float vhigh = P_MAXFLOAT);

	PARTICLEDLL_API void __stdcall pExplosion(float center_x, float center_y, float center_z, float velocity,
		float magnitude, float stdev, float epsilon = P_EPS, float age = 0.0f, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pFollow(float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);

	PARTICLEDLL_API void __stdcall pGravitate(float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);

	PARTICLEDLL_API void __stdcall pGravity(float dir_x, float dir_y, float dir_z, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pJet(float center_x, float center_y, float center_z, float magnitude = 1.0f,
		float epsilon = P_EPS, float max_radius = P_MAXFLOAT, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pKillOld(float age_limit, BOOL kill_less_than = FALSE);

	PARTICLEDLL_API void __stdcall pMatchVelocity(float magnitude = 1.0f, float epsilon = P_EPS,
		float max_radius = P_MAXFLOAT);

	PARTICLEDLL_API void __stdcall pMove();

	PARTICLEDLL_API void __stdcall pOrbitLine(float p_x, float p_y, float p_z,
		float axis_x, float axis_y, float axis_z, float magnitude = 1.0f,
		float epsilon = P_EPS, float max_radius = P_MAXFLOAT, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pOrbitPoint(float center_x, float center_y, float center_z,
		float magnitude = 1.0f, float epsilon = P_EPS,
		float max_radius = P_MAXFLOAT, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pRandomAccel(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pRandomDisplace(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pRandomVelocity(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pRestore(float time);

	PARTICLEDLL_API void __stdcall pSink(BOOL kill_inside, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pSinkVelocity(BOOL kill_inside, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pSource(float particle_rate, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pSpeedLimit(float min_speed, float max_speed = P_MAXFLOAT);

	PARTICLEDLL_API void __stdcall pTargetColor(float color_x, float color_y, float color_z,
		float alpha, float scale);

	PARTICLEDLL_API void __stdcall pTargetColorD(float color_x, float color_y, float color_z,
		float alpha, float scale);

	PARTICLEDLL_API void __stdcall pTargetSize(float size_x, float size_y, float size_z,
		float scale_x = 0.0f, float scale_y = 0.0f, float scale_z = 0.0f);

	PARTICLEDLL_API void __stdcall pTargetRotate(float rot_x, float rot_y, float rot_z, float scale = 0.0f);

	PARTICLEDLL_API void __stdcall pTargetRotateD(float scale, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __stdcall pTargetVelocity(float vel_x, float vel_y, float vel_z, float scale, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pTargetVelocityD(float scale, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_parent=TRUE);

	PARTICLEDLL_API void __stdcall pVertex(float x, float y, float z);

	PARTICLEDLL_API void __stdcall pVortex(float center_x, float center_y, float center_z,
		float axis_x, float axis_y, float axis_z,
		float magnitude = 1.0f, float epsilon = P_EPS,
		float max_radius = P_MAXFLOAT, BOOL allow_parent=TRUE);
};
#endif
