/* Copyright (C) Tom Forsyth, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tom Forsyth, 2001"
 */
// Some standard useful classes, templates, etc.


#ifndef ArbitraryListH
#define ArbitraryListH


// An arbitrary-sized list template class.
// Designed to hold _unsorted_ data, but only RemoveItem()
// actually disturbs the order, so you can use it for general arrays
// if you don't use that function.
template <class T>
class ArbitraryList
{
	T		*pT;				// The list.
	int		iSize;				// The current size of the list.
	int		iReservedSize;		// The current reserved size of the list.


public:

	// Constructor, with optional initial size setting.
	ArbitraryList ( int iInitialSize = 0 )
	{
		pT = NULL;
		iSize = 0;
		iReservedSize = 0;
		if ( iInitialSize > 0 )
		{
			SizeTo ( iInitialSize );
		}
	}

	// Destructor.
	~ArbitraryList ( void )
	{
		if ( pT == NULL )
		{
			VERIFY ( iReservedSize == 0 );
			VERIFY ( iSize == 0 );
		}
		else
		{
			VERIFY ( iReservedSize > 0 );
			VERIFY ( iSize > 0 );
			delete[] pT;
			pT = NULL;
		}
	}

	// Returns the pointer to the given item.
	T *Item ( int iItem )
	{
		VERIFY ( iItem < iSize );
		return ( &pT[iItem] );
	}

	// Returns the pointer to the first item.
	T *Ptr ( void )
	{
		return ( pT );
	}

	// Returns the size of the list
	int Size ( void )
	{
		return iSize;
	}

	// Grows or shrinks the list to this number of items.
	// Preserves existing items.
	// Items that fall off the end of a shrink may vanish.
	// Returns the pointer to the first item.
	T *SizeTo ( int iNum )
	{
		VERIFY ( iNum >= 0 );
		iSize = iNum;
		if ( iNum <= iReservedSize )
		{
			if ( iNum == 0 )
			{
				// Shrunk to 0 - bin the memory.
				delete[] pT;
				pT = NULL;
				iReservedSize = 0;
			}
			else
			{
				VERIFY ( pT != NULL );
			}
			return ( pT );
		}
		else
		{
			// Need to grow. Grow by 50% more than
			// needed to avoid constant regrows.
			int iNewSize = ( iNum * 3 ) >> 1;
			if ( pT == NULL )
			{
				VERIFY ( iReservedSize == 0 );
				pT = new T [iNewSize];
			}
			else
			{
				VERIFY ( iReservedSize != 0 );

				T *pOldT = pT;
				pT = new T[iNewSize];
				for ( int i = 0; i < iReservedSize; i++ )
				{
					pT[i] = pOldT[i];
				}
				delete[] pOldT;
			}
			VERIFY ( pT != NULL );
			iReservedSize = iNewSize;
			return ( pT );
		}
	}

	// Adds one item to the list and returns a pointer to that new item.
	T *AddItem ( void )
	{
		SizeTo ( iSize + 1 );
		return ( &pT[iSize-1] );
	}

	// Removes the given item number by copying the last item
	// to that position and shrinking the list.
	void RemoveItem ( int iItemNumber )
	{
		VERIFY ( iItemNumber < iSize );
		pT[iItemNumber] = pT[iSize-1];
		SizeTo ( iSize - 1 );
	}

	// Copy the specified data into the list.
	void CopyFrom ( int iFirstItem, T *p, int iNumItems )
	{
		for ( int i = 0; i < iNumItems; i++ )
		{
			*(Item ( i + iFirstItem ) ) = p[i];
		}
	}

	// A copy from another arbitrary list of the same type.
	void CopyFrom ( int iFirstItem, ArbitraryList<T> &other, int iFirstOtherItem, int iNumItems )
	{
		for ( int i = 0; i < iNumItems; i++ )
		{
			*(Item ( i + iFirstItem ) ) = *(other.Item ( i + iFirstOtherItem ) );
		}
	}


	// Copy constructor.
	ArbitraryList ( const ArbitraryList<T> &other )
	{
		int iNumItems = other.Size();

		pT = NULL;
		iSize = 0;
		iReservedSize = 0;
		if ( iInitialSize > 0 )
		{
			SizeTo ( iNumItems );
		}
		for ( int i = 0; i < iNumItems; i++ )
		{
			*(Item(i) ) = *(other.Item(i) );
		}
	}
};

#endif //#ifndef ArbitraryListH
