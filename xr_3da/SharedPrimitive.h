// SharedPrimitive.h: interface for the CSharedPrimitive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAREDPRIMITIVE_H__22244801_1B1B_47AE_976E_42A3325CF063__INCLUDED_)
#define AFX_SHAREDPRIMITIVE_H__22244801_1B1B_47AE_976E_42A3325CF063__INCLUDED_
#pragma once

#define VSTREAM_ALIGN	512

extern DWORD rsVStream_Scale;

enum LOCK_FLAGS
{
	LOCKFLAGS_FLUSH  = /* D3DLOCK_NOSYSLOCK | */ D3DLOCK_DISCARD,
	LOCKFLAGS_APPEND = /* D3DLOCK_NOSYSLOCK | */ D3DLOCK_NOOVERWRITE
};

class ENGINE_API CVertexStream  
{
	friend class				CSharedStreams;
	friend class				CDraw;
	friend class				CRender;
private :
	IDirect3DVertexBuffer8*		pVB;
	DWORD						mFVF;
	DWORD						mStride;
	DWORD						mCount;		// requisted count
	DWORD						mSize;		// real size (usually mCount, aligned on 512b boundary)
	DWORD						mPosition;
private:
	void						_clear	()
	{ ZeroMemory	(this,sizeof(*this)); }
	void						Create	();
	void						Destroy	();
public:
	IC IDirect3DVertexBuffer8*	getBuffer() { return pVB;		}
	IC DWORD					getFVF()	{ return mFVF;		}
	IC DWORD					getStride()	{ return mStride;	}

	BOOL						RequestStorage(DWORD vCount)
	{
		if (0==pVB) {
			mCount = _MAX	(mCount,vCount);
			return TRUE;
		} else {
			if (vCount<=mSize) return TRUE;
			else return FALSE;
		}
	}
	// Use at beginning of frame to force a flush of VB contents on first draw
	void						RequestFlush() { mPosition=mSize; }

	IC void*					Lock( DWORD Count, DWORD& vOffset )
	{
		vOffset				= 0;
		BYTE* pLockedData	= 0;
		
		// Ensure there is enough space in the VB for this data
		if (0==pVB) Create (); R_ASSERT(Count<mSize);

		// If either user forced us to flush,
		// or there is not enough space for the vertex data,
		// then flush the buffer contents
		//
		DWORD dwFlags = LOCKFLAGS_APPEND;
		if ( ( Count + mPosition ) >= mSize )
		{
			mPosition	= 0;						// clear position
			dwFlags		= LOCKFLAGS_FLUSH;			// discard it's contens
		}
		
		pVB->Lock( mPosition * mStride, Count * mStride, &pLockedData, dwFlags);
		VERIFY(pLockedData);
		
		vOffset		=	mPosition;

		return LPVOID(pLockedData);
	}
	IC void						Unlock(DWORD RealCount)
	{
		mPosition	+=	RealCount;

		VERIFY(pVB);
		pVB->Unlock();
	}
	IC DWORD					Stride() { return mStride; }

	CVertexStream()			{ _clear();				};
	CVertexStream(DWORD F)	{ _clear();	mFVF = F; mStride = D3DXGetFVFVertexSize(F); };
	~CVertexStream()		{ Destroy();			};
};

class ENGINE_API CIndexStream  
{
	friend class				CSharedStreams;
	friend class				CDraw;
	friend class				CRender;

	enum						{ mStride=2ul	};
private :
	IDirect3DIndexBuffer8*		pIB;
	DWORD						mCount;		// requisted count
	DWORD						mSize;		// real size (usually mCount, aligned on 512b boundary)
	DWORD						mPosition;
private:
	void						_clear	()
	{ 
		ZeroMemory				(this,sizeof(*this)); 
		mCount					= 3*4096;
	}
	void						Create	();
	void						Destroy	();
public:
	IC IDirect3DIndexBuffer8*	getBuffer() { return pIB; }

	BOOL						RequestStorage(DWORD iCount)
	{
		if (0==pIB) {
			mCount = _MAX(mCount,iCount);
			return TRUE;
		} else {
			if (iCount<=mSize) return TRUE;
			else return FALSE;
		}
	}
	// Use at beginning of frame to force a flush of VB contents on first draw
	void						RequestFlush() { mPosition=mSize; }

	IC WORD*					Lock( DWORD Count, DWORD& vOffset )
	{
		vOffset				= 0;
		BYTE* pLockedData	= 0;
		
		// Ensure there is enough space in the VB for this data
		if (0==pIB) Create (); R_ASSERT(Count<mSize);

		// If either user forced us to flush,
		// or there is not enough space for the index data,
		// then flush the buffer contents
		//
		DWORD dwFlags = LOCKFLAGS_APPEND;
		if ( ( Count + mPosition ) >= mSize )
		{
			mPosition	= 0;						// clear position
			dwFlags		= LOCKFLAGS_FLUSH;			// discard it's contens
		}
		
		pIB->Lock( mPosition * mStride, Count * mStride, &pLockedData, dwFlags);
		VERIFY(pLockedData);
		
		vOffset			=	mPosition;

		return					LPWORD(pLockedData);
	}
	IC void						Unlock(DWORD RealCount)
	{
		mPosition		+=	RealCount;
		VERIFY			(pIB);
		pIB->Unlock		();
	}
	IC DWORD					Remain() { return mSize-mPosition; }

	CIndexStream()			{ _clear();				};
	~CIndexStream()			{ Destroy();			};
};

class ENGINE_API CSharedStreams
{
	typedef svector<CVertexStream,256>	vecS;
	typedef vecS::iterator				vecSIT;
private:
	vecS								Vertex;
	CIndexStream						Index;
public:
	CVertexStream*						Create	(DWORD FVF, DWORD Vcnt)
	{
		vecSIT I = Vertex.begin(), E=Vertex.end();
		for (; I!=E; I++) {
			if (I->mFVF!=FVF)				continue;
			if (I->RequestStorage(Vcnt))	return I;
		}

		// Here we have to add yet another VB
		Vertex.push_back(CVertexStream(FVF));
		Vertex.back().RequestStorage(Vcnt);
		return &Vertex.back();
	}
	CIndexStream*						Get_IB() { return &Index; }

	void	OnDeviceDestroy	()
	{
		vecSIT I = Vertex.begin();
		for (; I!=Vertex.end(); I++)	I->Destroy();
		Index.Destroy		();
	}
	void	BeginFrame		()
	{
		vecSIT I = Vertex.begin();
		for (; I!=Vertex.end(); I++)	I->RequestFlush();
		Index.RequestFlush	();
	}
};

#endif // !defined(AFX_SHAREDPRIMITIVE_H__22244801_1B1B_47AE_976E_42A3325CF063__INCLUDED_)
