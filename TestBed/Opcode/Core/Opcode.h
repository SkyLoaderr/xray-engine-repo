///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Main file for Opcode.dll.
 *	\file		Opcode.h
 *	\author		Pierre Terdiman
 *	\date		March, 20, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __OPCODE_H__
#define __OPCODE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compilation messages
#if defined(OPCODEDISTRIB_EXPORTS)
	#pragma message("Compiling OPCODE")
#elif !defined(OPCODEDISTRIB_EXPORTS)
	#pragma message("Using OPCODE")
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Preprocessor
#ifdef OPCODEDISTRIB_EXPORTS
	#define OPCODE_API __declspec(dllexport)
#else
	#define OPCODE_API __declspec(dllimport)
#endif

#ifndef __ICECORE_H__
	#ifdef WIN32
	#include <windows.h>
	#include <windowsx.h>
	#endif // WIN32

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <assert.h>
	#include <shlobj.h>
	#include <mmsystem.h>
	#include <stdarg.h>
	#include <time.h>
	#include <float.h>

	#ifndef ASSERT
		#define	ASSERT	assert
	#endif

	#define	Log
	#define	SetIceError		false
	#define	EC_OUTOFMEMORY	"Out of memory"
	#define	Alignment
	#define ICECORE_API		OPCODE_API
	#define	override(baseclass)	virtual

	// IceCore includes
	#include "IceTypes.h"
	#include "IceFPU.h"
	#include "IceMemoryMacros.h"
	#include "IceContainer.h"
	#include "IceRandom.h"
#endif

#ifndef __ICEMATHS_H__
	#include <Math.h>

	#define ICEMATHS_API	OPCODE_API

	class HPoint;
	class Matrix3x3;
	class Matrix4x4;
	class Quat;
	class PRS;
	class PR;

	// IceMaths includes
	#include "IcePoint.h"
	#include "IceHPoint.h"
	#include "IceMatrix3x3.h"
	#include "IceMatrix4x4.h"
	#include "IceRay.h"
#endif


#ifndef __MESHMERIZER_H__

	#define MESHMERIZER_API	OPCODE_API

	enum CubeIndex;
	class Plane;
	class ProgressiveEigen;

	// Meshmerizer includes
	#include "IceTriangle.h"
	#include "IceAABB.h"
	#include "IceBoundingSphere.h"
#endif

	namespace Opcode
	{
		// Bulk-of-the-work
		#include "OPC_Settings.h"
		#include "OPC_Common.h"
		#include "OPC_TreeBuilders.h"
		#include "OPC_AABBTree.h"
		#include "OPC_OptimizedTree.h"
		#include "OPC_Model.h"
		#include "OPC_BVTCache.h"
		#include "OPC_Collider.h"
		#include "OPC_TreeCollider.h"
		#include "OPC_RayCollider.h"
		#include "OPC_SphereCollider.h"
	}

#endif // __OPCODE_H__
