///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for a ray collider.
 *	\file		OPC_RayCollider.cpp
 *	\author		Pierre Terdiman
 *	\date		June, 2, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains an AABB ray collider.
 *	This class performs a stabbing query on an AABB tree, i.e. does a ray-mesh collision.
 *
 *	HIGHER DISTANCE BOUND:
 *
 *		If P0 and P1 are two 3D points, let's define:
 *		- d = distance between P0 and P1
 *		- Origin	= P0
 *		- Direction	= (P1 - P0) / d = normalized direction vector
 *		- A parameter t such as a point P on the line (P0,P1) is P = Origin + t * Direction
 *		- t = 0  -->  P = P0
 *		- t = d  -->  P = P1
 *
 *		Then we can define a general "ray" as:
 *
 *			struct Ray
 *			{
 *				Point	Origin;
 *				Point	Direction;
 *			};
 *
 *		But it actually maps three different things:
 *		- a segment,   when 0 <= t <= d
 *		- a half-line, when 0 <= t < +infinity, or -infinity < t <= d
 *		- a line,      when -infinity < t < +infinity
 *
 *		In Opcode, we support segment queries, which yield half-line queries by setting d = +infinity.
 *		We don't support line-queries. If you need them, shift the origin along the ray by an appropriate margin.
 *
 *		In short, the lower bound is always 0, and you can setup the higher bound "d" with AABBRayCollider::SetMaxDist().
 *
 *		Query	|segment			|half-line		|line
 *		--------|-------------------|---------------|----------------
 *		Usages	|-shadow feelers	|-raytracing	|-
 *				|-sweep tests		|-in/out tests	|
 *
 *		For true segment queries using (P0, P1) instead of (Origin, Dir), the ray-aabb overlap test *could* be replaced with
 *		a more efficient one. Here I'm thinking about the Volition one by Tim Schröder, which appears to be a bit faster in
 *		some cases, but sometimes a bit slower. YMMV. Note that Tim's code in GDMag can easily be optimized.
 *
 *	FIRST CONTACT:
 *
 *		- You can setup "first contact" mode or "all contacts" mode with AABBRayCollider::SetFirstContact().
 *		- In "first contact" mode we return as soon as the ray hits one face. If can be useful e.g. for shadow feelers, where
 *		you want to know whether the path to the light is free or not (a boolean answer is enough).
 *		- In "all contacts" mode we return all faces hit by the ray.
 *
 *	TEMPORAL COHERENCE:
 *
 *		- You can enable or disable temporal coherence with AABBRayCollider::SetTemporalCoherence().
 *		- It currently only works in "first contact" mode.
 *		- If temporal coherence is enabled, the previously hit triangle is cached during the first query. Then, next queries
 *		start by colliding the ray against the cached triangle. If they still colliden we return immediately.
 *
 *	CLOSEST HIT:
 *
 *		- You can enable or disable "closest hit" with AABBRayCollider::SetClosestHit().
 *		- It currently only works in "all contact" mode.
 *		- If closest hit is enabled, faces are sorted by distance on-the-fly and the closest one only is reported.
 *
 *	BACKFACE CULLING:
 *
 *		- You can enable or disable backface culling with AABBRayCollider::SetCulling().
 *		- If culling is enabled, ray will not hit back faces (only front faces).
 *		
 *
 *
 *	\class		AABBRayCollider
 *	\author		Pierre Terdiman
 *	\version	1.0
 *	\date		June, 2, 2001
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompiled Header
#include "Stdafx.h"

using namespace Opcode;

#include "OPC_RayAABBOverlap.h"
#include "OPC_RayTriOverlap.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Constructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AABBRayCollider::AABBRayCollider()
 :	mNbRayBVTests		(0),
	mNbRayPrimTests		(0),
	mNbIntersections	(0),
	mClosestHit			(false),
	mCulling			(true),
	mUserData			(0),
	mObjCallback		(null),
	mStabbedFaces		(null),
	mMaxDist			(MAX_FLOAT),
	mMaxDist2			(MAX_FLOAT)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Destructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AABBRayCollider::~AABBRayCollider()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Validates current settings. You should call this method after all the settings and callbacks have been defined.
 *	\return		null if everything is ok, else a string describing the problem
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* AABBRayCollider::ValidateSettings()
{
	if(!mObjCallback)							return "Object callback must be defined! Call: SetCallbackObj().";
	if(!mStabbedFaces)							return "Destination array must be defined! Call: SetDestination().";
	if(mMaxDist<0.0f)							return "Higher distance bound must be positive!";
	if(mTemporalCoherence && !mFirstContact)	return "Temporal coherence only works with ""First contact"" mode!";
	if(mClosestHit && mFirstContact)			return "Closest hit doesn't work with ""First contact"" mode!";
	return null;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Generic stabbing query for generic OPCODE models. After the call, access the results:
 *	- with GetContactStatus()
 *	- in the user-provided destination array
 *
 *	\param		worldray		[in] stabbing ray in world space
 *	\param		model			[in] Opcode model to collide with
 *	\param		world			[in] model's world matrix
 *	\param		cache			[in] a possibly cached face index, or null
 *	\return		true if success
 *	\warning	SCALE NOT SUPPORTED. The matrices must contain rotation & translation parts only.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABBRayCollider::Collide(const Ray& worldray, OPCODE_Model* model, const Matrix4x4& world, udword* cache)
{
	// Checkings
	if(!model)	return false;

	// Simple double-dispatch
	if(!model->HasLeafNodes())
	{
		if(model->IsQuantized())
		{
			const AABBQuantizedNoLeafTree* T = (const AABBQuantizedNoLeafTree*)model->GetTree();
			return Collide(worldray, T, world, cache);
		}
		else
		{
			const AABBNoLeafTree* T = (const AABBNoLeafTree*)model->GetTree();
			return Collide(worldray, T, world, cache);
		}
	}
	else
	{
		if(model->IsQuantized())
		{
			const AABBQuantizedTree* T = (const AABBQuantizedTree*)model->GetTree();
			return Collide(worldray, T, world, cache);
		}
		else
		{
			const AABBCollisionTree* T = (const AABBCollisionTree*)model->GetTree();
			return Collide(worldray, T, world, cache);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Initializes a stabbing query :
 *	- reset stats & contact status
 *	- compute ray in local space
 *
 *	\param		worldray		[in] stabbing ray in world space
 *	\param		world			[in] object's world matrix
 *	\warning	SCALE NOT SUPPORTED. The matrix must contain rotation & translation parts only.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AABBRayCollider::InitQuery(const Ray& worldray, const Matrix4x4& world)
{
	// Reset stats & contact status
	mContact			= false;
	mNbRayBVTests		= 0;
	mNbRayPrimTests		= 0;
	mNbIntersections	= 0;
	if(mStabbedFaces)	mStabbedFaces->Reset();

	// Compute ray in local space
	Matrix3x3 InvWorld = world;
	mDir = InvWorld * worldray.mDir;

	Matrix4x4 World;
	InvertPRMatrix(World, world);
	mOrigin = worldray.mOrig * World;

	// Precompute 1.0f / dir
	if(IR(mDir.x))	mOneOverDir.x = 1.0f / mDir.x;
	else			mOneOverDir.x = 0.0f;
	if(IR(mDir.y))	mOneOverDir.y = 1.0f / mDir.y;
	else			mOneOverDir.y = 0.0f;
	if(IR(mDir.z))	mOneOverDir.z = 1.0f / mDir.z;
	else			mOneOverDir.z = 0.0f;

	// Precompute mMaxDist2
	mMaxDist2 = mMaxDist * mMaxDist;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to take advantage of temporal coherence.
 *	\param		faceid		[in] index of previously stabbed triangle
 *	\warning	only works for "First Contact" mode
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABBRayCollider::CheckTemporalCoherence(udword* faceid)
{
	// mObjCallback && mStabbedFaces have already been checked.

	// Test previously colliding primitives first
	if(mTemporalCoherence && mFirstContact && faceid && *faceid!=INVALID_ID)
	{
		// Request vertices from the app
		VertexPointers VP;	(mObjCallback)(*faceid, VP, mUserData);

		// Perform ray-cached tri overlap
		static CollisionFace CF;
		CF.mFaceID = *faceid;
		if(RayTriOverlap(*VP.Vertex[0], *VP.Vertex[1], *VP.Vertex[2], CF.mDistance, CF.mU, CF.mV))
		{
			// Intersection point is valid if:
			// - distance is positive (else it can just be a face behind the orig point)
			// - distance is smaller than a given max distance (useful for shadow feelers)
			if(CF.mDistance>0.0f && CF.mDistance<mMaxDist)
			{
				// Set contact status
				mContact = true;

				mStabbedFaces->AddFace(CF);
				return true;
			}
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Stabbing query for normal trees.
 *	\param		worldray		[in] stabbing ray in world space
 *	\param		tree			[in] object's AABB tree
 *	\param		world			[in] object's world matrix
 *	\param		cache			[in] a possibly cached face index, or null
 *	\return		true if success
 *	\warning	SCALE NOT SUPPORTED. The matrix must contain rotation & translation parts only.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABBRayCollider::Collide(const Ray& worldray, const AABBCollisionTree* tree, const Matrix4x4& world, udword* cache)
{
	// Checkings
	if(!tree || !mObjCallback || !mStabbedFaces)	return false;

	// Init collision query
	InitQuery(worldray, world);

	// Check previous state
	if(CheckTemporalCoherence(cache))		return true;

	// Perform stabbing query
	_Stab(tree->GetNodes());

	// Update cache if needed
	if(cache && mContact)
	{
		CollisionFace* Current = mStabbedFaces->GetFaces();
		if(Current)	*cache	= Current->mFaceID;
		else		*cache	= INVALID_ID;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Stabbing query for no-leaf trees.
 *	\param		worldray		[in] stabbing ray in world space
 *	\param		tree			[in] object's AABB tree
 *	\param		world			[in] object's world matrix
 *	\param		cache			[in] a possibly cached face index, or null
 *	\return		true if success
 *	\warning	SCALE NOT SUPPORTED. The matrix must contain rotation & translation parts only.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABBRayCollider::Collide(const Ray& worldray, const AABBNoLeafTree* tree, const Matrix4x4& world, udword* cache)
{
	// Checkings
	if(!tree || !mObjCallback || !mStabbedFaces)	return false;

	// Init collision query
	InitQuery(worldray, world);

	// Check previous state
	if(CheckTemporalCoherence(cache))		return true;

	// Perform stabbing query
	_Stab(tree->GetNodes());

	// Update cache if needed
	if(cache && mContact)
	{
		CollisionFace* Current = mStabbedFaces->GetFaces();
		if(Current)	*cache	= Current->mFaceID;
		else		*cache	= INVALID_ID;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Stabbing query for quantized trees.
 *	\param		worldray		[in] stabbing ray in world space
 *	\param		tree			[in] object's AABB tree
 *	\param		world			[in] object's world matrix
 *	\param		cache			[in] a possibly cached face index, or null
 *	\return		true if success
 *	\warning	SCALE NOT SUPPORTED. The matrix must contain rotation & translation parts only.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABBRayCollider::Collide(const Ray& worldray, const AABBQuantizedTree* tree, const Matrix4x4& world, udword* cache)
{
	// Checkings
	if(!tree || !mObjCallback || !mStabbedFaces)	return false;

	// Init collision query
	InitQuery(worldray, world);

	// Check previous state
	if(CheckTemporalCoherence(cache))		return true;

	// Setup dequantization coeffs
	mCenterCoeff	= tree->mCenterCoeff;
	mExtentsCoeff	= tree->mExtentsCoeff;

	// Perform stabbing query
	_Stab(tree->GetNodes());

	// Update cache if needed
	if(cache && mContact)
	{
		CollisionFace* Current = mStabbedFaces->GetFaces();
		if(Current)	*cache	= Current->mFaceID;
		else		*cache	= INVALID_ID;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Stabbing query for quantized no-leaf trees.
 *	\param		worldray		[in] stabbing ray in world space
 *	\param		tree			[in] object's AABB tree
 *	\param		world			[in] object's world matrix
 *	\param		cache			[in] a possibly cached face index, or null
 *	\return		true if success
 *	\warning	SCALE NOT SUPPORTED. The matrix must contain rotation & translation parts only.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABBRayCollider::Collide(const Ray& worldray, const AABBQuantizedNoLeafTree* tree, const Matrix4x4& world, udword* cache)
{
	// Checkings
	if(!tree || !mObjCallback || !mStabbedFaces)	return false;

	// Init collision query
	InitQuery(worldray, world);

	// Check previous state
	if(CheckTemporalCoherence(cache))		return true;

	// Setup dequantization coeffs
	mCenterCoeff	= tree->mCenterCoeff;
	mExtentsCoeff	= tree->mExtentsCoeff;

	// Perform stabbing query
	_Stab(tree->GetNodes());

	// Update cache if needed
	if(cache && mContact)
	{
		CollisionFace* Current = mStabbedFaces->GetFaces();
		if(Current)	*cache	= Current->mFaceID;
		else		*cache	= INVALID_ID;
	}
	return true;
}

#define STAB_PRIM(prim)																			\
	/* Request vertices from the app */															\
	VertexPointers VP;	(mObjCallback)(prim, VP, mUserData);									\
																								\
	/* Perform ray-tri overlap test and return */												\
	static CollisionFace CF;																	\
	CF.mFaceID = prim;																			\
	if(RayTriOverlap(*VP.Vertex[0], *VP.Vertex[1], *VP.Vertex[2], CF.mDistance, CF.mU, CF.mV))	\
	{																							\
		/* Intersection point is valid if: */													\
		/* - distance is positive (else it can just be a face behind the orig point) */			\
		/* - distance is smaller than a given max distance (useful for shadow feelers) */		\
		if(CF.mDistance>0.0f)																	\
		{																						\
			mNbIntersections++;																	\
			if(CF.mDistance<mMaxDist)															\
			{																					\
				/* Set contact status */														\
				mContact = true;																\
																								\
				if(!mClosestHit)																\
				{																				\
					mStabbedFaces->AddFace(CF);													\
				}																				\
				else																			\
				{																				\
					CollisionFace* Current = mStabbedFaces->GetFaces();							\
					if(!Current)	mStabbedFaces->AddFace(CF);									\
					else if(CF.mDistance<Current->mDistance)	*Current = CF;					\
				}																				\
			}																					\
		}																						\
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive stabbing query for normal AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AABBRayCollider::_Stab(const AABBCollisionNode* node)
{
	// Perform Ray-AABB overlap test
	Point Coords;
	if(!RayAABBOverlap(node->mAABB.mCenter, node->mAABB.mExtents, Coords))	return;

	// Test distance to box, so that we can stop the query earlier when the box is out of range.
	// Avoid the test when distance is max
	if(IR(mMaxDist)!=IEEE_MAX_FLOAT)
	{
		float Dist2 = Coords.SquareDistance(mOrigin);
		if(Dist2 > mMaxDist2)	return;
	}

	if(node->IsLeaf())
	{
		STAB_PRIM(node->GetPrimitive())
	}
	else
	{
		_Stab(node->GetPos());
		if(mFirstContact && mContact) return;
		_Stab(node->GetNeg());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive stabbing query for quantized AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AABBRayCollider::_Stab(const AABBQuantizedNode* node)
{
	// Dequantize box
	const QuantizedAABB* Box = &node->mAABB;
	const Point Center(float(Box->mCenter[0]) * mCenterCoeff.x, float(Box->mCenter[1]) * mCenterCoeff.y, float(Box->mCenter[2]) * mCenterCoeff.z);
	const Point Extents(float(Box->mExtents[0]) * mExtentsCoeff.x, float(Box->mExtents[1]) * mExtentsCoeff.y, float(Box->mExtents[2]) * mExtentsCoeff.z);

	// Perform Ray-AABB overlap test
	Point Coords;
	if(!RayAABBOverlap(Center, Extents, Coords))	return;

	// Test distance to box, so that we can stop the query earlier when the box is out of range.
	// Avoid the test when distance is max
	if(IR(mMaxDist)!=IEEE_MAX_FLOAT)
	{
		float Dist2 = Coords.SquareDistance(mOrigin);
		if(Dist2 > mMaxDist2)	return;
	}

	if(node->IsLeaf())
	{
		STAB_PRIM(node->GetPrimitive())
	}
	else
	{
		_Stab(node->GetPos());
		if(mFirstContact && mContact) return;
		_Stab(node->GetNeg());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive stabbing query for no-leaf AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AABBRayCollider::_Stab(const AABBNoLeafNode* node)
{
	// Perform Ray-AABB overlap test
	Point Coords;
	if(!RayAABBOverlap(node->mAABB.mCenter, node->mAABB.mExtents, Coords))	return;

	// Test distance to box, so that we can stop the query earlier when the box is out of range.
	// Avoid the test when distance is max
	if(IR(mMaxDist)!=IEEE_MAX_FLOAT)
	{
		float Dist2 = Coords.SquareDistance(mOrigin);
		if(Dist2 > mMaxDist2)	return;
	}

	if(node->HasLeaf())
	{
		STAB_PRIM(node->GetPrimitive())
	}
	else _Stab(node->GetPos());

	if(mFirstContact && mContact) return;

	if(node->HasLeaf2())
	{
		STAB_PRIM(node->GetPrimitive2())
	}
	else _Stab(node->GetNeg());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive stabbing query for quantized no-leaf AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AABBRayCollider::_Stab(const AABBQuantizedNoLeafNode* node)
{
	// Dequantize box
	const QuantizedAABB* Box = &node->mAABB;
	const Point Center(float(Box->mCenter[0]) * mCenterCoeff.x, float(Box->mCenter[1]) * mCenterCoeff.y, float(Box->mCenter[2]) * mCenterCoeff.z);
	const Point Extents(float(Box->mExtents[0]) * mExtentsCoeff.x, float(Box->mExtents[1]) * mExtentsCoeff.y, float(Box->mExtents[2]) * mExtentsCoeff.z);

	// Perform Ray-AABB overlap test
	Point Coords;
	if(!RayAABBOverlap(Center, Extents, Coords))	return;

	// Test distance to box, so that we can stop the query earlier when the box is out of range.
	// Avoid the test when distance is max
	if(IR(mMaxDist)!=IEEE_MAX_FLOAT)
	{
		float Dist2 = Coords.SquareDistance(mOrigin);
		if(Dist2 > mMaxDist2)	return;
	}

	if(node->HasLeaf())
	{
		STAB_PRIM(node->GetPrimitive())
	}
	else _Stab(node->GetPos());

	if(mFirstContact && mContact) return;

	if(node->HasLeaf2())
	{
		STAB_PRIM(node->GetPrimitive2())
	}
	else _Stab(node->GetNeg());
}
