///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains common classes & defs used in OPCODE.
 *	\file		OPC_Common.h
 *	\author		Pierre Terdiman
 *	\date		March, 20, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __OPC_COMMON_H__
#define __OPC_COMMON_H__

	struct VertexPointers
	{
		const Point*	Vertex[3];
	};

	//! Request vertices
	typedef void	(*OPC_CALLBACK)	(udword triangleindex, VertexPointers& triangle, udword userdata);

	class OPCODE_API CollisionAABB
	{
		public:
		//! Constructor
		__forceinline				CollisionAABB()			{}
		//! Destructor
		__forceinline				~CollisionAABB()		{}

						Point		mCenter;				//!< Box center
						Point		mExtents;				//!< Box extents
	};

	class OPCODE_API QuantizedAABB
	{
		public:
		//! Constructor
		__forceinline				QuantizedAABB()			{}
		//! Destructor
		__forceinline				~QuantizedAABB()		{}

						sword		mCenter[3];				//!< Quantized center
						uword		mExtents[3];			//!< Quantized extents
	};

	class OPCODE_API CollisionFace
	{
		public:
		//! Constructor
		__forceinline				CollisionFace()			{}
		//! Destructor
		__forceinline				~CollisionFace()		{}

						udword		mFaceID;				//!< Index of touched face
						float		mDistance;				//!< Distance from collider to hitpoint
						float		mU, mV;					//!< Impact barycentric coordinates
	};

	class OPCODE_API CollisionFaces : private Container
	{
		public:
		//! Constructor
		__forceinline					CollisionFaces()						{}
		//! Destructor
		__forceinline					~CollisionFaces()						{}

		__forceinline	udword			GetNbFaces()					const	{ return GetNbEntries()>>2;				}
		__forceinline	CollisionFace*	GetFaces()						const	{ return (CollisionFace*)GetEntries();	}

		__forceinline	void			Reset()									{ Container::Reset();					}

						void			AddFace(const CollisionFace& face)		{ Add(face.mFaceID).Add(face.mDistance).Add(face.mU).Add(face.mV);	}
	};

#endif //__OPC_COMMON_H__