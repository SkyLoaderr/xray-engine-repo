///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for a sphere collider.
 *	\file		OPC_SphereCollider.cpp
 *	\author		Pierre Terdiman
 *	\date		June, 2, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains an AABB sphere collider.
 *	This class performs a collision test between a sphere and an AABB tree. You can use this to do a standard player vs world collision,
 *	in a Nettle/Telemachos way. It doesn't suffer from all reported bugs in those two classic codes - the "new" one by Paul Nettle is a
 *	debuggued version I think. Collision response can be driven by reported collision data - it works extremely well for me. In sake of
 *	efficiency, all meshes (that is, all AABB trees) should of course also be kept in an extra hierarchical structure (octree, whatever).
 *
 *	\class		AABBSphereCollider
 *	\author		Pierre Terdiman
 *	\version	1.0
 *	\date		June, 2, 2001
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompiled Header
#include "Stdafx.h"

using namespace Opcode;

#include "OPC_SphereAABBOverlap.h"
#include "OPC_SphereTriOverlap.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Constructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AABBSphereCollider::AABBSphereCollider()
 :	mNbSphereBVTests	(0),
	mNbSpherePrimTests	(0),
	mUserData			(0),
	mObjCallback		(null),
	mFaces				(null)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Destructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AABBSphereCollider::~AABBSphereCollider()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Validates current settings. You should call this method after all the settings and callbacks have been defined.
 *	\return		null if everything is ok, else a string describing the problem
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* AABBSphereCollider::ValidateSettings()
{
	if(!mObjCallback)							return "Object callback must be defined! Call: SetCallbackObj().";
	if(!mFaces)									return "Destination array must be defined! Call: SetDestination().";
	if(mTemporalCoherence && !mFirstContact)	return "Temporal coherence only works with ""First contact"" mode!";

	return null;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Generic collision query for generic OPCODE models. After the call, access the results:
 *	- with GetContactStatus()
 *	- in the user-provided destination array
 *
 *	\param		sphere			[in] collision sphere in local space
 *	\param		model			[in] Opcode model to collide with
 *	\param		worlds			[in] sphere's world matrix
 *	\param		worldm			[in] model's world matrix
 *	\param		cache			[in] a possibly cached face index, or null
 *	\return		true if success
 *	\warning	SCALE NOT SUPPORTED. The matrices must contain rotation & translation parts only.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABBSphereCollider::Collide(const Sphere& sphere, OPCODE_Model* model, const Matrix4x4& worlds, const Matrix4x4& worldm, udword* cache)
{
	// Checkings
	if(!model)	return false;

	// Simple double-dispatch
	if(!model->HasLeafNodes())
	{
		if(model->IsQuantized())
		{
			const AABBQuantizedNoLeafTree* T = (const AABBQuantizedNoLeafTree*)model->GetTree();
			return Collide(sphere, T, worlds, worldm, cache);
		}
		else
		{
			const AABBNoLeafTree* T = (const AABBNoLeafTree*)model->GetTree();
			return Collide(sphere, T, worlds, worldm, cache);
		}
	}
	else
	{
		if(model->IsQuantized())
		{
			const AABBQuantizedTree* T = (const AABBQuantizedTree*)model->GetTree();
			return Collide(sphere, T, worlds, worldm, cache);
		}
		else
		{
			const AABBCollisionTree* T = (const AABBCollisionTree*)model->GetTree();
			return Collide(sphere, T, worlds, worldm, cache);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Initializes a collision query :
 *	- reset stats & contact status
 *	- setup matrices
 *
 *	\param		sphere		[in] sphere in local space
 *	\param		worlds		[in] sphere's world matrix
 *	\param		worldm		[in] model's world matrix
 *	\warning	SCALE NOT SUPPORTED. The matrices must contain rotation & translation parts only.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AABBSphereCollider::InitQuery(const Sphere& sphere, const Matrix4x4& worlds, const Matrix4x4& worldm)
{
	// Reset stats & contact status
	mContact			= false;
	mNbSphereBVTests	= 0;
	mNbSpherePrimTests	= 0;
	if(mFaces)	mFaces->Reset();

	// Compute sphere in model space

	// Setup matrices
	Matrix4x4 InvWorld1;
	InvertPRMatrix(InvWorld1, worldm);
	Matrix4x4 WorldStoM = worlds * InvWorld1;

	mCenter		= sphere.mCenter * WorldStoM;
	mRadius2	= sphere.mRadius * sphere.mRadius;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to take advantage of temporal coherence.
 *	\param		faceid		[in] index of previously collided triangle
 *	\warning	only works for "First Contact" mode
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABBSphereCollider::CheckTemporalCoherence(udword* faceid)
{
	// mObjCallback && mFaces have already been checked.

	// Test previously colliding primitives first
	if(mTemporalCoherence && mFirstContact && faceid && *faceid!=INVALID_ID)
	{
		// Request vertices from the app
		VertexPointers VP;	(mObjCallback)(*faceid, VP, mUserData);

		// Perform sphere-cached tri overlap
		static CollisionFace CF;
		CF.mFaceID = *faceid;
		if(SphereTriOverlap(*VP.Vertex[0], *VP.Vertex[1], *VP.Vertex[2], CF.mDistance, CF.mU, CF.mV))
		{
			// Set contact status
			mContact = true;

			mFaces->AddFace(CF);
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Collision query for normal trees.
 *	\param		sphere			[in] collision sphere in local space
 *	\param		tree			[in] model's AABB tree
 *	\param		worlds			[in] sphere's world matrix
 *	\param		worldm			[in] model's world matrix
 *	\param		cache			[in] a possibly cached face index, or null
 *	\return		true if success
 *	\warning	SCALE NOT SUPPORTED. The matrices must contain rotation & translation parts only.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABBSphereCollider::Collide(const Sphere& sphere, const AABBCollisionTree* tree, const Matrix4x4& worlds, const Matrix4x4& worldm, udword* cache)
{
	// Checkings
	if(!tree || !mObjCallback || !mFaces)	return false;

	// Init collision query
	InitQuery(sphere, worlds, worldm);

	// Check previous state
	if(CheckTemporalCoherence(cache))		return true;

	// Perform collision query
	_Collide(tree->GetNodes());

	// Update cache if needed
	if(cache && mContact)
	{
		CollisionFace* Current = mFaces->GetFaces();
		if(Current)	*cache	= Current->mFaceID;
		else		*cache	= INVALID_ID;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Collision query for no-leaf trees.
 *	\param		sphere			[in] collision sphere in local space
 *	\param		tree			[in] model's AABB tree
 *	\param		worlds			[in] sphere's world matrix
 *	\param		worldm			[in] model's world matrix
 *	\param		cache			[in] a possibly cached face index, or null
 *	\return		true if success
 *	\warning	SCALE NOT SUPPORTED. The matrices must contain rotation & translation parts only.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABBSphereCollider::Collide(const Sphere& sphere, const AABBNoLeafTree* tree, const Matrix4x4& worlds, const Matrix4x4& worldm, udword* cache)
{
	// Checkings
	if(!tree || !mObjCallback || !mFaces)	return false;

	// Init collision query
	InitQuery(sphere, worlds, worldm);

	// Check previous state
	if(CheckTemporalCoherence(cache))		return true;

	// Perform collision query
	_Collide(tree->GetNodes());

	// Update cache if needed
	if(cache && mContact)
	{
		CollisionFace* Current = mFaces->GetFaces();
		if(Current)	*cache	= Current->mFaceID;
		else		*cache	= INVALID_ID;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Collision query for quantized trees.
 *	\param		sphere			[in] collision sphere in local space
 *	\param		tree			[in] model's AABB tree
 *	\param		worlds			[in] sphere's world matrix
 *	\param		worldm			[in] model's world matrix
 *	\param		cache			[in] a possibly cached face index, or null
 *	\return		true if success
 *	\warning	SCALE NOT SUPPORTED. The matrices must contain rotation & translation parts only.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABBSphereCollider::Collide(const Sphere& sphere, const AABBQuantizedTree* tree, const Matrix4x4& worlds, const Matrix4x4& worldm, udword* cache)
{
	// Checkings
	if(!tree || !mObjCallback || !mFaces)	return false;

	// Init collision query
	InitQuery(sphere, worlds, worldm);

	// Check previous state
	if(CheckTemporalCoherence(cache))		return true;

	// Setup dequantization coeffs
	mCenterCoeff	= tree->mCenterCoeff;
	mExtentsCoeff	= tree->mExtentsCoeff;

	// Perform collision query
	_Collide(tree->GetNodes());

	// Update cache if needed
	if(cache && mContact)
	{
		CollisionFace* Current = mFaces->GetFaces();
		if(Current)	*cache	= Current->mFaceID;
		else		*cache	= INVALID_ID;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Collision query for quantized no-leaf trees.
 *	\param		sphere			[in] collision sphere in local space
 *	\param		tree			[in] model's AABB tree
 *	\param		worlds			[in] sphere's world matrix
 *	\param		worldm			[in] model's world matrix
 *	\param		cache			[in] a possibly cached face index, or null
 *	\return		true if success
 *	\warning	SCALE NOT SUPPORTED. The matrices must contain rotation & translation parts only.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABBSphereCollider::Collide(const Sphere& sphere, const AABBQuantizedNoLeafTree* tree, const Matrix4x4& worlds, const Matrix4x4& worldm, udword* cache)
{
	// Checkings
	if(!tree || !mObjCallback || !mFaces)	return false;

	// Init collision query
	InitQuery(sphere, worlds, worldm);

	// Check previous state
	if(CheckTemporalCoherence(cache))		return true;

	// Setup dequantization coeffs
	mCenterCoeff	= tree->mCenterCoeff;
	mExtentsCoeff	= tree->mExtentsCoeff;

	// Perform collision query
	_Collide(tree->GetNodes());

	// Update cache if needed
	if(cache && mContact)
	{
		CollisionFace* Current = mFaces->GetFaces();
		if(Current)	*cache	= Current->mFaceID;
		else		*cache	= INVALID_ID;
	}
	return true;
}

#define SPHERE_PRIM(prim)																			\
	/* Request vertices from the app */																\
	VertexPointers VP;	(mObjCallback)(prim, VP, mUserData);										\
																									\
	/* Perform sphere-tri overlap test and return */												\
	static CollisionFace CF;																		\
	CF.mFaceID = prim;																				\
	if(SphereTriOverlap(*VP.Vertex[0], *VP.Vertex[1], *VP.Vertex[2], CF.mDistance, CF.mU, CF.mV))	\
	{																								\
		/* Set contact status */																	\
		mContact = true;																			\
		mFaces->AddFace(CF);																		\
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive collision query for normal AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AABBSphereCollider::_Collide(const AABBCollisionNode* node)
{
	// Perform Sphere-AABB overlap test
	if(!SphereAABBOverlap(node->mAABB.mCenter, node->mAABB.mExtents))	return;

	if(node->IsLeaf())
	{
		SPHERE_PRIM(node->GetPrimitive())
	}
	else
	{
		_Collide(node->GetPos());
		if(mFirstContact && mContact) return;
		_Collide(node->GetNeg());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive collision query for quantized AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AABBSphereCollider::_Collide(const AABBQuantizedNode* node)
{
	// Dequantize box
	const QuantizedAABB* Box = &node->mAABB;
	const Point Center(float(Box->mCenter[0]) * mCenterCoeff.x, float(Box->mCenter[1]) * mCenterCoeff.y, float(Box->mCenter[2]) * mCenterCoeff.z);
	const Point Extents(float(Box->mExtents[0]) * mExtentsCoeff.x, float(Box->mExtents[1]) * mExtentsCoeff.y, float(Box->mExtents[2]) * mExtentsCoeff.z);

	// Perform Sphere-AABB overlap test
	if(!SphereAABBOverlap(Center, Extents))	return;

	if(node->IsLeaf())
	{
		SPHERE_PRIM(node->GetPrimitive())
	}
	else
	{
		_Collide(node->GetPos());
		if(mFirstContact && mContact) return;
		_Collide(node->GetNeg());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive collision query for no-leaf AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AABBSphereCollider::_Collide(const AABBNoLeafNode* node)
{
	// Perform Sphere-AABB overlap test
	if(!SphereAABBOverlap(node->mAABB.mCenter, node->mAABB.mExtents))	return;

	if(node->HasLeaf())
	{
		SPHERE_PRIM(node->GetPrimitive())
	}
	else _Collide(node->GetPos());

	if(mFirstContact && mContact) return;

	if(node->HasLeaf2())
	{
		SPHERE_PRIM(node->GetPrimitive2())
	}
	else _Collide(node->GetNeg());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive collision query for quantized no-leaf AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AABBSphereCollider::_Collide(const AABBQuantizedNoLeafNode* node)
{
	// Dequantize box
	const QuantizedAABB* Box = &node->mAABB;
	const Point Center(float(Box->mCenter[0]) * mCenterCoeff.x, float(Box->mCenter[1]) * mCenterCoeff.y, float(Box->mCenter[2]) * mCenterCoeff.z);
	const Point Extents(float(Box->mExtents[0]) * mExtentsCoeff.x, float(Box->mExtents[1]) * mExtentsCoeff.y, float(Box->mExtents[2]) * mExtentsCoeff.z);

	// Perform Sphere-AABB overlap test
	if(!SphereAABBOverlap(Center, Extents))	return;

	if(node->HasLeaf())
	{
		SPHERE_PRIM(node->GetPrimitive())
	}
	else _Collide(node->GetPos());

	if(mFirstContact && mContact) return;

	if(node->HasLeaf2())
	{
		SPHERE_PRIM(node->GetPrimitive2())
	}
	else _Collide(node->GetNeg());
}
