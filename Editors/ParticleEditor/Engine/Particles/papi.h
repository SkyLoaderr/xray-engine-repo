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
		PDRectangle = 10 // Rhombus-shaped planar region
	};

	// State setting calls

	PARTICLEDLL_API void __cdecl pColor(float red, float green, float blue, float alpha = 1.0f);

	PARTICLEDLL_API void __cdecl pColorD(float alpha, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __cdecl pSize(float size_x, float size_y = 1.0f, float size_z = 1.0f);

	PARTICLEDLL_API void __cdecl pSizeD(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __cdecl pStartingAge(float age, float sigma = 1.0f);

	PARTICLEDLL_API void __cdecl pTimeStep(float new_dt);

	PARTICLEDLL_API void __cdecl pVelocity(float x, float y, float z);

	PARTICLEDLL_API void __cdecl pVelocityD(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __cdecl pVertexB(float x, float y, float z);

	PARTICLEDLL_API void __cdecl pVertexBD(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __cdecl pVertexBTracks(bool track_vertex = true);


	// Action List Calls

	PARTICLEDLL_API void __cdecl pCallActionList(int action_list_num);

	PARTICLEDLL_API void __cdecl pDeleteActionLists(int action_list_num, int action_list_count = 1);

	PARTICLEDLL_API void __cdecl pEndActionList();

	PARTICLEDLL_API int __cdecl pGenActionLists(int action_list_count = 1);

	PARTICLEDLL_API void __cdecl pNewActionList(int action_list_num);


	// Particle Group Calls

	PARTICLEDLL_API void __cdecl pCopyGroup(int p_src_group_num, int index = 0, int copy_count = P_MAXINT);

	PARTICLEDLL_API void __cdecl pCurrentGroup(int p_group_num);

	PARTICLEDLL_API void __cdecl pDeleteParticleGroups(int p_group_num, int p_group_count = 1);

	PARTICLEDLL_API void __cdecl pDrawGroupl(int dlist, bool const_size = false,
		bool const_color = false, bool const_rotation = false);

	PARTICLEDLL_API void __cdecl pDrawGroupp(int primitive, bool const_size = false,
		bool const_color = false);

	PARTICLEDLL_API int __cdecl pGenParticleGroups(int p_group_count = 1, int max_particles = 0);

	PARTICLEDLL_API int __cdecl pGetGroupCount();

	PARTICLEDLL_API int __cdecl pGetParticles(int index, int count, float *position = NULL, float *color = NULL,
		float *vel = NULL, float *size = NULL, float *age = NULL);

	PARTICLEDLL_API int __cdecl pSetMaxParticles(int max_count);


	// Actions

	PARTICLEDLL_API void __cdecl pAvoid(float magnitude, float epsilon, float look_ahead, 
		PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __cdecl pBounce(float friction, float resilience, float cutoff,
		PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __cdecl pCopyVertexB(bool copy_pos = true, bool copy_vel = false);

	PARTICLEDLL_API void __cdecl pDamping(float damping_x, float damping_y, float damping_z,
		float vlow = 0.0f, float vhigh = P_MAXFLOAT);

	PARTICLEDLL_API void __cdecl pExplosion(float center_x, float center_y, float center_z, float velocity,
		float magnitude, float stdev, float epsilon = P_EPS, float age = 0.0f);

	PARTICLEDLL_API void __cdecl pFollow(float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);

	PARTICLEDLL_API void __cdecl pGravitate(float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);

	PARTICLEDLL_API void __cdecl pGravity(float dir_x, float dir_y, float dir_z);

	PARTICLEDLL_API void __cdecl pJet(float center_x, float center_y, float center_z, float magnitude = 1.0f,
		float epsilon = P_EPS, float max_radius = P_MAXFLOAT);

	PARTICLEDLL_API void __cdecl pKillOld(float age_limit, bool kill_less_than = false);

	PARTICLEDLL_API void __cdecl pMatchVelocity(float magnitude = 1.0f, float epsilon = P_EPS,
		float max_radius = P_MAXFLOAT);

	PARTICLEDLL_API void __cdecl pMove();

	PARTICLEDLL_API void __cdecl pOrbitLine(float p_x, float p_y, float p_z,
		float axis_x, float axis_y, float axis_z, float magnitude = 1.0f,
		float epsilon = P_EPS, float max_radius = P_MAXFLOAT);

	PARTICLEDLL_API void __cdecl pOrbitPoint(float center_x, float center_y, float center_z,
		float magnitude = 1.0f, float epsilon = P_EPS,
		float max_radius = P_MAXFLOAT);

	PARTICLEDLL_API void __cdecl pRandomAccel(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __cdecl pRandomDisplace(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __cdecl pRandomVelocity(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __cdecl pRestore(float time);

	PARTICLEDLL_API void __cdecl pShade(float color_x, float color_y, float color_z,
		float alpha, float scale);

	PARTICLEDLL_API void __cdecl pSink(bool kill_inside, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __cdecl pSinkVelocity(bool kill_inside, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __cdecl pSource(float particle_rate, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void __cdecl pSpeedLimit(float min_speed, float max_speed = P_MAXFLOAT);

	PARTICLEDLL_API void __cdecl pTargetColor(float color_x, float color_y, float color_z,
		float alpha, float scale);

	PARTICLEDLL_API void __cdecl pTargetSize(float size_x, float size_y, float size_z,
		float scale_x = 0.0f, float scale_y = 0.0f, float scale_z = 0.0f);

	PARTICLEDLL_API void __cdecl pTargetVelocity(float vel_x, float vel_y, float vel_z, float scale);

	PARTICLEDLL_API void __cdecl pVertex(float x, float y, float z);

	PARTICLEDLL_API void __cdecl pVortex(float center_x, float center_y, float center_z,
		float axis_x, float axis_y, float axis_z,
		float magnitude = 1.0f, float epsilon = P_EPS,
		float max_radius = P_MAXFLOAT);

};
#endif
