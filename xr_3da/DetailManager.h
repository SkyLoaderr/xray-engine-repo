// DetailManager.h: interface for the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DETAILMANAGER_H__2C7B9CBD_4751_4D3E_8020_4792B800E4E2__INCLUDED_)
#define AFX_DETAILMANAGER_H__2C7B9CBD_4751_4D3E_8020_4792B800E4E2__INCLUDED_
#pragma once

#include "xr_list.h"
#include "xrpool.h"
#include "detailformat.h"
#include "detailmodel.h"

const int		dm_max_objects	= 64;
const int		dm_obj_in_slot	= 4;
const int		dm_size			= 20;
const int		dm_cache_line	= dm_size+1+dm_size;
const int		dm_cache_size	= dm_cache_line*dm_cache_line;
const float		dm_fade			= float(2*dm_size)-.5f;
const float		dm_slot_size	= DETAIL_SLOT_SIZE;
const float		dm_slot_radius	= DETAIL_SLOT_SIZE*0.70710678118654752440084436210485f; // (slot_size/2)*sqrtf(2)

class ENGINE_API CDetailManager  
{
public:
	struct	SlotItem
	{
		Fvector				P;
		float				scale;
		float				C;
		DWORD				C_dw;
		float				scale_calculated;           
		Fmatrix				mRotY;
		DWORD				vis_ID;
	};
	struct	SlotPart
	{
		DWORD				id;	
		vector<SlotItem*>	items;
	};
	enum	SlotType
	{
		stReady		= 0,	// Ready to use
		stPending,			// Pending for decompression

		stFORCEDWORD = 0xffffffff
	};
	struct Slot 
	{
		DWORD				type;
		int					sx,sz;
		Fbox				BB;
		SlotPart			G[dm_obj_in_slot];

		Slot()				{ type=stReady; sx=sz=0; BB.invalidate(); }
	};

	typedef		vector <vector<SlotItem*> >			vis_list;
	typedef		svector<CDetail*,dm_max_objects>	DetailVec;
	typedef		DetailVec::iterator					DetailIt;
	typedef		poolSS<SlotItem,4096>				PSS;
public:	
	int								dither			[16][16];
public:
	CStream*						dtFS;
	DetailHeader					dtH;
	DetailSlot*						dtSlots;		// note: pointer into VFS
	DetailSlot						DS_empty;

public:
	DetailVec						objects;
	vis_list						visible	[3];	// 0=still, 1=Wave1, 2=Wave2

	Slot*							cache[dm_cache_line][dm_cache_line];		// grid-cache itself
	svector<Slot*,dm_cache_size>	cache_task;									// non-unpacked slots
	Slot							cache_pool[dm_cache_size];					// just memory for slots
	int								cache_cx;
	int								cache_cz;

	PSS								poolSI;
public:

#ifdef _EDITOR
	virtual ObjectList*				GetSnapObjects	()=0;
#endif
	IC bool							UseVS			()		{ return HW.Caps.vertex.dwVersion >= CAP_VERSION(1,1); }

	// Software processor
	CVS*							soft_VS;
	void							soft_Load		();
	void							soft_Unload		();
	void							soft_Render		();

	// Hardware processor
	CVS*							hw_VS_wave;
	CVS*							hw_VS_still;
	DWORD							hw_BatchSize;
	IDirect3DVertexBuffer8*			hw_VB;
	IDirect3DIndexBuffer8*			hw_IB;
	void							hw_Load			();
	void							hw_Unload		();
	void							hw_Render		();
	void							hw_Render_dump	(CVS* vs, vis_list& vis, DWORD c_base);

public:
	DetailSlot&						QueryDB			(int sx, int sz);

	void							cache_Initialize();
	void							cache_Update	(int sx, int sz, Fvector& view, int limit);
	void							cache_Task		(int gx, int gz, Slot* D);
	Slot*							cache_Query		(int sx, int sz);
	void							cache_Decompress(Slot* D);
	BOOL							cache_Validate	();
	int								cg2w_X			(int x)			{ return cache_cx-dm_size+x;					}
	int								cg2w_Z			(int z)			{ return cache_cz-dm_size+(dm_cache_line-1-z);	}
	int								w2cg_X			(int x)			{ return x-cache_cx+dm_size;					}
	int								w2cg_Z			(int z)			{ return cache_cz-dm_size+(dm_cache_line-1-z);	}
	
	void							Load			();
	void							Unload			();
	void							Render			(Fvector& EYE);
									
	CDetailManager					();
	virtual ~CDetailManager			();
};									
									
#endif // !defined(AFX_DETAILMANAGER_H__2C7B9CBD_4751_4D3E_8020_4792B800E4E2__INCLUDED_)
