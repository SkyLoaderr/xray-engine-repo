#pragma once

#include "ISheduled.h"

class	ENGINE_API	CSheduler
{
private:
	struct Item
	{
		u32			dwTimeForExecute;
		u32			dwTimeOfLastExecute;
		ISheduled*	Object;
		u32			dwPadding;				// for align-issues

		IC bool		operator < (Item& I)
		{	return dwTimeForExecute > I.dwTimeForExecute; }
	};
private:
	xr_vector<Item>	ItemsRT;
	xr_vector<Item>	Items;

	IC void			Push	(Item& I);
	IC void			Pop		();
	IC Item&		Top		()
	{
		return Items.front();
	}
public:
	u64				cycles_start;
	u64				cycles_limit;
	BOOL			fibered;
public:
	void			ProcessStep	();
	void			Process		();
	void			Update		();

	void			Switch		();
	IC void			Slice		()
	{
		if ((CPU::GetCycleCount()-cycles_start)>cycles_limit)
			Switch();
	}

	void			Register	(ISheduled* A, BOOL RT=FALSE );
	void			Unregister	(ISheduled* A	);
	void			EnsureOrder	(ISheduled* Before, ISheduled* After);

	void			Initialize	();
	void			Destroy		();
};
