// SharedPrimitive.h: interface for the CSharedPrimitive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAREDPRIMITIVE_H__22244801_1B1B_47AE_976E_42A3325CF063__INCLUDED_)
#define AFX_SHAREDPRIMITIVE_H__22244801_1B1B_47AE_976E_42A3325CF063__INCLUDED_
#pragma once

enum
{
	LOCKFLAGS_FLUSH		= /* D3DLOCK_NOSYSLOCK | */ D3DLOCK_DISCARD,
	LOCKFLAGS_APPEND	= /* D3DLOCK_NOSYSLOCK | */ D3DLOCK_NOOVERWRITE
};

class ENGINE_API CVertexStream  
{
	friend class				CSharedStreams;
	friend class				CDraw;
	friend class				CRender;
private :
	IDirect3DVertexBuffer8*		pVB;
	DWORD						mSize;			// size in bytes
	DWORD						mPosition;		// position in bytes
	DWORD						mDiscardID;		// ID of discard - usually for caching
private:
	void						_clear	()
	{ ZeroMemory	(this,sizeof(*this)); }
	void						Create	();
	void						Destroy	();
public:
	IC IDirect3DVertexBuffer8*	getBuffer()		{ return pVB;			}
	IC DWORD					getDiscard()	{ return mDiscardID;	}
	IC void						Flush()			{ mPosition=mSize;		}

	IC void*					Lock			( DWORD Count, DWORD Stride, DWORD& vOffset )
	{
		vOffset				= 0;
		BYTE* pLockedData	= 0;
		
		// Ensure there is enough space in the VB for this data
		DWORD	bytes_need	= Count*Stride;
		R_ASSERT			(bytes_need<=mSize);

		// Align position to stride and calc offset
		vOffset				= (mPosition/Stride)+1;
		mPosition			= vOffset*Stride;

		// If either user forced us to flush,
		// or there is not enough space for the vertex data,
		// then flush the buffer contents
		DWORD dwFlags		= LOCKFLAGS_APPEND;
		if ( ( bytes_need + mPosition ) >= mSize )
		{
			mPosition			= 0;						// clear position
			dwFlags				= LOCKFLAGS_FLUSH;			// discard it's contens
			mDiscardID			++;
		}

		pVB->Lock			( mPosition, bytes_need, &pLockedData, dwFlags);
		VERIFY				(pLockedData);

		return LPVOID		(pLockedData);
	}

	IC void						Unlock			( DWORD Count, DWORD Stride)
	{
		mPosition			+=	Count*Stride;

		VERIFY				(pVB);
		pVB->Unlock			();
	}

	CVertexStream()			{ _clear();				};
	~CVertexStream()		{ Destroy();			};
};

class ENGINE_API CIndexStream  
{
	friend class				CSharedStreams;
	friend class				CDraw;
	friend class				CRender;
private :
	IDirect3DIndexBuffer8*		pIB;
	DWORD						mSize;		// real size (usually mCount, aligned on 512b boundary)
	DWORD						mPosition;
	DWORD						mDiscardID;
private:
	void						_clear	()
	{ ZeroMemory				(this,sizeof(*this)); }
	void						Create	();
	void						Destroy	();
public:
	IC IDirect3DIndexBuffer8*	getBuffer()		{ return pIB;			}
	IC DWORD					getDiscard()	{ return mDiscardID;	}
	void						Flush()			{ mPosition=mSize;		}

	IC WORD*					Lock			( DWORD Count, DWORD& vOffset )
	{
		vOffset					= 0;
		BYTE* pLockedData		= 0;
		
		// Ensure there is enough space in the VB for this data
		R_ASSERT				(2*Count<=mSize);

		// If either user forced us to flush,
		// or there is not enough space for the index data,
		// then flush the buffer contents
		DWORD dwFlags = LOCKFLAGS_APPEND;
		if ( 2*( Count + mPosition ) >= mSize )
		{
			mPosition	= 0;						// clear position
			dwFlags		= LOCKFLAGS_FLUSH;			// discard it's contens
			mDiscardID	++;
		}
		pIB->Lock				( mPosition * 2, Count * 2, &pLockedData, dwFlags);
		VERIFY					(pLockedData);
		
		vOffset					=	mPosition;

		return					LPWORD(pLockedData);
	}
	IC void						Unlock(DWORD RealCount)
	{
		mPosition				+=	RealCount;
		VERIFY					(pIB);
		pIB->Unlock				();
	}

	CIndexStream()				{ _clear();				};
	~CIndexStream()				{ Destroy();			};
};

class ENGINE_API CSharedStreams
{
public:
	CVertexStream			Vertex;
	CIndexStream			Index;
public:
	void	OnDeviceCreate	()
	{
		Vertex.Create		();
		Index.Create		();
	}
	void	OnDeviceDestroy	()
	{
		Index.Destroy		();
		Vertex.Destroy		();
	}
	void	BeginFrame		()
	{
		Vertex.Flush		();
		Index.Flush			();
	}
};

#endif // !defined(AFX_SHAREDPRIMITIVE_H__22244801_1B1B_47AE_976E_42A3325CF063__INCLUDED_)
