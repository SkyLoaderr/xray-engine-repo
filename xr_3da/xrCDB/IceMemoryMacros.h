///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains all memory macros.
 *	\file		IceMemoryMacros.h
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEMEMORYMACROS_H__
#define __ICEMEMORYMACROS_H__

#undef ZeroMemory
#undef CopyMemory
#undef MoveMemory
#undef FillMemory

//!	A function to clear a buffer.
//!	\param	addr		buffer address
//!	\param	size		buffer length
//!	\see	FillMemory
//!	\see	CopyMemory
//!	\see	MoveMemory
ICECORE_API __forceinline void ZeroMemory(void* addr, udword size)					{ memset(addr, 0, size);		}

//!	A function to fill a buffer with a given byte.
//!	\param	addr		buffer address
//!	\param	size		buffer length
//!	\param	val			the byte value
//!	\see	ZeroMemory
//!	\see	CopyMemory
//!	\see	MoveMemory
ICECORE_API __forceinline void FillMemory(void* dest, udword size, ubyte val)		{ memset(dest, val, size);		}

//!	A function to copy a buffer.
//!	\param	addr		destination buffer address
//!	\param	addr		source buffer address
//!	\param	size		buffer length
//!	\see	ZeroMemory
//!	\see	FillMemory
//!	\see	MoveMemory
ICECORE_API __forceinline void CopyMemory(void* dest, const void* src, udword size)	{ memcpy(dest, src, size);		}

//!	A function to move a buffer.
//!	\param	addr		destination buffer address
//!	\param	addr		source buffer address
//!	\param	size		buffer length
//!	\see	ZeroMemory
//!	\see	FillMemory
//!	\see	CopyMemory
ICECORE_API __forceinline void MoveMemory(void* dest, const void* src, udword size)	{ memmove(dest, src, size);		}

#define SIZEOFOBJECT		sizeof(*this)									//!< Gives the size of current object. Avoid some mistakes (e.g. "sizeof(this)").
//#define CLEAROBJECT		{ memset(this, 0, SIZEOFOBJECT);	}			//!< Clears current object. Laziness is my business. HANDLE WITH CARE.
#define DELETESINGLE(x)		if (x) { delete x;				x = null; }		//!< Deletes an instance of a class.
#define DELETEARRAY(x)		if (x) { delete []x;			x = null; }		//!< Deletes an array.
#define SAFE_RELEASE(x)		if (x) { (x)->Release();		(x) = null; }	//!< Safe D3D-style release
#define SAFE_DESTRUCT(x)	if (x) { (x)->SelfDestruct();	(x) = null; }	//!< Safe ICE-style release
#define CHECKALLOC(x)		if(!x) return SetIceError("Out of memory.", EC_OUTOFMEMORY);	//!< Standard alloc checking. HANDLE WITH CARE.

#endif // __ICEMEMORYMACROS_H__
