///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for tree builders.
 *	\file		OPC_TreeBuilders.h
 *	\author		Pierre Terdiman
 *	\date		March, 20, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __OPC_TREEBUILDERS_H__
#define __OPC_TREEBUILDERS_H__

	//! Tree splitting rules
	enum SplittingRules
	{
		SPLIT_COMPLETE			= (1<<0),		//!< Build a complete tree (2*N-1 nodes)
		SPLIT_LARGESTAXIS		= (1<<1),		//!< Split along the largest axis
		SPLIT_SPLATTERPOINTS	= (1<<2),		//!< Splatter primitive centers (QuickCD-style)
		SPLIT_BESTAXIS			= (1<<3),		//!< Try largest axis, then second, then last
		SPLIT_BALANCED			= (1<<4),		//!< Try to keep a well-balanced tree
		SPLIT_FIFTY				= (1<<5),		//!< Arbitrary 50-50 split
		SPLIT_FORCE_DWORD		= 0x7fffffff
	};

	class OPCODE_API AABBTreeBuilder
	{
		public:
		//! Constructor
											AABBTreeBuilder() :
												mLimit(0),
												mRules(SPLIT_FORCE_DWORD),
												mCount(0),
												mNbPrimitives(0)		{}
		//! Destructor
		virtual								~AABBTreeBuilder()			{}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Computes the AABB of a set of primitives.
		 *	\param		primitives		[in] list of indices of primitives
		 *	\param		nbprims			[in] number of indices
		 *	\param		globalbox		[out] global AABB enclosing the set of input primitives
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual			bool				ComputeGlobalBox(udword* primitives, udword nbprims, AABB& globalbox) const	= 0;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Computes the splitting value along a given axis for a given primitive.
		 *	\param		index			[in] index of the primitive to split
		 *	\param		nbprims			[in] axis index (0,1,2)
		 *	\return		splitting value
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual			float				GetSplittingValue(udword index, udword axis) const							= 0;

						udword				mLimit;				//!< Limit number of primitives / node
						udword				mRules;				//!< Building/Splitting rules (a combination of flags)
						udword				mNbPrimitives;		//!< Total number of primitives.
		// Stats
		__forceinline	void				SetCount(udword nb)			{ mCount=nb;			}
		__forceinline	void				IncreaseCount(udword nb)	{ mCount+=nb;			}
		__forceinline	udword				GetCount()			const	{ return mCount;		}

		private:
						udword				mCount;				//!< Stats: number of nodes created
	};

	class OPCODE_API AABBTreeOfAABBsBuilder : public AABBTreeBuilder
	{
		public:
		//! Constructor
												AABBTreeOfAABBsBuilder() : mAABBList(null)	{}
		//! Destructor
		virtual									~AABBTreeOfAABBsBuilder()					{}

		override(AABBTreeBuilder)	bool		ComputeGlobalBox(udword* primitives, udword nbprims, AABB& globalbox) const;
		override(AABBTreeBuilder)	float		GetSplittingValue(udword index, udword axis) const;

		const						AABB*		mAABBList;			//!< Shortcut to an app-controlled list of AABBs.
	};

	class OPCODE_API AABBTreeOfTrianglesBuilder : public AABBTreeBuilder
	{
		public:
		//! Constructor
												AABBTreeOfTrianglesBuilder() : mTriList(null), mNbTriangles(0)	{}
		//! Destructor
		virtual									~AABBTreeOfTrianglesBuilder()									{}

		override(AABBTreeBuilder)	bool		ComputeGlobalBox(udword* primitives, udword nbprims, AABB& globalbox) const;
		override(AABBTreeBuilder)	float		GetSplittingValue(udword index, udword axis) const;

		const						Triangle*	mTriList;			//!< Shortcut to an app-controlled list of triangles.
		const						Point*		mVerts;				//!< Shortcut to an app-controlled list of vertices.
		const						udword		mNbTriangles;		//!< Total number of triangles.
	};

#endif // __OPC_TREEBUILDERS_H__
