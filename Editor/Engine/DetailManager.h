// DetailManager.h: interface for the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DETAILMANAGER_H__2C7B9CBD_4751_4D3E_8020_4792B800E4E2__INCLUDED_)
#define AFX_DETAILMANAGER_H__2C7B9CBD_4751_4D3E_8020_4792B800E4E2__INCLUDED_
#pragma once

#include "xr_list.h"
#include "detailformat.h"

const int		dm_max_objects	= 32;
const int		dm_obj_in_slot	= 4;
const int		dm_size			= 7;
const int		dm_cache_line	= 1+dm_size+1+dm_size+1;
const int		dm_cache_size	= dm_cache_line*dm_cache_line;

class ENGINE_API CDetailManager;

class ENGINE_API CDetail 
{
public:
	struct fvfVertexIn
	{
		Fvector P;
		float	u,v;
	};
	struct fvfVertexOut
	{
		Fvector P;
		DWORD	C;
		float	u,v;
	};
public:
	Shader*		shader;	
	DWORD		flags;	
	float		s_min;
	float		s_max;
	float		radius;

	fvfVertexIn	*vertices;
	DWORD		number_vertices;
	WORD		*indices;
	DWORD		number_indices;

	void		Load		(CStream* S);
	void		Unload		();
	void		Transfer	(Fmatrix& mXform, fvfVertexOut* vDest, DWORD C, WORD* iDest, DWORD iOffset);
};

class ENGINE_API CDetailManager  
{
public:
	struct	SlotItem
	{
		Fvector	P;
		float	scale;
		float	phase_x;
		float	phase_z;
		DWORD	C;
		
		float	scale_calculated;
		Fmatrix	mRotY;
	};
	struct	SlotPart
	{
		DWORD				id;	
		CList<SlotItem>		items;
	};
	enum	SlotType
	{
		stReady		= 0,	// Ready to use
		stInvalid,			// Invalid cache entry
		stPending,			// Pending for decompression

		stFORCEDWORD = 0xffffffff
	};
	struct Slot 
	{
		DWORD				type;
		int					sx,sz;
		DWORD				dwFrameUsed;	// LRU cache
		Fbox				BB;
		SlotPart			G[dm_obj_in_slot];
	};
public:	
	int						dither			[16][16];
public:
	CVirtualFileStream*		dtFS;
	DetailHeader			dtH;
	DetailSlot*				dtSlots;		// note: pointer into VFS
public:
	svector<CDetail,dm_max_objects>			objects;
	svector<Slot,dm_cache_size>				cache;

	svector<CList<SlotItem*>,dm_max_objects> visible;
public:
	CVertexStream*			VS;

	void					Decompress		(int sx, int sz, Slot& D);
	Slot&					Query			(int sx, int sz);
	DetailSlot&				QueryDB			(int sx, int sz);
	void					UpdateCache		(int limit);
	
	void					Load			();
	void					Unload			();
	void					Render			(Fvector& EYE);

	CDetailManager			();
	virtual ~CDetailManager	();
};

#endif // !defined(AFX_DETAILMANAGER_H__2C7B9CBD_4751_4D3E_8020_4792B800E4E2__INCLUDED_)
