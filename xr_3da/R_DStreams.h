#ifndef r_DStreamsH
#define r_DStreamsH
#pragma once

enum
{
	LOCKFLAGS_FLUSH		= D3DLOCK_DISCARD,
	LOCKFLAGS_APPEND	= D3DLOCK_NOOVERWRITE
};

class ENGINE_API _VertexStream
{
private :
	IDirect3DVertexBuffer9*		pVB;
	u32							mSize;			// size in bytes
	u32							mPosition;		// position in bytes
	u32							mDiscardID;		// ID of discard - usually for caching
#ifdef DEBUG
	u32							dbg_lock;
#endif
private:
	void						_clear			()
	{
		pVB			= NULL;
		mSize		= 0;
		mPosition	= 0;
		mDiscardID	= 0;
		dbg_lock	= 0;
	}
public:
	void						Create			();
	void						Destroy			();

	IC IDirect3DVertexBuffer9*	Buffer()		{ return pVB;			}
	IC u32						DiscardID()		{ return mDiscardID;	}
	IC void						Flush()			{ mPosition=mSize;		}

	void*						Lock			( u32 vl_Count, u32 Stride, u32& vOffset );
	void						Unlock			( u32 Count, u32 Stride);

	_VertexStream()			{ _clear();				};
	~_VertexStream()		{ Destroy();			};
};

class ENGINE_API _IndexStream
{
private :
	IDirect3DIndexBuffer9*		pIB;
	u32							mSize;		// real size (usually mCount, aligned on 512b boundary)
	u32							mPosition;
	u32							mDiscardID;
private:
	void						_clear	()
	{
		pIB			= NULL;
		mSize		= 0;
		mPosition	= 0;
		mDiscardID	= 0;
	}
public:
	void						Create	();
	void						Destroy	();

	IC IDirect3DIndexBuffer9*	Buffer()		{ return pIB;			}
	IC u32						DiscardID()		{ return mDiscardID;	}
	void						Flush()			{ mPosition=mSize;		}

	IC u16*						Lock			( u32 Count, u32& vOffset )
	{
		PGO						(Msg("PGO:IB_LOCK:%d",Count));
		vOffset					= 0;
		BYTE* pLockedData		= 0;

		// Ensure there is enough space in the VB for this data
		R_ASSERT				((2*Count<=mSize) && Count);

		// If either user forced us to flush,
		// or there is not enough space for the index data,
		// then flush the buffer contents
		u32 dwFlags = LOCKFLAGS_APPEND;
		if ( 2*( Count + mPosition ) >= mSize )
		{
			mPosition	= 0;						// clear position
			dwFlags		= LOCKFLAGS_FLUSH;			// discard it's contens
			mDiscardID	++;
		}
		pIB->Lock				( mPosition * 2, Count * 2, (void**) &pLockedData, dwFlags);
		VERIFY					(pLockedData);

		vOffset					=	mPosition;

		return					LPWORD(pLockedData);
	}
	IC void						Unlock(u32 RealCount)
	{
		PGO						(Msg("PGO:IB_UNLOCK:%d",RealCount));
		mPosition				+=	RealCount;
		VERIFY					(pIB);
		pIB->Unlock				();
	}

	_IndexStream()				{ _clear();				};
	~_IndexStream()				{ Destroy();			};
};
#endif
