// DetailManager.h: interface for the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DETAILMANAGER_H__2C7B9CBD_4751_4D3E_8020_4792B800E4E2__INCLUDED_)
#define AFX_DETAILMANAGER_H__2C7B9CBD_4751_4D3E_8020_4792B800E4E2__INCLUDED_
#pragma once

#include "xr_list.h"
#include "detailformat.h"
#include "detailmodel.h"

const int		dm_max_objects	= 64;
const int		dm_obj_in_slot	= 4;
const int		dm_size			= 10;
const int		dm_cache_line	= 1+dm_size+1+dm_size+1;
const int		dm_cache_size	= dm_cache_line*dm_cache_line;
const float		dm_fade			= float(2*dm_size)-.5f;

DEFINE_SVECTOR(CDetail*,dm_max_objects,DetailVec,DetailIt);

class ENGINE_API CDetailManager  
{
public:
	struct	SlotItem
	{
		Fvector	P;
		float	scale;
		float	C;
		DWORD	C_dw;
		float	scale_calculated;           
		Fmatrix	mRotY;
		DWORD	vis_ID;
	};
	struct	SlotPart
	{
		DWORD				id;	
		vector<SlotItem>	items;
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

	typedef svector<vector<SlotItem*>,dm_max_objects>	vis_list;
public:	
	int						dither			[16][16];
public:
	CStream*				dtFS;
	DetailHeader			dtH;
	DetailSlot*				dtSlots;		// note: pointer into VFS
	DetailSlot				DS_empty;
public:
	DetailVec				objects;
	svector<Slot,dm_cache_size>		cache;
	vis_list						visible[3];	// 0=still, 1=Wave1, 2=Wave2
public:
#ifdef _EDITOR
	virtual ObjectList*		GetSnapObjects	()=0;
#endif
	IC bool					UseVS			()		{ return HW.Caps.vertex.dwVersion >= CAP_VERSION(1,1); }

	// Software processor
	CVS*					soft_VS;
	void					soft_Load		();
	void					soft_Unload		();
	void					soft_Render		();

	// Hardware processor
	CVS*					hw_VS_wave;
	CVS*					hw_VS_still;
	DWORD					hw_BatchSize;
	IDirect3DVertexBuffer8*	hw_VB;
	IDirect3DIndexBuffer8*	hw_IB;
	void					hw_Load			();
	void					hw_Unload		();
	void					hw_Render		();
	void					hw_Render_dump	(CVS* vs, vis_list& vis, DWORD c_base);

public:
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
