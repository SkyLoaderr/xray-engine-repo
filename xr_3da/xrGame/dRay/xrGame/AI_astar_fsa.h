/* 

  FixedSizeAllocator class
  Copyright 2001 Justin Heyes-Jones
  Copyright 2001 Oles Shishkovtsov

  This class is a constant time O(1) memory manager for objects of 
  a specified type. The type is specified using a template class.

  Memory is allocated from a fixed size buffer which you can specify in the
  class constructor or use the default.

  Using GetFirst and GetNext it is possible to iterate through the elements
  one by one, and this would be the most common use for the class. 

  I would suggest using this class when you want O(1) add and delete
  and you don't do much searching, which would be O(n). Structures such as binary 
  trees can be used to get O(logn) access time.

*/

#ifndef FSA_H
#define FSA_H

template <class USER_TYPE> class FixedSizeAllocator
{

public: 
	// Constants
	enum
	{
		FSA_DEFAULT_SIZE = 4096*8
	};

	// This class enables us to transparently manage the extra data 
	// needed to enable the user class to form part of the double-linked
	// list class
	struct FSA_ELEMENT
	{
		USER_TYPE UserType;
		
		FSA_ELEMENT *pPrevFree;
		FSA_ELEMENT *pNextFree;
	
		FSA_ELEMENT *pPrevUsed;
		FSA_ELEMENT *pNextUsed;
	};

public: // methods
	FixedSizeAllocator	( u32 MaxElements = FSA_DEFAULT_SIZE ) :
	m_MaxElements		( MaxElements ),
	m_pFirstUsed		( NULL )
	{
		// Allocate enough memory for the maximum number of elements
		m_pMemory		= new FSA_ELEMENT[ m_MaxElements ];
		m_pFirstFree	= m_pMemory;	// Set the free list first pointer

		// Clear the memory
		ZeroMemory( m_pMemory, sizeof( FSA_ELEMENT ) * m_MaxElements );

		// Point at first element
		FSA_ELEMENT *pElement = m_pFirstFree;

		// Set the double linked free list
		for( u32 i=0; i<m_MaxElements; i++ )
		{
			pElement->pPrevFree = pElement-1;
			pElement->pNextFree = pElement+1;

			pElement++;
		}

		// first element should have a null prev
		m_pFirstFree->pPrevFree = NULL;
		// last element should have a null next
		(pElement-1)->pNextFree = NULL;
	}


	~FixedSizeAllocator()
	{
		// Free up the memory
		delete [] m_pMemory;
	}

	// Allocate a new USER_TYPE and return a pointer to it 
	USER_TYPE *alloc()
	{

		FSA_ELEMENT *pNewNode = NULL;

		if( !m_pFirstFree )
		{
			return NULL;
		}
		else
		{

			pNewNode = m_pFirstFree;
			m_pFirstFree = pNewNode->pNextFree;

			//maintain free list
			if( pNewNode->pNextFree )
			{
				pNewNode->pNextFree->pPrevFree = NULL;
			}
			else
			{
				// not sure if this happens
			}

			// Make sure no information remains in the freelist
			pNewNode->pNextFree = NULL;
			pNewNode->pPrevFree = NULL;

			//maintain used list

			pNewNode->pPrevUsed = NULL; // the allocated node is always first in the list

			if( m_pFirstUsed == NULL )
			{
				pNewNode->pNextUsed = NULL;
			}
			else
			{
				m_pFirstUsed->pPrevUsed = pNewNode;
				pNewNode->pNextUsed = m_pFirstUsed;
			}

			m_pFirstUsed = pNewNode;
		}	
		
		return reinterpret_cast<USER_TYPE*>(pNewNode);
	}

	// Free the given user type
	// For efficiency I don't check whether the user_data is a valid
	// pointer that was allocated. I may add some debug only checking
	void release ( USER_TYPE* user_data )
	{

		FSA_ELEMENT *pNode = reinterpret_cast<FSA_ELEMENT*>(user_data);

		// invalidate free list pointers
		pNode->pNextFree = NULL;
		pNode->pPrevFree = NULL;

		// manage free list
		if( m_pFirstFree == NULL ) 
		{
			// free list was empty
			m_pFirstFree = pNode;
		}
		else
		{
			// Add this node to the start of the free list
			m_pFirstFree->pPrevFree = pNode;
			pNode->pNextFree = m_pFirstFree;
			m_pFirstFree = pNode;
		}

		// manage used list, remove this node from it
		if( pNode->pPrevUsed )
		{
			pNode->pPrevUsed->pNextUsed = pNode->pNextUsed;
		}
		else
		{
			// this handles the case that we delete the first node in the used list
			// the firstused pointer needs to be handled
			m_pFirstUsed = pNode->pNextUsed;
		}

		if( pNode->pNextUsed )
		{
			pNode->pNextUsed->pPrevUsed = pNode->pPrevUsed;
		}

		// invalidate used list pointers
		pNode->pNextUsed = NULL; 
		pNode->pPrevUsed = NULL;

	}

	// Iterators
	USER_TYPE *GetFirst()
	{
		return reinterpret_cast<USER_TYPE *>(m_pFirstUsed);
	}

	USER_TYPE *GetNext( USER_TYPE *node )
	{
		return reinterpret_cast<USER_TYPE *>
			(
				(reinterpret_cast<FSA_ELEMENT *>(node))->pNextUsed
			);
	}

private: // data
	FSA_ELEMENT *	m_pFirstFree;
	FSA_ELEMENT *	m_pFirstUsed;
	u32				m_MaxElements;
	FSA_ELEMENT *	m_pMemory;
};

#endif // defined FSA_H
