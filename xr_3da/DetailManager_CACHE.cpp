#include "stdafx.h"
#include "DetailManager.h"

void CDetailManager::cache_Initialize	()
{
	// Centroid
	cache_cx			= 0;
	cache_cz			= 0;

	// Initialize cache-grid
	Slot*	slt = cache_pool;
	for (u32 i=0; i<dm_cache_line; i++)
		for (u32 j=0; j<dm_cache_line; j++, slt++)
		{
			cache			[i][j]	= slt;
			cache_Task		(j,i,slt);
		}
	R_ASSERT	(cache_Validate());
}

CDetailManager::Slot*	CDetailManager::cache_Query	(int r_x, int r_z)
{
	int			gx		= w2cg_X(r_x + cache_cx);	VERIFY	(gx>=0 && gx<dm_cache_line);
	int			gz		= w2cg_Z(r_z + cache_cz);	VERIFY	(gz>=0 && gz<dm_cache_line);
	return		cache	[gz][gx];
}

void 	CDetailManager::cache_Task		(int gx, int gz, Slot* D)
{
	int sx					= cg2w_X	(gx);
	int sz					= cg2w_Z	(gz);
	DetailSlot&	DS			= QueryDB	(sx,sz);

	// Unpacking
	u32 old_type			= D->type;
	D->type					= stPending;
	D->sx					= sx;
	D->sz					= sz;

	D->BB.min.set			(sx*dm_slot_size,			DS.y_min,	sz*dm_slot_size);
	D->BB.max.set			(D->BB.min.x+dm_slot_size,	DS.y_max,	D->BB.min.z+dm_slot_size);
	D->BB.grow				(EPS_L);

	for (int i=0; i<dm_obj_in_slot; i++)
	{
		D->G[i].id			= DS.items[i].id;
		for (u32 clr=0; clr<D->G[i].items.size(); clr++)
			poolSI.destroy(D->G[i].items[clr]);
		D->G[i].items.clear	();
	}

	if (old_type != stPending)
	{
		cache_task.push_back(D);
	}
}


BOOL	CDetailManager::cache_Validate	()
{
	for (int z=0; z<dm_cache_line; z++)
	{
		for (int x=0; x<dm_cache_line; x++)
		{
			int		w_x		= cg2w_X(x);
			int		w_z		= cg2w_Z(z);
			Slot*	D		= cache[z][x];

			if (D->sx	!= w_x)	return FALSE;
			if (D->sz	!= w_z)	return FALSE;
		}
	}
	return TRUE;
}

void	CDetailManager::cache_Update	(int v_x, int v_z, Fvector& view, int limit)
{
	// *****	Cache shift
	while (cache_cx!=v_x)
	{
		if (v_x>cache_cx)	{
			// shift matrix to left
			cache_cx ++;
			for (int z=0; z<dm_cache_line; z++)
			{
				Slot*	S	= cache[z][0];
				for			(int x=1; x<dm_cache_line; x++)		cache[z][x-1] = cache[z][x];
				cache		[z][dm_cache_line-1] = S;
				cache_Task	(dm_cache_line-1, z, S);
			}
			// R_ASSERT	(cache_Validate());
		} else {
			// shift matrix to right
			cache_cx --;
			for (int z=0; z<dm_cache_line; z++)
			{
				Slot*	S	= cache[z][dm_cache_line-1];
				for			(int x=dm_cache_line-1; x>0; x--)	cache[z][x] = cache[z][x-1]; 
				cache		[z][0]	= S;
				cache_Task	(0,z,S);
			}
			// R_ASSERT	(cache_Validate());
		}
	}
	while (cache_cz!=v_z)	
	{
		if (v_z>cache_cz)	{
			// shift matrix down a bit
			cache_cz ++;
			for (int x=0; x<dm_cache_line; x++)
			{
				Slot*	S	= cache[dm_cache_line-1][x];
				for			(int z=dm_cache_line-1; z>0; z--)	cache[z][x] = cache[z-1][x];
				cache		[0][x]	= S;
				cache_Task	(x,0,S);
			}
			// R_ASSERT	(cache_Validate());
		} else {
			// shift matrix up
			cache_cz --;
			for (int x=0; x<dm_cache_line; x++)
			{
				Slot*	S	= cache[0][x];
				for			(int z=1; z<dm_cache_line; z++)		cache[z-1][x] = cache[z][x];
				cache		[dm_cache_line-1][x]	= S;
				cache_Task	(x,dm_cache_line-1,S);
			}
			// R_ASSERT	(cache_Validate());
		}
	}

	// Task performer
	if (cache_task.size() == dm_cache_size)	limit = dm_cache_size;
	for (int iteration=0; cache_task.size() && (iteration<limit); iteration++)
	{
		u32	best_id		= 0;
		float	best_dist	= flt_max;

		for (u32 entry=0; entry<cache_task.size(); entry++)
		{
			// Gain access to data
			Slot*		S	= cache_task[entry];
			VERIFY		(stPending == S->type);

			// Estimate
			Fvector		C;
			S->BB.getcenter	(C);
			float		D	= view.distance_to_sqr	(C);

			// Select 
			if (D<best_dist)
			{
				best_dist	= D;
				best_id		= entry;
			}
		}

		// Decompress and remove task
		cache_Decompress	(cache_task[best_id]);
		cache_task.erase	(best_id);
	}
}

DetailSlot&	CDetailManager::QueryDB(int sx, int sz)
{
	int db_x = sx+dtH.offs_x;
	int db_z = sz+dtH.offs_z;
	if ((db_x>=0) && (db_x<int(dtH.size_x)) && (db_z>=0) && (db_z<int(dtH.size_z)))
	{
		u32 linear_id				= db_z*dtH.size_x + db_x;
		return dtSlots				[linear_id];
	} else {
		// Empty slot
		DS_empty.y_min				= 0;
		DS_empty.y_max				= EPS_L;

		DS_empty.items[0].id		= 0xff;
		DS_empty.items[1].id		= 0xff;
		DS_empty.items[2].id		= 0xff;
		DS_empty.items[3].id		= 0xff;

		DS_empty.color.a0			= 8;
		DS_empty.color.a1			= 8;
		DS_empty.color.a2			= 8;
		DS_empty.color.a3			= 8;
		return DS_empty;
	}
}
