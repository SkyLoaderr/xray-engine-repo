///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a simple container class.
 *	\file		IceContainer.h
 *	\author		Pierre Terdiman
 *	\date		February, 5, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICECONTAINER_H__
#define __ICECONTAINER_H__

	#define CONTAINER_STATS

	class ICECORE_API Container
	{
		public:
		// Constructor / Destructor
										Container();
										Container(udword size, float growthfactor);
										~Container();
		// Management
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A O(1) method to add a value in the container. The container is automatically resized if needed.
		 *	The method is inline, not the resize. The call overhead happens on resizes only, which is not a problem since the resizing operation
		 *	costs a lot more than the call overhead...
		 *
		 *	\param		entry		[in] a udword to store in the container
		 *	\see		Add(float entry)
		 *	\see		Empty()
		 *	\see		Contains(udword entry)
		 *	\return		Self-Reference
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__forceinline	Container&		Add(udword entry)
						{
							// Resize if needed
							if(mCurNbEntries==mMaxNbEntries)	Resize();

							// Add new entry
							mEntries[mCurNbEntries++]	= entry;
							return *this;
						}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A O(1) method to add a value in the container. The container is automatically resized if needed.
		 *	The method is inline, not the resize. The call overhead happens on resizes only, which is not a problem since the resizing operation
		 *	costs a lot more than the call overhead...
		 *
		 *	\param		entry		[in] a float to store in the container
		 *	\see		Add(udword entry)
		 *	\see		Empty()
		 *	\see		Contains(udword entry)
		 *	\return		Self-Reference
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__forceinline	Container&		Add(float entry)
						{
							// Resize if needed
							if(mCurNbEntries==mMaxNbEntries)	Resize();

							// Add new entry
							mEntries[mCurNbEntries++]	= IR(entry);
							return *this;
						}

		//! Add unique [slow]
						Container&		AddUnique(udword entry)
						{
							if(!Contains(entry))	Add(entry);
							return *this;
						}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A method to clear the container. All stored values are deleted, and it frees used ram.
		 *	\see		Reset()
		 *	\return		Self-Reference
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__forceinline	Container&		Empty()
						{
							#ifdef CONTAINER_STATS
							mUsedRam-=mMaxNbEntries*sizeof(udword);
							#endif
							DELETEARRAY(mEntries);
							mCurNbEntries = mMaxNbEntries = 0;
							return *this;
						}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A method to reset the container. Stored values are discarded but the buffer is kept so that further calls don't need resizing again.
		 *	That's a kind of temporal coherence.
		 *	\see		Empty()
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__forceinline	void			Reset()
						{
							// Avoid the write if possible
							// ### CMOV
							if(mCurNbEntries)	mCurNbEntries = 0;
						}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A method to set the initial size of the container. If it already contains something, it's discarded.
		 *	\param		nb		[in] Number of entries
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						bool			SetSize(udword nb);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A method to refit the container and get rid of unused bytes.
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						bool			Refit();

		// A method to check whether the container already contains a given value.
						bool			Contains(udword entry, udword* location=null) const;
		// A method to delete an entry - doesn't preserve insertion order.
						bool			Delete(udword entry);
		// A method to delete an entry - does preserve insertion order.
						bool			DeleteKeepingOrder(udword entry);
		//! A method to delete the very last entry.
		__forceinline	void			DeleteLastEntry()						{ if(mCurNbEntries)	mCurNbEntries--;			}
		//! A method to delete the entry whose index is given
		__forceinline	void			DeleteIndex(udword index)				{ mEntries[index] = mEntries[--mCurNbEntries];	}

		// Helpers
						Container&		FindNext(udword& entry, bool wrap=false);
						Container&		FindPrev(udword& entry, bool wrap=false);
		// Data access.
		__forceinline	udword			GetNbEntries()					const	{ return mCurNbEntries;		}	//!< Returns the current number of entries.
		__forceinline	udword			GetEntry(udword i)				const	{ return mEntries[i];		}	//!< Returns ith entry
		__forceinline	udword*			GetEntries()					const	{ return mEntries;			}	//!< Returns the list of entries.

		// Growth control
		__forceinline	float			GetGrowthFactor()				const	{ return mGrowthFactor;		}	//!< Returns the growth factor
		__forceinline	void			SetGrowthFactor(float growth)			{ mGrowthFactor = growth;	}	//!< Sets the growth factor

		//! Access as an array
		__forceinline	udword&			operator[](udword i)			const	{ ASSERT(i>=0 && i<mCurNbEntries); return mEntries[i];	}

		// Stats
						udword			GetUsedRam()					const;

		//! Operator for Container A = Container B
						void			operator = (const Container& object)
						{
							SetSize(object.GetNbEntries());
							CopyMemory(mEntries, object.GetEntries(), mMaxNbEntries*sizeof(udword));
							mCurNbEntries = mMaxNbEntries;
						}

#ifdef CONTAINER_STATS
		__forceinline	udword			GetNbContainers()				const	{ return mNbContainers;		}
		__forceinline	udword			GetTotalBytes()					const	{ return mUsedRam;			}
		private:

		static			udword			mNbContainers;		//!< Number of containers around
		static			udword			mUsedRam;			//!< Amount of bytes used by containers in the system
#endif
		private:
		// Resizing
						bool			Resize();
		// Data
						udword			mMaxNbEntries;		//!< Maximum possible number of entries
						udword			mCurNbEntries;		//!< Current number of entries
						udword*			mEntries;			//!< List of entries
						float			mGrowthFactor;		//!< Resize: new number of entries = old number * mGrowthFactor
	};

#endif // __ICECONTAINER_H__
